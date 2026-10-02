
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "wpi/util/StringMap.hpp"

namespace pybind11
{
namespace detail
{

template <typename Value>
struct type_caster<wpi::util::StringMap<Value>>
 : map_caster<wpi::util::StringMap<Value>, std::string, Value> { };

} // namespace detail
} // namespace pybind11