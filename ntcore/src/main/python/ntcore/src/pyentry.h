
#include <semiwrap.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableValue.h>

namespace pyntcore {

py::object GetBooleanEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetDoubleEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetFloatEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetIntegerEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetStringEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetRawEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetBooleanArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetDoubleArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetFloatArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetIntegerArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetStringArrayEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);
py::object GetValueEntry(const nt::NetworkTableEntry &entry, py::object defaultValue);

};
