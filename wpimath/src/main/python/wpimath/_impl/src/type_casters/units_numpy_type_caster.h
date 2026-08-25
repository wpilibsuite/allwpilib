#pragma once

#include <pybind11/numpy.h>
#include <wpi/units/core.hpp>

namespace pybind11 {
namespace detail {

template <wpi::units::UnitType Unit>
struct npy_format_descriptor<Unit> {
  static constexpr auto name = const_name("numpy.float64");
  static constexpr int value = npy_api::NPY_DOUBLE_;

  static pybind11::dtype dtype() { return pybind11::dtype(value); }
};

} // namespace detail
} // namespace pybind11
