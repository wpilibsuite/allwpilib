#pragma once

#include "wpi/units/torque.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::newton_meter_t> {
  static constexpr auto name = _("wpimath.units.newton_meters");
};

template <> struct handle_type_name<wpi::units::newton_meters> {
  static constexpr auto name = _("wpimath.units.newton_meters");
};

// template <> struct handle_type_name<wpi::units::foot_pound_t> {
//   static constexpr auto name = _("wpimath.units.foot_pounds");
// };

// template <> struct handle_type_name<wpi::units::foot_pounds> {
//   static constexpr auto name = _("wpimath.units.foot_pounds");
// };

template <> struct handle_type_name<wpi::units::foot_poundal_t> {
  static constexpr auto name = _("wpimath.units.foot_poundals");
};

template <> struct handle_type_name<wpi::units::foot_poundals> {
  static constexpr auto name = _("wpimath.units.foot_poundals");
};

template <> struct handle_type_name<wpi::units::inch_pound_t> {
  static constexpr auto name = _("wpimath.units.inch_pounds");
};

template <> struct handle_type_name<wpi::units::inch_pounds> {
  static constexpr auto name = _("wpimath.units.inch_pounds");
};

template <> struct handle_type_name<wpi::units::meter_kilogram_t> {
  static constexpr auto name = _("wpimath.units.meter_kilograms");
};

template <> struct handle_type_name<wpi::units::meter_kilograms> {
  static constexpr auto name = _("wpimath.units.meter_kilograms");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
