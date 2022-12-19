#include "Utils.h"

PyObject *vec_to_array(std::vector<uint8_t>& vec) {
    static PyObject *single_array;
    if (!single_array) {
        PyObject *array_module = PyImport_ImportModule("array");
        if (!array_module)
            return NULL;
        PyObject *array_type = PyObject_GetAttrString(array_module, "array");
        Py_DECREF(array_module);
        if (!array_type)
            return NULL;
        single_array = PyObject_CallFunction(array_type, "s[B]", "B", 0);
        Py_DECREF(array_type);
        if (!single_array)
            return NULL;
    }
    // extra-fast way to create an empty array of count elements:
    //   array = single_element_array * count
    PyObject *pysize = PyLong_FromSsize_t(vec.size());
    if (!pysize)
        return NULL;
    PyObject *array = PyNumber_Multiply(single_array, pysize);
    Py_DECREF(pysize);
    if (!array)
        return NULL;

    // now, obtain the address of the array's buffer
    PyObject *buffer_info = PyObject_CallMethod(array, "buffer_info", "");
    if (!buffer_info) {
        Py_DECREF(array);
        return NULL;
    }
    PyObject *pyaddr = PyTuple_GetItem(buffer_info, 0);
    void *addr = PyLong_AsVoidPtr(pyaddr);

    // and, finally, copy the data.
    if (vec.size())
        memcpy(addr, &vec[0], vec.size() * sizeof(uint8_t));

    return array;
}