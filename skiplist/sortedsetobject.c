#include <Python.h>

#define MAX_LEVEL 16
#define PROBABILITY 0.25

#define LESSTHAN(p, q) PyObject_RichCompareBool(p, q, Py_LT)
#define EQUAL(p, q)    PyObject_RichCompareBool(p, q, Py_EQ)


typedef struct Node {
    PyObject *value;
    struct Node *forwards[MAX_LEVEL];
} Node;


typedef struct {
    PyObject_VAR_HEAD
    Node head;
    int level;
} SortedSet;


typedef struct {
    PyObject_HEAD
    Node *node;
    SortedSet *self;
} SortedSetIter;


static int
random_level(void)
{
    static const unsigned int p = PROBABILITY * 0xFFFF;
    int l = 0;
    while ((random() & 0xFFFF) < p)
        ++l;
    return l < MAX_LEVEL ? l : MAX_LEVEL - 1;
}


static Node *
find_gt_or_eq(SortedSet *self, PyObject *arg, Node **update)
{
    Node *next = NULL;
    Node *x = &self->head;
    Py_ssize_t i;
    int cmp;

    for (i = self->level; i >= 0; i--) {
        next = x->forwards[i];
        while (next != NULL && (cmp = LESSTHAN(next->value, arg))) {
            if (cmp == -1)
                return NULL;
            x = next;
            next = next->forwards[i];
        }
        if (update != NULL)
            update[i] = x;
    }

    return next;
}


static PyObject *
SortedSet_add(SortedSet *self, PyObject *arg)
{
    Node *update[MAX_LEVEL];
    Py_ssize_t i;
    int lvl;
    Node *new_node;
    Node *next = find_gt_or_eq(self, arg, update);

    if (PyErr_Occurred())
        return NULL;

    if (next != NULL && EQUAL(next->value, arg)) {
        Py_INCREF(arg);
        Py_DECREF(next->value);
        next->value = arg;
    } else {
        lvl = random_level();
        if (lvl > self->level) {
            for (i = self->level + 1; i <= lvl; ++i)
                update[i] = &self->head;
            self->level = lvl;
        }

        new_node = PyMem_Malloc(sizeof(Node));
        if (new_node == NULL)
            return PyErr_NoMemory();
        Py_INCREF(arg);
        new_node->value = arg;

        for (i = 0; i <= lvl; ++i) {
            new_node->forwards[i] = update[i]->forwards[i];
            update[i]->forwards[i] = new_node;
        }

        Py_SIZE(self) += 1;
    }

    Py_RETURN_NONE;
}


static int
SortedSet_init(SortedSet *self, PyObject *args, PyObject *kwds)
{
    PyObject *iterable = NULL, *it, *key;

    if (!PyArg_ParseTuple(args, "|O:SortedSet", &iterable))
        return -1;

    if (iterable == NULL)
        return 0;

    it = PyObject_GetIter(iterable);
    if (it == NULL) {
        return -1;
    }

    while ((key = PyIter_Next(it)) != NULL) {
        if (SortedSet_add(self, key) == NULL) {
            Py_DECREF(it);
            Py_DECREF(key);
            return -1;
        }
        Py_DECREF(key);
    }
    Py_DECREF(it);

    if (PyErr_Occurred())
        return -1;

    return 0;
}


