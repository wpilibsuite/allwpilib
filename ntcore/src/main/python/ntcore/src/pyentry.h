
#include <semiwrap.h>

#include "wpi/nt/NetworkTableEntry.hpp"
#include "wpi/nt/NetworkTableValue.hpp"

namespace pyntcore {

py::object GetBooleanEntry(const wpi::nt::NetworkTableEntry& entry,
                           py::object defaultValue);
py::object GetDoubleEntry(const wpi::nt::NetworkTableEntry& entry,
                          py::object defaultValue);
py::object GetFloatEntry(const wpi::nt::NetworkTableEntry& entry,
                         py::object defaultValue);
py::object GetIntegerEntry(const wpi::nt::NetworkTableEntry& entry,
                           py::object defaultValue);
py::object GetStringEntry(const wpi::nt::NetworkTableEntry& entry,
                          py::object defaultValue);
py::object GetRawEntry(const wpi::nt::NetworkTableEntry& entry,
                       py::object defaultValue);
py::object GetBooleanArrayEntry(const wpi::nt::NetworkTableEntry& entry,
                                py::object defaultValue);
py::object GetDoubleArrayEntry(const wpi::nt::NetworkTableEntry& entry,
                               py::object defaultValue);
py::object GetFloatArrayEntry(const wpi::nt::NetworkTableEntry& entry,
                              py::object defaultValue);
py::object GetIntegerArrayEntry(const wpi::nt::NetworkTableEntry& entry,
                                py::object defaultValue);
py::object GetStringArrayEntry(const wpi::nt::NetworkTableEntry& entry,
                               py::object defaultValue);
py::object GetValueEntry(const wpi::nt::NetworkTableEntry& entry,
                         py::object defaultValue);

}  // namespace pyntcore
