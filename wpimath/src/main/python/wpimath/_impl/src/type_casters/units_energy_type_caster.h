#pragma once

#include "wpi/units/energy.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::joule_t> {
  static constexpr auto name = _("wpimath.units.joules");
};

template <> struct handle_type_name<wpi::units::joules> {
  static constexpr auto name = _("wpimath.units.joules");
};

template <> struct handle_type_name<wpi::units::nanojoule_t> {
  static constexpr auto name = _("wpimath.units.nanojoules");
};

template <> struct handle_type_name<wpi::units::nanojoules> {
  static constexpr auto name = _("wpimath.units.nanojoules");
};

template <> struct handle_type_name<wpi::units::microjoule_t> {
  static constexpr auto name = _("wpimath.units.microjoules");
};

template <> struct handle_type_name<wpi::units::microjoules> {
  static constexpr auto name = _("wpimath.units.microjoules");
};

template <> struct handle_type_name<wpi::units::millijoule_t> {
  static constexpr auto name = _("wpimath.units.millijoules");
};

template <> struct handle_type_name<wpi::units::millijoules> {
  static constexpr auto name = _("wpimath.units.millijoules");
};

template <> struct handle_type_name<wpi::units::kilojoule_t> {
  static constexpr auto name = _("wpimath.units.kilojoules");
};

template <> struct handle_type_name<wpi::units::kilojoules> {
  static constexpr auto name = _("wpimath.units.kilojoules");
};

template <> struct handle_type_name<wpi::units::calorie_t> {
  static constexpr auto name = _("wpimath.units.calories");
};

template <> struct handle_type_name<wpi::units::calories> {
  static constexpr auto name = _("wpimath.units.calories");
};

template <> struct handle_type_name<wpi::units::nanocalorie_t> {
  static constexpr auto name = _("wpimath.units.nanocalories");
};

template <> struct handle_type_name<wpi::units::nanocalories> {
  static constexpr auto name = _("wpimath.units.nanocalories");
};

template <> struct handle_type_name<wpi::units::microcalorie_t> {
  static constexpr auto name = _("wpimath.units.microcalories");
};

template <> struct handle_type_name<wpi::units::microcalories> {
  static constexpr auto name = _("wpimath.units.microcalories");
};

template <> struct handle_type_name<wpi::units::millicalorie_t> {
  static constexpr auto name = _("wpimath.units.millicalories");
};

template <> struct handle_type_name<wpi::units::millicalories> {
  static constexpr auto name = _("wpimath.units.millicalories");
};

template <> struct handle_type_name<wpi::units::kilocalorie_t> {
  static constexpr auto name = _("wpimath.units.kilocalories");
};

template <> struct handle_type_name<wpi::units::kilocalories> {
  static constexpr auto name = _("wpimath.units.kilocalories");
};

template <> struct handle_type_name<wpi::units::kilowatt_hour_t> {
  static constexpr auto name = _("wpimath.units.kilowatt_hours");
};

template <> struct handle_type_name<wpi::units::kilowatt_hours> {
  static constexpr auto name = _("wpimath.units.kilowatt_hours");
};

template <> struct handle_type_name<wpi::units::watt_hour_t> {
  static constexpr auto name = _("wpimath.units.watt_hours");
};

template <> struct handle_type_name<wpi::units::watt_hours> {
  static constexpr auto name = _("wpimath.units.watt_hours");
};

template <> struct handle_type_name<wpi::units::british_thermal_unit_t> {
  static constexpr auto name = _("wpimath.units.british_thermal_units");
};

template <> struct handle_type_name<wpi::units::british_thermal_units> {
  static constexpr auto name = _("wpimath.units.british_thermal_units");
};

template <> struct handle_type_name<wpi::units::british_thermal_unit_iso_t> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_iso");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_iso> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_iso");
};

template <> struct handle_type_name<wpi::units::british_thermal_unit_59_t> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_59");
};

template <> struct handle_type_name<wpi::units::british_thermal_units_59> {
  static constexpr auto name = _("wpimath.units.british_thermal_units_59");
};

template <> struct handle_type_name<wpi::units::therm_t> {
  static constexpr auto name = _("wpimath.units.therms");
};

template <> struct handle_type_name<wpi::units::therms> {
  static constexpr auto name = _("wpimath.units.therms");
};

template <> struct handle_type_name<wpi::units::foot_pound_t> {
  static constexpr auto name = _("wpimath.units.foot_pounds");
};

template <> struct handle_type_name<wpi::units::foot_pounds> {
  static constexpr auto name = _("wpimath.units.foot_pounds");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
