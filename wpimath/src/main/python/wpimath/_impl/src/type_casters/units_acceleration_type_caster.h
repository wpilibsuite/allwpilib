#pragma once

#include "wpi/units/acceleration.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::meters_per_second_squared_t> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared");
};

template <> struct handle_type_name<wpi::units::meters_per_second_squared> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared");
};

template <> struct handle_type_name<wpi::units::feet_per_second_squared_t> {
  static constexpr auto name = _("wpimath.units.feet_per_second_squared");
};

template <> struct handle_type_name<wpi::units::feet_per_second_squared> {
  static constexpr auto name = _("wpimath.units.feet_per_second_squared");
};

template <> struct handle_type_name<wpi::units::standard_gravity_t> {
  static constexpr auto name = _("wpimath.units.standard_gravity");
};

template <> struct handle_type_name<wpi::units::standard_gravity> {
  static constexpr auto name = _("wpimath.units.standard_gravity");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
