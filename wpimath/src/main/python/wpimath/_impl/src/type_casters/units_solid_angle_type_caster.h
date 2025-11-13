#pragma once

#include "wpi/units/solid_angle.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::steradian_t> {
  static constexpr auto name = _("wpimath.units.steradians");
};

template <> struct handle_type_name<wpi::units::steradians> {
  static constexpr auto name = _("wpimath.units.steradians");
};

template <> struct handle_type_name<wpi::units::nanosteradian_t> {
  static constexpr auto name = _("wpimath.units.nanosteradians");
};

template <> struct handle_type_name<wpi::units::nanosteradians> {
  static constexpr auto name = _("wpimath.units.nanosteradians");
};

template <> struct handle_type_name<wpi::units::microsteradian_t> {
  static constexpr auto name = _("wpimath.units.microsteradians");
};

template <> struct handle_type_name<wpi::units::microsteradians> {
  static constexpr auto name = _("wpimath.units.microsteradians");
};

template <> struct handle_type_name<wpi::units::millisteradian_t> {
  static constexpr auto name = _("wpimath.units.millisteradians");
};

template <> struct handle_type_name<wpi::units::millisteradians> {
  static constexpr auto name = _("wpimath.units.millisteradians");
};

template <> struct handle_type_name<wpi::units::kilosteradian_t> {
  static constexpr auto name = _("wpimath.units.kilosteradians");
};

template <> struct handle_type_name<wpi::units::kilosteradians> {
  static constexpr auto name = _("wpimath.units.kilosteradians");
};

template <> struct handle_type_name<wpi::units::degree_squared_t> {
  static constexpr auto name = _("wpimath.units.degrees_squared");
};

template <> struct handle_type_name<wpi::units::degrees_squared> {
  static constexpr auto name = _("wpimath.units.degrees_squared");
};

template <> struct handle_type_name<wpi::units::spat_t> {
  static constexpr auto name = _("wpimath.units.spats");
};

template <> struct handle_type_name<wpi::units::spats> {
  static constexpr auto name = _("wpimath.units.spats");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
