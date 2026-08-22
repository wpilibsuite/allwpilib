#pragma once

#include "wpi/units/core.hpp"

namespace pybind11 {
namespace detail {

template <> struct handle_type_name<wpi::units::dimensionless<>> {
  static constexpr auto name = _("float");
};

template <> struct handle_type_name<wpi::units::dimensionless_> {
  static constexpr auto name = _("float");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
