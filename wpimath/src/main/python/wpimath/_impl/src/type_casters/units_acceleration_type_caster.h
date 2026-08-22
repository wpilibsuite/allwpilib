#pragma once

#include <wpi/units/acceleration.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::meters_per_second_squared<>> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared");
};

template <> struct handle_type_name<wpi::units::meters_per_second_squared_> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared");
};

template <> struct handle_type_name<wpi::units::feet_per_second_squared<>> {
  static constexpr auto name = _("wpimath.units.feet_per_second_squared");
};

template <> struct handle_type_name<wpi::units::feet_per_second_squared_> {
  static constexpr auto name = _("wpimath.units.feet_per_second_squared");
};

template <> struct handle_type_name<wpi::units::standard_gravity<>> {
  static constexpr auto name = _("wpimath.units.standard_gravity");
};

template <> struct handle_type_name<wpi::units::standard_gravity_> {
  static constexpr auto name = _("wpimath.units.standard_gravity");
};

template <> struct handle_type_name<wpi::units::gals<>> {
  static constexpr auto name = _("wpimath.units.gals");
};

template <> struct handle_type_name<wpi::units::gals_> {
  static constexpr auto name = _("wpimath.units.gals");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
