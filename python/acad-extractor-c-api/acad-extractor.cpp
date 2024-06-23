#define Py_LIMITED_API 0x03090000

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "../../cpp/core.h"

static PyObject* select_on_screen(PyObject* self, PyObject* args)
{
    PyObject* names_py = Py_None;
    PyObject* attrs_list_py = Py_None;
    PyObject* props_list_py = Py_None;
    PyObject* handles_py = Py_None;
    PyObject* callback_py = Py_None;
    PyObject* points_py = Py_None;
    if (!PyArg_ParseTuple(args, "OOO|OOO", &names_py, &attrs_list_py, &props_list_py, &handles_py, &callback_py, &points_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "bad args");
        return 0;
    }
    if (!PyList_Check(names_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "names is not not list");
        return 0;
    }
    if (!PyList_Check(attrs_list_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "attrs_list is not not list");
        return 0;
    }
    if (!PyList_Check(props_list_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "props_list is not not list");
        return 0;
    }
    if (handles_py != Py_None && !PyList_Check(handles_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "handles is not not list");
        return 0;
    }
    if (callback_py != Py_None && !PyCallable_Check(callback_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "callback is not not callable");
        return 0;
    }
    if (!PyList_Check(points_py))
    {
        PyErr_SetString(PyExc_RuntimeError, "points is not not list");
        return 0;
    }

    std::vector<BSTR> names;
    std::vector<std::vector<std::pair<BSTR, BSTR>>> attrs_list;
    std::vector<std::vector<std::pair<BSTR, VARIANT>>> props_list;
    std::vector<BSTR> handles;
    std::function<void(int processed, int total)> callback;
    if (callback_py != Py_None)
    {
        callback = [callback_py](int processed, int total)
        {
            PyObject* args = Py_BuildValue("(ii)", processed, total);
            PyObject* result = PyObject_Call(callback_py, args, 0);
            Py_XDECREF(result);
            Py_XDECREF(args);
        };
    }
    std::vector<std::vector<double>> points;
    try
    {
        long dt = select_on_screen(&names, &attrs_list, &props_list, handles_py != Py_None ? &handles : 0, callback, points_py != Py_None ? &points : 0);

        for (const BSTR& name : names)
        {
            PyObject* name_py = PyUnicode_FromWideChar(name, -1);
            SysFreeString(name);
            PyList_Append(names_py, name_py);
            Py_XDECREF(name_py);
        }

        if (handles_py != Py_None)
        {
            for (const BSTR& handle : handles)
            {
                PyObject* handle_py = PyUnicode_FromWideChar(handle, -1);
                SysFreeString(handle);
                PyList_Append(handles_py, handle_py);
                Py_XDECREF(handle_py);
            }
        }

        if (points_py != Py_None)
        {
            for (const auto& point : points)
            {
                PyObject* point_py = PyList_New(0);
                for (const auto& coord : point)
                {
                    PyObject* coord_py = PyFloat_FromDouble(coord);
                    PyList_Append(point_py, coord_py);
                    Py_XDECREF(coord_py);
                }
                PyList_Append(points_py, point_py);
                Py_XDECREF(point_py);
            }
        }

        for (const auto& attrs : attrs_list)
        {
            PyObject* attrs_py = PyList_New(0);
            for (const std::pair<BSTR, BSTR>& attr : attrs)
            {
                PyObject* attr_py = PyList_New(0);

                PyObject* attr_name_py = PyUnicode_FromWideChar(attr.first, -1);
                SysFreeString(attr.first);
                PyList_Append(attr_py, attr_name_py);
                Py_XDECREF(attr_name_py);

                PyObject* attr_value_py = PyUnicode_FromWideChar(attr.second, -1);
                SysFreeString(attr.second);
                PyList_Append(attr_py, attr_value_py);
                Py_XDECREF(attr_value_py);

                PyList_Append(attrs_py, attr_py);
                Py_XDECREF(attr_py);
            }
            PyList_Append(attrs_list_py, attrs_py);
            Py_XDECREF(attrs_py);
        }

        for (auto& props : props_list)
        {
            PyObject* props_py = PyList_New(0);
            for (std::pair<BSTR, VARIANT>& prop : props)
            {
                PyObject* prop_py = PyList_New(0);

                PyObject* prop_name_py = PyUnicode_FromWideChar(prop.first, -1);
                SysFreeString(prop.first);
                PyList_Append(prop_py, prop_name_py);
                Py_XDECREF(prop_name_py);

                PyObject* prop_value_py;
                if (V_VT(&prop.second) == VT_BSTR)
                {
                    prop_value_py = PyUnicode_FromWideChar(V_BSTR(&prop.second), -1);
                }
                else
                {
                    prop_value_py = PyFloat_FromDouble(V_R8(&prop.second));
                }
                VariantClear(&prop.second);
                PyList_Append(prop_py, prop_value_py);
                Py_XDECREF(prop_value_py);

                PyList_Append(props_py, prop_py);
                Py_XDECREF(prop_py);
            }
            PyList_Append(props_list_py, props_py);
            Py_XDECREF(props_py);
        }

        return PyLong_FromLong(dt);
    }
    catch (const std::exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    }
    return 0;
}

static PyMethodDef methods[] =
{
    {"select_on_screen", select_on_screen, METH_VARARGS, "select_on_screen"},
    {0, 0, 0, 0}
};

static PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "acad_extractor", "acad_extractor", -1, methods
};

PyMODINIT_FUNC PyInit_acad_extractor(void)
{
    return PyModule_Create(&module);
}
