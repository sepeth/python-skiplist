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


static PyObject *
add(SortedSet *self, PyObject *arg)
{
    Node *next;
    Node *update[MAX_LEVEL];
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
        update[i] = x;
    }

    next = x->forwards[0];
    if (next != NULL && EQUAL(next->value, arg)) {
        Py_INCREF(arg);
        Py_DECREF(next->value);
        next->value = arg;
    } else {
        int lvl = random_level();
        if (lvl > self->level) {
            for (i = self->level + 1; i <= lvl; ++i)
                update[i] = &self->head;
            self->level = lvl;
        }

        Node *node = PyMem_RawMalloc(sizeof(Node));
        if (node == NULL)
            return PyErr_NoMemory();
        Py_INCREF(arg);
        node->value = arg;

        for (i = 0; i <= lvl; ++i) {
            node->forwards[i] = update[i]->forwards[i];
            update[i]->forwards[i] = node;
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
        if (add(self, key) == NULL) {
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
SortedSet_add(SortedSet *self, PyObject *args) {
    PyObject *v;
    if (!PyArg_UnpackTuple(args, "add", 1, 1, &v))
        return NULL;
    return add(self, v);
}


static PyObject *
SortedSet_remove(SortedSet *self, PyObject *args)
{
    PyObject *v;
    if (!PyArg_UnpackTuple(args, "remove", 1, 1, &v))
        return NULL;

    Node *next;
    Node *update[MAX_LEVEL];
    Node *x = &self->head;
    Py_ssize_t i;
    int cmp;

    for (i = self->level; i >= 0; --i) {
        next = x->forwards[i];
        while (next != NULL && (cmp = LESSTHAN(next->value, v))) {
            if (cmp == -1)
                return NULL;
            x = next;
            next = next->forwards[i];
        }
        update[i] = x;
    }

    if (next != NULL && EQUAL(next->value, v)) {
        for (i = self->level; i >= 0; --i) {
            if (update[i]->forwards[i] == next) {
                update[i]->forwards[i] = next->forwards[i];
            }
        }
        Py_DECREF(next->value);
        Py_SIZE(self) -= 1;
        PyMem_RawFree(next);
    } else {
        PyErr_Format(PyExc_KeyError, "%R is not in the SortedSet", v);
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject *
SortedSet_print(SortedSet *self, PyObject *v) {
    Node *p;
    for (p = &self->head; p != NULL; p = p->forwards[0]) {
        printf("%s\n", PyUnicode_AsUTF8(PyObject_Repr(p->value)));
    }
    Py_RETURN_NONE;
}


static PyObject *
SortedSet_subscript(SortedSet *self, PyObject *key)
{
    Node *next;
    Node *update[MAX_LEVEL];
    Node *x = &self->head;
    Py_ssize_t i;
    int cmp;

    for (i = self->level; i >= 0; --i) {
        next = x->forwards[i];
        while (next != NULL && (cmp = LESSTHAN(next->value, key))) {
            if (cmp == -1)
                return NULL;
            x = next;
            next = next->forwards[i];
        }
        update[i] = x;
    }

    if (next != NULL && EQUAL(next->value, key)) {
        Py_INCREF(next->value);
        return next->value;
    } else {
        PyErr_Format(PyExc_KeyError, "%R is not in the SortedSet", key);
        return NULL;
    }
}


static Py_ssize_t
SortedSet_length(SortedSet *self)
{
    return Py_SIZE(self);
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
    {"add", (PyCFunction)SortedSet_add, METH_VARARGS,
     "add an element into the list"},
    {"print", (PyCFunction)SortedSet_print, METH_NOARGS,
     "print the list"},
    {"remove", (PyCFunction)SortedSet_remove, METH_VARARGS,
     "remove an element from the list"},
    {"__getitem__", (PyCFunction)SortedSet_subscript, METH_O,
     "get an element from the list"},
    {NULL}  /* Sentinel */
};


static PySequenceMethods sortedset_as_sequence = {
    (lenfunc)SortedSet_length,                      /* sq_length */
    0
};


static PyMappingMethods sortedset_as_mapping = {
    (lenfunc)SortedSet_length,       /*mp_length*/
    (binaryfunc)SortedSet_subscript, /*mp_subscript*/
    0,                               /*mp_ass_subscript*/
};


static PyTypeObject SortedSetType = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "skiplist.SortedSet",          /* tp_name */
    sizeof(SortedSet),             /* tp_basicsize */
    0,                             /* tp_itemsize */
    (destructor)SortedSet_dealloc, /* tp_dealloc */
    0,                             /* tp_print */
    0,                             /* tp_getattr */
    0,                             /* tp_setattr */
    0,                             /* tp_reserved */
    0,                             /* tp_repr */
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
