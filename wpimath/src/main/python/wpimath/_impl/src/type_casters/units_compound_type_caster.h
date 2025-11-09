#pragma once

#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/voltage.hpp"

namespace pybind11 {
namespace detail {

template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<wpi::units::radian, wpi::units::inverse<wpi::units::meter>>>> {
  static constexpr auto name = _("wpimath.units.radians_per_meter");
};

template <>
struct handle_type_name<wpi::units::unit_t<wpi::units::compound_unit<
    wpi::units::radians_per_second, wpi::units::inverse<wpi::units::volt>>>> {
  static constexpr auto name = _("wpimath.units.radians_per_second_per_volt");
};

template <>
struct handle_type_name<wpi::units::unit_t<wpi::units::inverse<wpi::units::seconds>>> {
  static constexpr auto name = _("wpimath.units.units_per_second");
};

template <>
struct handle_type_name<
    wpi::units::unit_t<wpi::units::inverse<wpi::units::squared<wpi::units::seconds>>>> {
  static constexpr auto name = _("wpimath.units.units_per_second_squared");
};

using volt_seconds = wpi::units::compound_unit<wpi::units::volts, wpi::units::seconds>;
using volt_seconds_squared = wpi::units::compound_unit<volt_seconds, wpi::units::seconds>;

template <> struct handle_type_name<wpi::units::unit_t<volt_seconds>> {
  static constexpr auto name = _("wpimath.units.volt_seconds");
};

template <> struct handle_type_name<wpi::units::unit_t<volt_seconds_squared>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared");
};

template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<volt_seconds, wpi::units::inverse<wpi::units::meter>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_meter");
};
template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<volt_seconds_squared, wpi::units::inverse<wpi::units::meter>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_meter");
};
template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<volt_seconds, wpi::units::inverse<wpi::units::feet>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_feet");
};
template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<volt_seconds_squared, wpi::units::inverse<wpi::units::feet>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_feet");
};
template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<volt_seconds, wpi::units::inverse<wpi::units::radian>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_radian");
};
template <>
struct handle_type_name<wpi::units::unit_t<wpi::units::compound_unit<
    volt_seconds_squared, wpi::units::inverse<wpi::units::radian>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_radian");
};

using unit_seconds = wpi::units::compound_unit<wpi::units::dimensionless::scalar, wpi::units::seconds>;
using unit_seconds_squared = wpi::units::compound_unit<unit_seconds, wpi::units::seconds>;

template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<unit_seconds_squared, wpi::units::inverse<wpi::units::dimensionless::scalar>>>> {
  static constexpr auto name = _("wpimath.units.unit_seconds_squared_per_unit");
};

template <>
struct handle_type_name<wpi::units::unit_t<
    wpi::units::compound_unit<wpi::units::meters_per_second_squared, wpi::units::inverse<wpi::units::volts>>>> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared_per_volt");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