static PyObject *
SortedSet_remove(SortedSet *self, PyObject *arg)
{
    Node *next = NULL;
    Node *update[MAX_LEVEL];
    Py_ssize_t i;

    next = find_gt_or_eq(self, arg, update);

    if (PyErr_Occurred())
        return NULL;

    if (next != NULL && EQUAL(next->value, arg)) {
        for (i = self->level; i >= 0; --i) {
            if (update[i]->forwards[i] == next) {
                update[i]->forwards[i] = next->forwards[i];
            }
        }
        Py_DECREF(next->value);
        Py_SIZE(self) -= 1;
        PyMem_Free(next);
    } else {
        PyErr_Format(PyExc_KeyError, "%R is not in the SortedSet", arg);
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *
SortedSet_subscript(SortedSet *self, PyObject *key)
{
    Node *next = find_gt_or_eq(self, key, NULL);

    if (PyErr_Occurred())
        return NULL;

    if (next != NULL && EQUAL(next->value, key)) {
        Py_INCREF(next->value);
        return next->value;
    } else {
        PyErr_Format(PyExc_KeyError, "%R is not in the SortedSet", key);
        return NULL;
    }
}


static int
SortedSet_contains(SortedSet *self, PyObject *key)
{
    Node *next = find_gt_or_eq(self, key, NULL);
    return next != NULL && EQUAL(next->value, key);
}


static Py_ssize_t
SortedSet_length(SortedSet *self)
{
    return Py_SIZE(self);
}


static PyObject *
SortedSet_level(SortedSet *self)
{
    return PyLong_FromLong(self->level);
}


static PyObject *
SortedSet_repr(SortedSet *self)
{
    PyObject *result = NULL, *keys, *listrepr, *tmp;
    int status = Py_ReprEnter((PyObject*)self);

    if (status != 0) {
        if (status < 0)
            return NULL;
        return PyUnicode_FromFormat("%s(...)", Py_TYPE(self)->tp_name);
    }

    if (!Py_SIZE(self)) {
        Py_ReprLeave((PyObject*)self);
        return PyUnicode_FromFormat("%s()", Py_TYPE(self)->tp_name);
    }

    keys = PySequence_List((PyObject*)self);
    if (keys == NULL)
        goto done;

    listrepr = PyObject_Repr(keys);
    Py_DECREF(keys);
    if (listrepr == NULL)
        goto done;

    tmp = PyUnicode_Substring(listrepr, 1, PyUnicode_GET_LENGTH(listrepr)-1);
    Py_DECREF(listrepr);
    if (tmp == NULL)
        goto done;

    listrepr = tmp;
    result = PyUnicode_FromFormat("%s({%U})", Py_TYPE(self)->tp_name, listrepr);

done:
    Py_ReprLeave((PyObject*)self);
    return result;
}


static int
SortedSet_traverse(SortedSet *self, visitproc visit, void *arg)
{
    Node *next = NULL;
    Node *p;
    for (p = self->head.forwards[0]; p != NULL; p = next) {
        next = p->forwards[0];
        Py_VISIT(p->value);
    }
    return 0;
}


static void
setnull(Node *node)
{
    Py_ssize_t i;
    for (i = 0; i < MAX_LEVEL; i++) {
        node->forwards[i] = NULL;
    }
}


static int
SortedSet_clear(SortedSet *self)
{
    Node *next = NULL;
    Node *p;
    for (p = self->head.forwards[0]; p != NULL; p = next) {
        PyObject *value = p->value;
        p->value = NULL;
        next = p->forwards[0];
        setnull(p);
        Py_XDECREF(value);
        PyMem_FREE(p);
    }
    setnull(&self->head);
    return 0;
}


static void
SortedSet_dealloc(SortedSet *self)
{
    SortedSet_clear(self);
    PyObject_GC_Del((PyObject *)self);
}


static PyObject* SortedSet_iter(PyObject *self);
static PyObject* SortedSetIter_next(SortedSetIter *it);
static int SortedSetIter_traverse(SortedSetIter *it, visitproc visit, void *arg);
static void SortedSetIter_dealloc(SortedSetIter *it);


static PyMethodDef SortedSet_methods[] = {
    {"add", (PyCFunction)SortedSet_add, METH_O,
     "add an element into the list"},
    {"remove", (PyCFunction)SortedSet_remove, METH_O,
     "remove an element from the list"},
    {"__getitem__", (PyCFunction)SortedSet_subscript, METH_O,
     "get an element from the list"},
    {"level", (PyCFunction)SortedSet_level, METH_NOARGS,
     "level of the skiplist"},
    {NULL}  /* Sentinel */
};


static PySequenceMethods sortedset_as_sequence = {
    (lenfunc)SortedSet_length,      /* sq_length */
    0,                              /* sq_concat */
    0,                              /* sq_repeat */
    0,                              /* sq_item */
    0,                              /* sq_slice */
    0,                              /* sq_ass_item */
    0,                              /* sq_ass_slice */
    (objobjproc)SortedSet_contains, /* sq_contains */
    0,                              /* sq_inplace_concat */
    0,                              /* sq_inplace_repeat */
};


static PyMappingMethods sortedset_as_mapping = {
    (lenfunc)SortedSet_length,       /*mp_length*/
    (binaryfunc)SortedSet_subscript, /*mp_subscript*/
    0,                               /*mp_ass_subscript*/
};


static PyTypeObject SortedSetType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "SortedSet",                   /* tp_name */
    sizeof(SortedSet),             /* tp_basicsize */
    0,                             /* tp_itemsize */
    (destructor)SortedSet_dealloc, /* tp_dealloc */
    0,                             /* tp_print */
    0,                             /* tp_getattr */
    0,                             /* tp_setattr */
    0,                             /* tp_reserved */
    (reprfunc)SortedSet_repr,      /* tp_repr */
    0,                             /* tp_as_number */
    &sortedset_as_sequence,        /* tp_as_sequence */
    &sortedset_as_mapping,         /* tp_as_mapping */
    PyObject_HashNotImplemented,   /* tp_hash  */
    0,                             /* tp_call */
    0,                             /* tp_str */
    0,                             /* tp_getattro */
    0,                             /* tp_setattro */
    0,                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC, /* tp_flags */
    "SortedSet Objects",                     /* tp_doc */
    (traverseproc)SortedSet_traverse,        /* tp_traverse */
    (inquiry)SortedSet_clear,                /* tp_clear */
    0,                             /* tp_richcompare */
    0,                             /* tp_weaklistoffset */
    SortedSet_iter,                /* tp_iter */
    0,                             /* tp_iternext */
    SortedSet_methods,             /* tp_methods */
    0,                             /* tp_members */
    0,                             /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    0,                             /* tp_descr_get */
    0,                             /* tp_descr_set */
    0,                             /* tp_dictoffset */
    (initproc)SortedSet_init,      /* tp_init */
    PyType_GenericAlloc,           /* tp_alloc */
    PyType_GenericNew,             /* tp_new */
};


static PyTypeObject SortedSetIter_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "SortedSetIter",
    sizeof(SortedSetIter),
    0,                                          /* tp_itemsize */
    /* methods */
    (destructor)SortedSetIter_dealloc,          /* tp_dealloc */
    0,                                          /* tp_print */
    0,                                          /* tp_getattr */
    0,                                          /* tp_setattr */
    0,                                          /* tp_reserved */
    0,                                          /* tp_repr */
    0,                                          /* tp_as_number */
    0,                                          /* tp_as_sequence */
    0,                                          /* tp_as_mapping */
    0,                                          /* tp_hash */
    0,                                          /* tp_call */
    0,                                          /* tp_str */
    0,                                          /* tp_getattro */
    0,                                          /* tp_setattro */
    0,                                          /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,    /* tp_flags */
    0,                                          /* tp_doc */
    (traverseproc)SortedSetIter_traverse,       /* tp_traverse */
    0,                                          /* tp_clear */
    0,                                          /* tp_richcompare */
    0,                                          /* tp_weaklistoffset */
    PyObject_SelfIter,                          /* tp_iter */
    (iternextfunc)SortedSetIter_next,           /* tp_iternext */
    0,                                          /* tp_methods */
    0,                                          /* tp_members */
    0,                                          /* tp_getset */
    0,                                          /* tp_base */
    0,                                          /* tp_dict */
    0,                                          /* tp_descr_get */
    0,                                          /* tp_descr_set */
    0,                                          /* tp_dictoffset */
    0,                                          /* tp_init */
    PyType_GenericAlloc,                        /* tp_alloc */
    PyType_GenericNew,                          /* tp_new */
};


