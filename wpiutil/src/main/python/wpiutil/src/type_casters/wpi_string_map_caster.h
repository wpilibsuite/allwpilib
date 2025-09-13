
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <wpi/StringMap.h>

namespace pybind11
{
namespace detail
{

template <typename Value>
struct type_caster<wpi::StringMap<Value>>
 : map_caster<wpi::StringMap<Value>, std::string, Value> { };

} // namespace detail
} // namespace pybind11