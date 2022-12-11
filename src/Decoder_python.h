#ifndef DECODER_WRAP
#define DECODER_WRAP

#include "Decoder.h"
#include <structmember.h>


/*****************************************************************************
* C style definition of Python classes.
* Each class would ref the C implemented class directly.
* No extra python data member is added to these classes,
* because the data members have been already packed as private members of the
* C classes.
*****************************************************************************/

typedef struct _C_Decoder
{
    PyObject_HEAD                    // == PyObject ob_base; Define the PyObject header.
        Decoder* _c_ref;  // Define the implementation of the C Object.
} C_Decoder;

static PyMemberDef C_Decoder_DataMembers[] =        // Register the members of the python class.
{ // Do not register any data, because all data of this class is private.
  //{"m_dEnglish", T_FLOAT,  offsetof(CScore, m_dEnglish), 0, "The English score of instance."},
    { "__addr__", T_ULONGLONG, offsetof(C_Decoder, _c_ref), READONLY, "The address of the reference in memory." },
    { nullptr, 0, 0, 0, nullptr }
};


/*****************************************************************************
* Declare the core methods of the classes.
*****************************************************************************/

static int C_Decoder_init(C_Decoder* self, PyObject* args, PyObject* kwargs) {  // Construct
    PyObject* path = nullptr;
    static char* kwlist[] = { "filepath", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist, &path)) {
        PyErr_SetString(PyExc_TypeError, "Error.Initialize: need 'filepath(str)'");
        return -1;
    }

    std::string _filepath;
    _filepath.clear();
    if (path) {
        _filepath = PyUnicode_AsUTF8(path);
    }

    self->_c_ref = new Decoder;
    if (!_filepath.empty()) {
        self->_c_ref->set_filepath(_filepath);
    }

    _filepath.clear();
    //cout << sizeof(self->_in_Handle) << " - " << sizeof(unsigned long long) << endl;
    return 0;
}

static void C_Decoder_Destruct(C_Decoder* self) {  // Destructor
    delete self->_c_ref;  // Delete the allocated class implementation.
    /* If there are still other members, also need to deallocate them,
     * for example, Py_XDECREF(self->Member); */
    Py_TYPE(self)->tp_free((PyObject*)self);  // Destruct the PyObject.
}

static PyObject* C_Decoder_Str(C_Decoder* self) {  // The __str__ (print) operator.
    // char *desc = ;
    return PyUnicode_FromString(self->_c_ref->get_filepath().c_str());  // Convert the string to unicode wide char.
}

static PyObject* C_Decoder_Repr(C_Decoder* self) {  // The __repr__ operator.
    return C_Decoder_Str(self);
}


/*****************************************************************************
* Additional methods of the classes.
*****************************************************************************/

PyDoc_STRVAR(Decoder_set_filepath_doc, "set_filepath(filepath)\
\
Set Decoder path function");

PyObject *Decoder_set_filepath(C_Decoder *self, PyObject *args) {
    /* Shared references that do not need Py_DECREF before returning. */

    char* filepath;
    /* Parse positional and keyword arguments */
    if (!PyArg_ParseTuple(args, "s", &filepath)) {
        return NULL;
    }

    if (*filepath == *"") {
        PyErr_SetString(PyExc_ValueError, "Filepath is empty");
        //PyErr_SetObject(PyExc_ValueError, filepath);
        return NULL;    /* return NULL indicates error */
    }

    std::string _filepath = filepath;
    if (!_filepath.empty()) {
        PyObject* ret = PyUnicode_FromString(
            self->_c_ref->set_filepath(_filepath).c_str()
        );

        _filepath.clear();
        return ret;
    }

    Py_RETURN_NONE;
}

PyDoc_STRVAR(Decoder_get_filepath_doc, "get_filepath()\
\
Get Decoder path function");

PyObject *Decoder_get_filepath(C_Decoder *self) {
    PyObject* ret = PyUnicode_FromString(
        self->_c_ref->get_filepath().c_str()
    );
    return ret;
}

PyDoc_STRVAR(Decoder_open_file_doc, "open_file()\
\
Get Decoder path function");

PyObject *Decoder_open_file(C_Decoder *self) {
    PyObject* ret = PyLong_FromLong(
        self->_c_ref->open_file()
    );
    return ret;
}

PyDoc_STRVAR(Decoder_get_file_info_doc, "get_file_info()\
\
Get file info");

PyObject *Decoder_get_file_info(C_Decoder *self) {
    return self->_c_ref->get_file_info();
}


/*****************************************************************************
* Register the methods of each class.
*****************************************************************************/

static PyMethodDef C_Decoder_MethodMembers[] =      // Register the member methods of Decoder.
{  // This step add the methods to the C-API of the class.
    { "set_filepath", (PyCFunction)Decoder_set_filepath, METH_VARARGS, Decoder_set_filepath_doc },
    { "get_filepath", (PyCFunction)Decoder_get_filepath, METH_NOARGS, Decoder_get_filepath_doc },
    { "open_file", (PyCFunction)Decoder_open_file, METH_NOARGS, Decoder_open_file_doc },
    { "get_file_info", (PyCFunction)Decoder_get_file_info, METH_NOARGS, Decoder_get_file_info_doc },
    { nullptr, nullptr, 0, nullptr }
};

/*****************************************************************************
* Declaration of the class, including the name, information and the members.
* This is the top-level packing of the class APIs.
*****************************************************************************/

static PyTypeObject C_Decoder_ClassInfo =
{
    PyVarObject_HEAD_INIT(nullptr, 0)"FFMPy.Decoder",  // The implementation of the __class__.__name__.
    sizeof(C_Decoder),          // The memory length of the class. This value is required for PyObject_New.
    0,
    (destructor)C_Decoder_Destruct,    // Destructor.
    0, 0, 0, 0,
    (reprfunc)C_Decoder_Repr,          // __repr__ method.
    0, 0, 0, 0, 0,
    (reprfunc)C_Decoder_Str,           // __str__ method.
    0, 0, 0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,     // If no methods are provided, this value is Py_TPFLAGS_DEFAULE.
    "This class has wrapped the C-API of FFmpeg decoder so that users could call its methods\n to decode the frame data in python quickly.",   // __doc__, the docstring of the class.
    0, 0, 0, 0, 0, 0,
    C_Decoder_MethodMembers,       // The collection of all method members.
    C_Decoder_DataMembers,         // THe collection of all data members.
    0, 0, 0, 0, 0, 0,
    (initproc)C_Decoder_init,      // Constructor.
    0,
};

#endif // !PYDECODER_WRAP