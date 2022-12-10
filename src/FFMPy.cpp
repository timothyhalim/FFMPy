#include "FFMPy.h"
#include "Decoder_python.h"

/*
 * Initialize FFMPy. May be called multiple times, so avoid
 * using static state.
 */
int exec_FFMPy(PyObject *module) {
    PyModule_AddStringConstant(module, "__author__", "timothy.halim");
    PyModule_AddStringConstant(module, "__version__", "1.0.0");

    return 0; /* success */
}

/*
 * Documentation for FFMPy.
 */
PyDoc_STRVAR(FFMPy_doc, "FFMPEG for python");


static PyModuleDef_Slot FFMPy_slots[] = {
    { Py_mod_exec, exec_FFMPy },
    { 0, NULL }
};


static PyModuleDef FFMPy_def = {
    PyModuleDef_HEAD_INIT,
    "FFMPy",
    FFMPy_doc,
    0,              /* m_size */
    NULL,           /* m_methods */
    NULL,
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_FFMPy(void) {
    // return PyModuleDef_Init(&FFMPy_def);
    
    PyObject* pReturn = 0;
    
    /*
        Initialize the module
    */
    pReturn = PyModule_Create(&FFMPy_def);  // Create the module according to the module info.
    if (pReturn == 0)
        return nullptr;
    Py_INCREF(&FFMPy_def);  // Because the module is not registered to the python counter, Py_INCREF is required to be invoked.
    
    /*
        Adding class
    */

    // Configure the __new__ method as the default method. This method is used for building the instances.
    C_Decoder_ClassInfo.tp_new = PyType_GenericNew;

    /* Finish the initialization, including the derivations.
     * When success, return 0; Otherwise, return -1 and throw errors. */
    if (PyType_Ready(&C_Decoder_ClassInfo) < 0)
        return nullptr;
    PyModule_AddObject(pReturn, "Decoder", (PyObject*)&C_Decoder_ClassInfo);  // Add the class as one module member.
    
    return pReturn;
}
