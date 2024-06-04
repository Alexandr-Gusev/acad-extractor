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
            PyList_Append(names_py, PyUnicode_FromWideChar(name, -1));
        }

        for (const auto& attrs : attrs_list)
        {
            PyObject* attrs_py = PyList_New(0);
            for (const std::pair<BSTR, BSTR>& attr : attrs)
            {
                PyObject* attr_py = PyList_New(0);
                PyList_Append(attr_py, PyUnicode_FromWideChar(attr.first, -1));
                PyList_Append(attr_py, PyUnicode_FromWideChar(attr.second, -1));
                PyList_Append(attrs_py, attr_py);
            }
            PyList_Append(attrs_list_py, attrs_py);
        }

        for (const auto& props : props_list)
        {
            PyObject* props_py = PyList_New(0);
            for (const std::pair<BSTR, VARIANT>& prop : props)
            {
                PyObject* prop_py = PyList_New(0);
                PyList_Append(prop_py, PyUnicode_FromWideChar(prop.first, -1));
                if (V_VT(&prop.second) == VT_BSTR)
                {
                    PyList_Append(prop_py, PyUnicode_FromWideChar(V_BSTR(&prop.second), -1));
                }
                else
                {
                    PyList_Append(prop_py, PyFloat_FromDouble(V_R8(&prop.second)));
                }
                PyList_Append(props_py, prop_py);
            }
            PyList_Append(props_list_py, props_py);
        }

        return PyLong_FromLong(dt);
    }
    catch (const std::exception& e)
    {
        PyErr_SetString(PyExc_RuntimeError, e.what());
    }
    Py_RETURN_NONE;
%End
