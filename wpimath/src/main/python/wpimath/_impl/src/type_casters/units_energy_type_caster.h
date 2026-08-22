#pragma once

#include <wpi/units/energy.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::joules<>> {
  static constexpr auto name = _("wpimath.units.joules");
};

template <> struct handle_type_name<wpi::units::joules_> {
  static constexpr auto name = _("wpimath.units.joules");
};

template <> struct handle_type_name<wpi::units::nanojoules<>> {
  static constexpr auto name = _("wpimath.units.nanojoules");
};

template <> struct handle_type_name<wpi::units::nanojoules_> {
  static constexpr auto name = _("wpimath.units.nanojoules");
};

template <> struct handle_type_name<wpi::units::microjoules<>> {
  static constexpr auto name = _("wpimath.units.microjoules");
};

template <> struct handle_type_name<wpi::units::microjoules_> {
  static constexpr auto name = _("wpimath.units.microjoules");
};

template <> struct handle_type_name<wpi::units::millijoules<>> {
  static constexpr auto name = _("wpimath.units.millijoules");
};

template <> struct handle_type_name<wpi::units::millijoules_> {
  static constexpr auto name = _("wpimath.units.millijoules");
};

template <> struct handle_type_name<wpi::units::kilojoules<>> {
  static constexpr auto name = _("wpimath.units.kilojoules");
};

template <> struct handle_type_name<wpi::units::kilojoules_> {
  static constexpr auto name = _("wpimath.units.kilojoules");
};

template <> struct handle_type_name<wpi::units::calories<>> {
  static constexpr auto name = _("wpimath.units.calories");
};

template <> struct handle_type_name<wpi::units::calories_> {
  static constexpr auto name = _("wpimath.units.calories");
};

template <> struct handle_type_name<wpi::units::nanocalories<>> {
  static constexpr auto name = _("wpimath.units.nanocalories");
};

template <> struct handle_type_name<wpi::units::nanocalories_> {
  static constexpr auto name = _("wpimath.units.nanocalories");
};

template <> struct handle_type_name<wpi::units::microcalories<>> {
  static constexpr auto name = _("wpimath.units.microcalories");
};

template <> struct handle_type_name<wpi::units::microcalories_> {
  static constexpr auto name = _("wpimath.units.microcalories");
};

template <> struct handle_type_name<wpi::units::millicalories<>> {
  static constexpr auto name = _("wpimath.units.millicalories");
};

template <> struct handle_type_name<wpi::units::millicalories_> {
  static constexpr auto name = _("wpimath.units.millicalories");
};

template <> struct handle_type_name<wpi::units::kilocalories<>> {
  static constexpr auto name = _("wpimath.units.kilocalories");
};

template <> struct handle_type_name<wpi::units::kilocalories_> {
  static constexpr auto name = _("wpimath.units.kilocalories");
};

template <> struct handle_type_name<wpi::units::kilowatt_hours<>> {
  static constexpr auto name = _("wpimath.units.kilowatt_hours");
};

template <> struct handle_type_name<wpi::units::kilowatt_hours_> {
  static constexpr auto name = _("wpimath.units.kilowatt_hours");
};

template <> struct handle_type_name<wpi::units::watt_hours<>> {
  static constexpr auto name = _("wpimath.units.watt_hours");
};

template <> struct handle_type_name<wpi::units::watt_hours_> {
  static constexpr auto name = _("wpimath.units.watt_hours");
};

template <> struct handle_type_name<wpi::units::british_thermal_units<>> {
  static constexpr auto name = _("wpimath.units.british_thermal_units");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_> {
  static constexpr auto name = _("wpimath.units.british_thermal_units");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_iso<>> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_iso");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_iso_> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_iso");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_59<>> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_59");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_59_> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_59");
};

template <> struct handle_type_name<wpi::units::therms<>> {
  static constexpr auto name = _("wpimath.units.therms");
};

template <> struct handle_type_name<wpi::units::therms_> {
  static constexpr auto name = _("wpimath.units.therms");
};

template <> struct handle_type_name<wpi::units::foot_pounds<>> {
  static constexpr auto name = _("wpimath.units.foot_pounds");
};

template <> struct handle_type_name<wpi::units::foot_pounds_> {
  static constexpr auto name = _("wpimath.units.foot_pounds");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
