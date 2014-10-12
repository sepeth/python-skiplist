#include <Python.h>

typedef struct Node {
    PyObject *value;
    struct Node *next;
} Node;


typedef struct {
    PyObject_VAR_HEAD
    Node *head;
} SortedSet;


static void
_SortedSet_dealloc(Node *node)
{
    if (node != NULL) {
        _SortedSet_dealloc(node->next);
        Py_DECREF(node->value);
        PyMem_RawFree(node);
    }
}


static void
SortedSet_dealloc(SortedSet *self)
{
    _SortedSet_dealloc(self->head);
}


static PyObject *
SortedSet_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SortedSet *self;
    
    self = (SortedSet*)type->tp_alloc(type, 0);
    if (self == NULL)
        return NULL;
    self->head = NULL;

    return (PyObject*) self;
}


static PyObject *
SortedSet_insert(SortedSet *self, PyObject *args) {
    PyObject *v;
    if (!PyArg_UnpackTuple(args, "insert", 1, 1, &v))
        return NULL;

    Node *node = PyMem_RawMalloc(sizeof(Node));
    if (node == NULL)
        return PyErr_NoMemory();

    Py_INCREF(v);
    node->value = v;
    node->next = self->head;
    self->head = node;
    Py_SIZE(self) += 1;
    Py_RETURN_NONE;
}


static Py_ssize_t
SortedSet_length(SortedSet *self) 
{
    return Py_SIZE(self);
}


static PyObject *
SortedSet_repr(SortedSet *self)
{
    Py_ssize_t i;
    PyObject *s;
    Node *p;
    _PyUnicodeWriter writer;
    
    if (Py_SIZE(self) == 0)
        return PyUnicode_FromString("[]");

    i = Py_ReprEnter((PyObject*)self);
    if (i != 0) {
        return i > 0 ? PyUnicode_FromString("[...]") : NULL;
    }

    _PyUnicodeWriter_Init(&writer);
    writer.overallocate = 1;
    /* "[" + "1" + ", 2" * (len - 1) + "]" */
    writer.min_length = 1 + 1 + (2 + 1) * (Py_SIZE(self) - 1) + 1;

    if (_PyUnicodeWriter_WriteChar(&writer, '[') < 0)
        goto error;

    for (i = 0, p = self->head; i < Py_SIZE(self); ++i) {
        if (i > 0) {
            if (_PyUnicodeWriter_WriteASCIIString(&writer, ", ", 2) < 0)
                goto error;
        }
        if (Py_EnterRecursiveCall(" while getting the repr of a list"))
            goto error;
        s = PyObject_Repr(p->value);
        Py_LeaveRecursiveCall();
        if (s == NULL)
            goto error;

        if (_PyUnicodeWriter_WriteStr(&writer, s) < 0) {
            Py_DECREF(s);
            goto error;
        }
        Py_DECREF(s);

        p = p->next;
    }

    writer.overallocate = 0;
    if (_PyUnicodeWriter_WriteChar(&writer, ']') < 0)
        goto error;

    Py_ReprLeave((PyObject*)self);
    return _PyUnicodeWriter_Finish(&writer);

error:
    _PyUnicodeWriter_Dealloc(&writer);
    Py_ReprLeave((PyObject*)self);
    return NULL;
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
    (reprfunc)SortedSet_repr,      /* tp_repr */
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
    SortedSet_new,                 /* tp_new */
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
