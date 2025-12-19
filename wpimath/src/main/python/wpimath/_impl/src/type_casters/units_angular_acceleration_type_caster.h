#pragma once

#include <wpi/units/angular_acceleration.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::radians_per_second_squared<>> {
  static constexpr auto name = _("wpimath.units.radians_per_second_squared");
};

template <> struct handle_type_name<wpi::units::radians_per_second_squared_> {
  static constexpr auto name = _("wpimath.units.radians_per_second_squared");
};

template <> struct handle_type_name<wpi::units::degrees_per_second_squared<>> {
  static constexpr auto name = _("wpimath.units.degrees_per_second_squared");
};

template <> struct handle_type_name<wpi::units::degrees_per_second_squared_> {
  static constexpr auto name = _("wpimath.units.degrees_per_second_squared");
};

template <> struct handle_type_name<wpi::units::turns_per_second_squared<>> {
  static constexpr auto name = _("wpimath.units.turns_per_second_squared");
};

template <> struct handle_type_name<wpi::units::turns_per_second_squared_> {
  static constexpr auto name = _("wpimath.units.turns_per_second_squared");
};

template <> struct handle_type_name<wpi::units::revolutions_per_minute_squared<>> {
  static constexpr auto name = _("wpimath.units.revolutions_per_minute_squared");
};

template <> struct handle_type_name<wpi::units::revolutions_per_minute_squared_> {
  static constexpr auto name = _("wpimath.units.revolutions_per_minute_squared");
};

template <> struct handle_type_name<wpi::units::revolutions_per_minute_per_second<>> {
  static constexpr auto name = _("wpimath.units.revolutions_per_minute_per_second");
};

template <> struct handle_type_name<wpi::units::revolutions_per_minute_per_second_> {
  static constexpr auto name = _("wpimath.units.revolutions_per_minute_per_second");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
