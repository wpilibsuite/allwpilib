#pragma once

#include <wpi/units/angular_velocity.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::radians_per_second<>> {
  static constexpr auto name = _("wpimath.units.radians_per_second");
};

template <> struct handle_type_name<wpi::units::radians_per_second_> {
  static constexpr auto name = _("wpimath.units.radians_per_second");
};

template <> struct handle_type_name<wpi::units::degrees_per_second<>> {
  static constexpr auto name = _("wpimath.units.degrees_per_second");
};

template <> struct handle_type_name<wpi::units::degrees_per_second_> {
  static constexpr auto name = _("wpimath.units.degrees_per_second");
};

template <> struct handle_type_name<wpi::units::turns_per_second<>> {
  static constexpr auto name = _("wpimath.units.turns_per_second");
};

template <> struct handle_type_name<wpi::units::turns_per_second_> {
  static constexpr auto name = _("wpimath.units.turns_per_second");
};

template <> struct handle_type_name<wpi::units::revolutions_per_minute<>> {
  static constexpr auto name = _("wpimath.units.revolutions_per_minute");
};

template <> struct handle_type_name<wpi::units::revolutions_per_minute_> {
  static constexpr auto name = _("wpimath.units.revolutions_per_minute");
};

template <> struct handle_type_name<wpi::units::revolutions_per_second<>> {
  static constexpr auto name = _("wpimath.units.revolutions_per_second");
};

template <> struct handle_type_name<wpi::units::revolutions_per_second_> {
  static constexpr auto name = _("wpimath.units.revolutions_per_second");
};

template <> struct handle_type_name<wpi::units::milliarcseconds_per_year<>> {
  static constexpr auto name = _("wpimath.units.milliarcseconds_per_year");
};

template <> struct handle_type_name<wpi::units::milliarcseconds_per_year_> {
  static constexpr auto name = _("wpimath.units.milliarcseconds_per_year");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
