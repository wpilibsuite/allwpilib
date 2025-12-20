#include <pybind11/pybind11.h>

#include "wpi/util/string.h"

namespace pybind11::detail {

template <> 
struct type_caster<WPI_String> {
public:
    PYBIND11_TYPE_CASTER(WPI_String, _("str"));

    bool load(handle src, bool convert) {
        if (!src) return false;

        if (!pybind11::isinstance<pybind11::str>(src)) {
            return false;
        }

        auto py_str = pybind11::reinterpret_borrow<pybind11::str>(src);
        std::string temp = py_str;

        value = WPI_String(temp.c_str(), temp.length());
        
        return true;
    }

    static handle cast(const WPI_String& str, return_value_policy /* policy */, handle /* parent */) {
        return pybind11::str(str.str, str.len).release();
    }
};

} // namespace pybind11::detail