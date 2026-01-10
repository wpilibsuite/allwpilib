#include <pybind11/pybind11.h>

#include "wpi/util/string.h"

namespace pybind11::detail {

template <> 
struct type_caster<WPI_String> {
public:
    PYBIND11_TYPE_CASTER(WPI_String, _("str"));

    bool load(handle src, bool convert) {
        if (!src) {
            return false;
        }

        Py_ssize_t size = -1;
        const char *str = PyUnicode_AsUTF8AndSize(src.ptr(), &size);
        if (!str) {
            PyErr_Clear();
            return false;
        }

        value = WPI_String{str, static_cast<size_t>(size)};
        
        return true;
    }

    static handle cast(const WPI_String& str, return_value_policy /* policy */, handle /* parent */) {
        return PyUnicode_FromStringAndSize(str.str, str.len);
    }
};

} // namespace pybind11::detail