#pragma once

#include <units/mass.h>

namespace pybind11 {
namespace detail {

template <> struct handle_type_name<units::dimensionless::scalar_t> {
  static constexpr auto name = _("float");
};

template <> struct handle_type_name<units::dimensionless::scalar> {
  static constexpr auto name = _("float");
};

// template <> struct handle_type_name<units::dimensionless::dimensionless_t> {
//   static constexpr auto name = _("float");
// };

// template <> struct handle_type_name<units::dimensionless::dimensionless> {
//   static constexpr auto name = _("float");
// };

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"