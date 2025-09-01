
#include <semiwrap.h>
#include <networktables/NetworkTableValue.h>
#include <networktables/NetworkTableType.h>
#include <fmt/format.h>

namespace pyntcore {

const char * nttype2str(NT_Type type);

py::object ntvalue2py(const nt::Value &ntvalue);

nt::Value py2ntvalue(py::handle h);

py::function valueFactoryByType(nt::NetworkTableType type);

inline void ensure_value_is(NT_Type expected, nt::Value *v) {
    if (v->type() != expected) {
        throw py::value_error(fmt::format(
            "Value type is {}, not {}", nttype2str(v->type()), nttype2str(expected)
        ));
    }
}

};