static PyModuleDef sortedsetmodule = {
    PyModuleDef_HEAD_INIT,
    "_sortedset",
    "SkipList implementation",
    -1
};


static PyObject *
SortedSet_iter(PyObject *self) {
    SortedSet *s = (SortedSet *)self;
    SortedSetIter *it = PyObject_GC_New(SortedSetIter, &SortedSetIter_Type);
    if (it == NULL) {
        return NULL;
    }
    it->node = s->head.forwards[0];
    it->self = s;
    Py_INCREF(self);
    PyObject_GC_Track(it);
    return (PyObject *)it;
}


static PyObject *
SortedSetIter_next(SortedSetIter *it)
{
    Node *node = it->node;
    if (node == NULL) {
        Py_DECREF(it->self);
        it->self = NULL;
        return NULL;
    }
    it->node = it->node->forwards[0];
    Py_INCREF(node->value);
    return node->value;
}


static void
SortedSetIter_dealloc(SortedSetIter *it)
{
    PyObject_GC_UnTrack(it);
    Py_XDECREF(it->self);
    PyObject_GC_Del(it);
}


static int
SortedSetIter_traverse(SortedSetIter *it, visitproc visit, void *arg)
{
    Py_VISIT(it->self);
    return 0;
}


PyMODINIT_FUNC
PyInit__sortedset(void)
{
    PyObject *m;
    if (PyType_Ready(&SortedSetType) < 0)
        return NULL;

    m = PyModule_Create(&sortedsetmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&SortedSetType);
    PyModule_AddObject(m, "SortedSet", (PyObject *)&SortedSetType);
    return m;
}
