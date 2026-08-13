#pragma once

#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

namespace pybind11 {
namespace detail {

template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<wpi::units::radians_, wpi::units::inverse<wpi::units::meters_>>>> {
  static constexpr auto name = _("wpimath.units.radians_per_meter");
};

template <>
struct handle_type_name<wpi::units::unit<wpi::units::compound_conversion_factor<
    wpi::units::radians_per_second_, wpi::units::inverse<wpi::units::volts_>>>> {
  static constexpr auto name = _("wpimath.units.radians_per_second_per_volt");
};

template <>
struct handle_type_name<wpi::units::unit<wpi::units::inverse<wpi::units::seconds_>>> {
  static constexpr auto name = _("wpimath.units.units_per_second");
};

template <>
struct handle_type_name<
    wpi::units::unit<wpi::units::inverse<wpi::units::squared<wpi::units::seconds_>>>> {
  static constexpr auto name = _("wpimath.units.units_per_second_squared");
};

using volt_seconds_ = wpi::units::compound_conversion_factor<wpi::units::volts_, wpi::units::seconds_>;
using volt_seconds_squared_ = wpi::units::compound_conversion_factor<volt_seconds_, wpi::units::seconds_>;

template <> struct handle_type_name<wpi::units::unit<volt_seconds_>> {
  static constexpr auto name = _("wpimath.units.volt_seconds");
};

template <> struct handle_type_name<wpi::units::unit<volt_seconds_squared_>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared");
};

template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<volt_seconds_, wpi::units::inverse<wpi::units::meters_>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_meter");
};
template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<volt_seconds_squared_, wpi::units::inverse<wpi::units::meters_>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_meter");
};
template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<volt_seconds_, wpi::units::inverse<wpi::units::feet_>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_feet");
};
template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<volt_seconds_squared_, wpi::units::inverse<wpi::units::feet_>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_feet");
};
template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<volt_seconds_, wpi::units::inverse<wpi::units::radians_>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_per_radian");
};
template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<wpi::units::meters_per_second_, wpi::units::inverse<wpi::units::radians_>>>> {
  static constexpr auto name = _("wpimath.units.meters_per_second_per_radian");
};
template <>
struct handle_type_name<wpi::units::unit<wpi::units::compound_conversion_factor<
    volt_seconds_squared_, wpi::units::inverse<wpi::units::radians_>>>> {
  static constexpr auto name = _("wpimath.units.volt_seconds_squared_per_radian");
};

using unit_seconds_ = wpi::units::compound_conversion_factor<wpi::units::dimensionless_, wpi::units::seconds_>;
using unit_seconds_squared_ = wpi::units::compound_conversion_factor<unit_seconds_, wpi::units::seconds_>;

template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<unit_seconds_squared_, wpi::units::inverse<wpi::units::dimensionless_>>>> {
  static constexpr auto name = _("wpimath.units.unit_seconds_squared_per_unit");
};

template <>
struct handle_type_name<wpi::units::unit<
    wpi::units::compound_conversion_factor<wpi::units::meters_per_second_squared_, wpi::units::inverse<wpi::units::volts_>>>> {
  static constexpr auto name = _("wpimath.units.meters_per_second_squared_per_volt");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
