%Module(name=acad_extractor, language="C++")

%ModuleCode
#include "core.h"
%End

SIP_PYOBJECT select_on_screen(SIP_PYLIST names, SIP_PYLIST attrs_list, SIP_PYLIST props_list);
%MethodCode
    PyObject* names_py = a0;
    PyObject* attrs_list_py = a1;
    PyObject* props_list_py = a2;

    std::vector<BSTR> names;
    std::vector<std::vector<std::pair<BSTR, BSTR>>> attrs_list;
    std::vector<std::vector<std::pair<BSTR, VARIANT>>> props_list;
    try
    {
        long dt = select_on_screen(&names, &attrs_list, &props_list);

        for (const BSTR& name : names)
        {
            PyObject* name_py = PyUnicode_FromWideChar(name, -1);
            SysFreeString(name);
            PyList_Append(names_py, name_py);
            Py_XDECREF(name_py);
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
    Py_RETURN_NONE;
%End
