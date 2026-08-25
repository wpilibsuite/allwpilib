#pragma once

#include <wpi/units/torque.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::newton_meters<>> {
  static constexpr auto name = _("wpimath.units.newton_meters");
};

template <> struct handle_type_name<wpi::units::newton_meters_> {
  static constexpr auto name = _("wpimath.units.newton_meters");
};

template <> struct handle_type_name<wpi::units::pound_feet<>> {
  static constexpr auto name = _("wpimath.units.pound_feet");
};

template <> struct handle_type_name<wpi::units::pound_feet_> {
  static constexpr auto name = _("wpimath.units.pound_feet");
};

template <> struct handle_type_name<wpi::units::foot_poundals<>> {
  static constexpr auto name = _("wpimath.units.foot_poundals");
};

template <> struct handle_type_name<wpi::units::foot_poundals_> {
  static constexpr auto name = _("wpimath.units.foot_poundals");
};

template <> struct handle_type_name<wpi::units::inch_pounds<>> {
  static constexpr auto name = _("wpimath.units.inch_pounds");
};

template <> struct handle_type_name<wpi::units::inch_pounds_> {
  static constexpr auto name = _("wpimath.units.inch_pounds");
};

template <> struct handle_type_name<wpi::units::meter_kilograms<>> {
  static constexpr auto name = _("wpimath.units.meter_kilograms");
};

template <> struct handle_type_name<wpi::units::meter_kilograms_> {
  static constexpr auto name = _("wpimath.units.meter_kilograms");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
