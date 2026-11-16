#pragma once

#include <pybind11/numpy.h>

namespace pybind11 {
namespace detail {

template <class U, typename T, template <typename> class S>
struct npy_format_descriptor<wpi::units::unit_t<U, T, S>> {
  static constexpr auto name = const_name("numpy.float64");
  static constexpr int value = npy_api::NPY_DOUBLE_;

  static pybind11::dtype dtype() { return pybind11::dtype(value); }
};

} // namespace detail
} // namespace pybind11
