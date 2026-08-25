#pragma once

#include <wpi/units/volume_flow_rate.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::cubic_meters_per_second<>> {
  static constexpr auto name = _("wpimath.units.cubic_meters_per_second");
};

template <> struct handle_type_name<wpi::units::cubic_meters_per_second_> {
  static constexpr auto name = _("wpimath.units.cubic_meters_per_second");
};

template <> struct handle_type_name<wpi::units::cubic_meters_per_hour<>> {
  static constexpr auto name = _("wpimath.units.cubic_meters_per_hour");
};

template <> struct handle_type_name<wpi::units::cubic_meters_per_hour_> {
  static constexpr auto name = _("wpimath.units.cubic_meters_per_hour");
};

template <> struct handle_type_name<wpi::units::liters_per_second<>> {
  static constexpr auto name = _("wpimath.units.liters_per_second");
};

template <> struct handle_type_name<wpi::units::liters_per_second_> {
  static constexpr auto name = _("wpimath.units.liters_per_second");
};

template <> struct handle_type_name<wpi::units::liters_per_minute<>> {
  static constexpr auto name = _("wpimath.units.liters_per_minute");
};

template <> struct handle_type_name<wpi::units::liters_per_minute_> {
  static constexpr auto name = _("wpimath.units.liters_per_minute");
};

template <> struct handle_type_name<wpi::units::gallons_per_minute<>> {
  static constexpr auto name = _("wpimath.units.gallons_per_minute");
};

template <> struct handle_type_name<wpi::units::gallons_per_minute_> {
  static constexpr auto name = _("wpimath.units.gallons_per_minute");
};

template <> struct handle_type_name<wpi::units::gallons_per_hour<>> {
  static constexpr auto name = _("wpimath.units.gallons_per_hour");
};

template <> struct handle_type_name<wpi::units::gallons_per_hour_> {
  static constexpr auto name = _("wpimath.units.gallons_per_hour");
};

template <> struct handle_type_name<wpi::units::cubic_feet_per_second<>> {
  static constexpr auto name = _("wpimath.units.cubic_feet_per_second");
};

template <> struct handle_type_name<wpi::units::cubic_feet_per_second_> {
  static constexpr auto name = _("wpimath.units.cubic_feet_per_second");
};

template <> struct handle_type_name<wpi::units::cubic_feet_per_minute<>> {
  static constexpr auto name = _("wpimath.units.cubic_feet_per_minute");
};

template <> struct handle_type_name<wpi::units::cubic_feet_per_minute_> {
  static constexpr auto name = _("wpimath.units.cubic_feet_per_minute");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
