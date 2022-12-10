#include <Python.h>

PyDoc_STRVAR(PyExt_extract_frame_doc, "extract_frame(obj, number)\
\
Extract frame function");

PyObject *PyExt_extract_frame(PyObject *self, PyObject *args) {
    PyObject *frame;
	if (!PyArg_ParseTuple(args, "O", &frame)) {
        // Check if argument is correct
        return NULL;
	}

    unsigned long long _frame = PyLong_AsUnsignedLongLong(frame);
    if (_frame == -1 && PyErr_Occurred()) {
        // Check if argument is unsigned int
        PyErr_Clear();
        return PyErr_Format(PyExc_TypeError, "Parameter must be an unsigned integer type, but got %s", Py_TYPE
        (frame)->tp_name);
    }

    PyObject* byte_list = PyList_New(static_cast<Py_ssize_t>(0));
    uint8_t foo[] = {
        255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,0,0, 255,0,0, 255,0,0, 255,0,0, 255,0,0,
        255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,0,0, 255,0,0, 255,0,0, 255,0,0, 255,0,0,
        255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,0,0, 255,0,0, 255,0,0, 255,0,0, 255,0,0,
        255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,0,0, 255,0,0, 255,0,0, 255,0,0, 255,0,0,
        255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,255,255, 255,0,0, 255,0,0, 255,0,0, 255,0,0, 255,0,0,
        0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,0,255, 0,0,255, 0,0,255, 0,0,255, 0,0,255,
        0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,0,255, 0,0,255, 0,0,255, 0,0,255, 0,0,255,
        0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,0,255, 0,0,255, 0,0,255, 0,0,255, 0,0,255,
        0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,0,255, 0,0,255, 0,0,255, 0,0,255, 0,0,255,
        0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,255,0, 0,0,255, 0,0,255, 0,0,255, 0,0,255, 0,0,255,
    };
    for (int i = 0; i < sizeof(foo) / sizeof(foo[0]); i++) {
        PyList_Append(byte_list, PyLong_FromLong(foo[i]));
    }
    PyObject* bytes = PyBytes_FromObject(byte_list);
    return bytes;
}


/*
 * Implements an example function.
 */

PyDoc_STRVAR(PyExt_example_doc, "example(obj, number)\
\
Example function");

PyObject *PyExt_example(PyObject *self, PyObject *args, PyObject *kwargs) {
    /* Shared references that do not need Py_DECREF before returning. */
    PyObject *obj = NULL;
    int number = 0;

    /* Parse positional and keyword arguments */
    static char* keywords[] = { "obj", "number", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &obj, &number)) {
        return NULL;
    }

    /* Function implementation starts here */

    if (number < 0) {
        PyErr_SetObject(PyExc_ValueError, obj);
        return NULL;    /* return NULL indicates error */
    }

    Py_RETURN_NONE;
}

/*
 * List of functions to add to PyExt in exec_PyExt().
 */
static PyMethodDef PyExt_functions[] = {
    { "example", (PyCFunction)PyExt_example, METH_VARARGS | METH_KEYWORDS, PyExt_example_doc },
    { "extract_frame", (PyCFunction)PyExt_extract_frame, METH_VARARGS, PyExt_extract_frame_doc },
    { NULL, NULL, 0, NULL } /* marks end of array */
};

/*
 * Initialize PyExt. May be called multiple times, so avoid
 * using static state.
 */
int exec_PyExt(PyObject *module) {
    PyModule_AddFunctions(module, PyExt_functions);

    PyModule_AddStringConstant(module, "__author__", "timothy.halim");
    PyModule_AddStringConstant(module, "__version__", "1.0.0");
    PyModule_AddIntConstant(module, "year", 2022);

    return 0; /* success */
}

/*
 * Documentation for PyExt.
 */
PyDoc_STRVAR(PyExt_doc, "The PyExt module");


static PyModuleDef_Slot PyExt_slots[] = {
    { Py_mod_exec, exec_PyExt },
    { 0, NULL }
};

static PyModuleDef PyExt_def = {
    PyModuleDef_HEAD_INIT,
    "PyExt",
    PyExt_doc,
    0,              /* m_size */
    NULL,           /* m_methods */
    PyExt_slots,
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_PyExt() {
    return PyModuleDef_Init(&PyExt_def);
}
