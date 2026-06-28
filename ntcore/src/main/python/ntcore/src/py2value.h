#pragma once

#include <fmt/format.h>
#include <semiwrap.h>

#include "wpi/nt/NetworkTableType.hpp"
#include "wpi/nt/NetworkTableValue.hpp"

namespace pyntcore {

const char* nttype2str(NT_Type type);

py::object ntvalue2py(const wpi::nt::Value& ntvalue);

wpi::nt::Value py2ntvalue(py::handle h);

py::function valueFactoryByType(wpi::nt::NetworkTableType type);

inline void ensure_value_is(NT_Type expected, wpi::nt::Value* v) {
  if (v->type() != expected) {
    throw py::value_error(fmt::format("Value type is {}, not {}",
                                      nttype2str(v->type()),
                                      nttype2str(expected)));
  }
}

}  // namespace pyntcore
