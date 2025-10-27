
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <wpi/util/SmallVector.hpp>

namespace pybind11
{
namespace detail
{

template <typename Type, unsigned Size> struct type_caster<wpi::util::SmallVector<Type, Size>>
 : list_caster<wpi::util::SmallVector<Type, Size>, Type> { };

} // namespace detail
} // namespace pybind11