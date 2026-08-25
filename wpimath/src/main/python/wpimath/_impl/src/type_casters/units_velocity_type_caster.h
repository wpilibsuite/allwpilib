#pragma once

#include <wpi/units/velocity.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::meters_per_second<>> {
  static constexpr auto name = _("wpimath.units.meters_per_second");
};

template <> struct handle_type_name<wpi::units::meters_per_second_> {
  static constexpr auto name = _("wpimath.units.meters_per_second");
};

template <> struct handle_type_name<wpi::units::feet_per_second<>> {
  static constexpr auto name = _("wpimath.units.feet_per_second");
};

template <> struct handle_type_name<wpi::units::feet_per_second_> {
  static constexpr auto name = _("wpimath.units.feet_per_second");
};

template <> struct handle_type_name<wpi::units::miles_per_hour<>> {
  static constexpr auto name = _("wpimath.units.miles_per_hour");
};

template <> struct handle_type_name<wpi::units::miles_per_hour_> {
  static constexpr auto name = _("wpimath.units.miles_per_hour");
};

template <> struct handle_type_name<wpi::units::kilometers_per_hour<>> {
  static constexpr auto name = _("wpimath.units.kilometers_per_hour");
};

template <> struct handle_type_name<wpi::units::kilometers_per_hour_> {
  static constexpr auto name = _("wpimath.units.kilometers_per_hour");
};

template <> struct handle_type_name<wpi::units::knots<>> {
  static constexpr auto name = _("wpimath.units.knots");
};

template <> struct handle_type_name<wpi::units::knots_> {
  static constexpr auto name = _("wpimath.units.knots");
};

template <> struct handle_type_name<wpi::units::feet_per_minute<>> {
  static constexpr auto name = _("wpimath.units.feet_per_minute");
};

template <> struct handle_type_name<wpi::units::feet_per_minute_> {
  static constexpr auto name = _("wpimath.units.feet_per_minute");
};

template <> struct handle_type_name<wpi::units::meters_per_minute<>> {
  static constexpr auto name = _("wpimath.units.meters_per_minute");
};

template <> struct handle_type_name<wpi::units::meters_per_minute_> {
  static constexpr auto name = _("wpimath.units.meters_per_minute");
};

template <> struct handle_type_name<wpi::units::inches_per_second<>> {
  static constexpr auto name = _("wpimath.units.inches_per_second");
};

template <> struct handle_type_name<wpi::units::inches_per_second_> {
  static constexpr auto name = _("wpimath.units.inches_per_second");
};

template <> struct handle_type_name<wpi::units::kilometers_per_second<>> {
  static constexpr auto name = _("wpimath.units.kilometers_per_second");
};

template <> struct handle_type_name<wpi::units::kilometers_per_second_> {
  static constexpr auto name = _("wpimath.units.kilometers_per_second");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
