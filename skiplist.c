#include <Python.h>

#define MAX_LEVEL 16
#define PROBABILITY 0.25


typedef struct Node {
    PyObject *value;
    struct Node *forwards[MAX_LEVEL];
} Node;


typedef struct {
    PyObject_VAR_HEAD
    Node head;
    int level;
} SortedSet;


static int
random_level(void)
{
    static const unsigned int p = PROBABILITY * 0xFFFF;
    int l = 1;
    while ((random() & 0xFFFF) < p)
        ++l;
    return l < MAX_LEVEL ? l : MAX_LEVEL;
}


static void
_SortedSet_dealloc(Node *node)
{
    if (node != NULL) {
        _SortedSet_dealloc(node->forwards[1]);
        Py_DECREF(node->value);
        PyMem_RawFree(node);
    }
}


static void
SortedSet_dealloc(SortedSet *self)
{
    _SortedSet_dealloc(self->head.forwards[1]);
}


static PyObject *
SortedSet_insert(SortedSet *self, PyObject *args) {
    PyObject *v;
    if (!PyArg_UnpackTuple(args, "insert", 1, 1, &v))
        return NULL;

    Node *next;
    Node *update[MAX_LEVEL];
    Node *x = &self->head;

    for (int i = self->level; i >= 1; i--) {
        next = x->forwards[i];
        while (next != NULL && PyObject_RichCompareBool(next->value, v, Py_LT)) {
            x = next;
            next = next->forwards[i];
        }
        update[i] = x;
    }

    next = x->forwards[1];
    if (next != NULL && PyObject_RichCompareBool(next->value, v, Py_EQ)) {
        // it's already in the set
        printf("IT IS ALREADY IN THE SET\n");
    } else {
        int lvl = random_level();
        if (lvl > self->level) {
            for (int i = self->level + 1; i <= lvl; ++i)
                update[i] = &self->head;
            self->level = lvl;
        }

        Node *node = PyMem_RawMalloc(sizeof(Node));
        if (node == NULL)
            return PyErr_NoMemory();
        Py_INCREF(v);
        node->value = v;

        for (int i = 1; i <= lvl; ++i) {
            node->forwards[i] = update[i]->forwards[i];
            update[i]->forwards[i] = node;
        }

        Py_SIZE(self) += 1;
    }

    for (Node *p = &self->head; p != NULL; p = p->forwards[1]) {
        printf("%s\n", PyUnicode_AsUTF8(PyObject_Repr(p->value)));
    }

    Py_RETURN_NONE;
}


static Py_ssize_t
SortedSet_length(SortedSet *self)
{
    return Py_SIZE(self);
}


static PyMethodDef SortedSet_methods[] = {
    {"insert", (PyCFunction)SortedSet_insert, METH_VARARGS,
     "insert an element into the list"
    },
    {NULL}  /* Sentinel */
};

static PySequenceMethods skiplist_as_sequence = {
    (lenfunc)SortedSet_length,                      /* sq_length */
    0
};


static PyTypeObject SortedSetType = {
    PyVarObject_HEAD_INIT(NULL, 0)
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
    &skiplist_as_sequence,         /* tp_as_sequence */
    0,                             /* tp_as_mapping */
    PyObject_HashNotImplemented,   /* tp_hash  */
    0,                             /* tp_call */
    0,                             /* tp_str */
    0,                             /* tp_getattro */
    0,                             /* tp_setattro */
    0,                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,            /* tp_flags */
    "SortedSet Objects",           /* tp_doc */
    0,                             /* tp_traverse */
    0,                             /* tp_clear */
    0,                             /* tp_richcompare */
    0,                             /* tp_weaklistoffset */
    0,                             /* tp_iter */
    0,                             /* tp_iternext */
    SortedSet_methods,             /* tp_methods */
    0,                             /* tp_members */
    0,                             /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    0,                             /* tp_descr_get */
    0,                             /* tp_descr_set */
    0,                             /* tp_dictoffset */
    0,                             /* tp_init */
    0,                             /* tp_alloc */
    PyType_GenericNew,             /* tp_new */
};


static PyModuleDef skiplistmodule = {
    PyModuleDef_HEAD_INIT,
    "skiplist",
    "SkipList implementation",
    -1
};


PyMODINIT_FUNC
PyInit_skiplist(void)
{
    PyObject *m;
    if (PyType_Ready(&SortedSetType) < 0)
        return NULL;

    m = PyModule_Create(&skiplistmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&SortedSetType);
    PyModule_AddObject(m, "SortedSet", (PyObject *)&SortedSetType);
    return m;
}
