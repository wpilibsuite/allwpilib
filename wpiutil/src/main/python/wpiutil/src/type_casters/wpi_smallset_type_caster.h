
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <wpi/SmallSet.h>

namespace pybind11
{
namespace detail
{

template <typename Type, unsigned Size> struct type_caster<wpi::SmallSet<Type, Size>>
 : set_caster<wpi::SmallSet<Type, Size>, Type> { };

} // namespace detail
} // namespace pybind11