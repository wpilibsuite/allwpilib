#pragma once

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/time.h>
#include <units/voltage.h>

namespace pybind11 {
namespace detail {

template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<units::radian, units::inverse<units::meter>>>> {
  static constexpr auto name = _("wpimath.units.radians_per_meter");
};

template <>
struct handle_type_name<units::unit_t<units::compound_unit<
    units::radians_per_second, units::inverse<units::volt>>>> {
  static constexpr auto name = _("wpimath.units.radians_per_second_per_volt");
};

template <>
struct handle_type_name<units::unit_t<units::inverse<units::seconds>>> {
  static constexpr auto name = _("wpimath.units.units_per_second");
};

template <>
struct handle_type_name<
    units::unit_t<units::inverse<units::squared<units::seconds>>>> {
  static constexpr auto name = _("wpimath.units.units_per_second_squared");
};

using volt_seconds = units::compound_unit<units::volts, units::seconds>;
using volt_seconds_squared = units::compound_unit<volt_seconds, units::seconds>;

template <> struct handle_type_name<units::unit_t<volt_seconds>> {
  static constexpr auto name = _("wpimath.units.volt_seconds");
};

template <> struct handle_type_name<units::unit_t<volt_seconds_squared>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared");
};

template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<volt_seconds, units::inverse<units::meter>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_meter");
};
template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<volt_seconds_squared, units::inverse<units::meter>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_meter");
};
template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<volt_seconds, units::inverse<units::feet>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_feet");
};
template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<volt_seconds_squared, units::inverse<units::feet>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_feet");
};
template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<volt_seconds, units::inverse<units::radian>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_radian");
};
template <>
struct handle_type_name<units::unit_t<units::compound_unit<
    volt_seconds_squared, units::inverse<units::radian>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_radian");
};

using unit_seconds = units::compound_unit<units::dimensionless::scalar, units::seconds>;
using unit_seconds_squared = units::compound_unit<unit_seconds, units::seconds>;

template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<unit_seconds_squared, units::inverse<units::dimensionless::scalar>>>> {
  static constexpr auto name = _("wpimath.units.unit_seconds_squared_per_unit");
};

template <>
struct handle_type_name<units::unit_t<
    units::compound_unit<units::meters_per_second_squared, units::inverse<units::volts>>>> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared_per_volt");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
