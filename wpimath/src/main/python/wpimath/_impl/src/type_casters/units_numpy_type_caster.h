#pragma once

#include <pybind11/numpy.h>

namespace pybind11 {
namespace detail {

template <wpi::units::ConversionFactorType ConversionFactor,
          wpi::units::ArithmeticType T,
          wpi::units::NumericalScaleType<T> NumericalScale>
struct npy_format_descriptor<
    wpi::units::unit<ConversionFactor, T, NumericalScale>> {
  static constexpr auto name = const_name("numpy.float64");
  static constexpr int value = npy_api::NPY_DOUBLE_;

  static pybind11::dtype dtype() { return pybind11::dtype(value); }
};

} // namespace detail
} // namespace pybind11
