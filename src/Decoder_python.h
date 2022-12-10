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

static int C_Decoder_init(C_Decoder* Self, PyObject* args, PyObject* kwargs) {  // Construct
    return 0;
}

static void C_Decoder_Destruct(C_Decoder* Self) {  // Destructor
    delete Self->_c_ref;  // Delete the allocated class implementation.
    /* If there are still other members, also need to deallocate them,
     * for example, Py_XDECREF(Self->Member); */
    Py_TYPE(Self)->tp_free((PyObject*)Self);  // Destruct the PyObject.
}

static PyObject* C_Decoder_Str(C_Decoder* Self) {  // The __str__ (print) operator.
    // std::ostringstream OStr;
    // OStr << *(Self->_c_ref);
    // std::string Str = OStr.str();
    // std::string Str = "Description";

    return PyUnicode_FromString("Description");  // Convert the string to unicode wide char.
}

static PyObject* C_Decoder_Repr(C_Decoder* Self) {  // The __repr__ operator.
    return C_Decoder_Str(Self);
}

/*****************************************************************************
* Register the methods of each class.
*****************************************************************************/

static PyMethodDef C_Decoder_MethodMembers[] =      // Register the member methods of Decoder.
{  // This step add the methods to the C-API of the class.
    { nullptr, nullptr, 0, nullptr }
};

/*****************************************************************************
* Declaration of the class, including the name, information and the members.
* This is the top-level packing of the class APIs.
*****************************************************************************/

static PyTypeObject C_MPDC_ClassInfo =
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