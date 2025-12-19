#pragma once

#include <wpi/units/solid_angle.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::steradians<>> {
  static constexpr auto name = _("wpimath.units.steradians");
};

template <> struct handle_type_name<wpi::units::steradians_> {
  static constexpr auto name = _("wpimath.units.steradians");
};

template <> struct handle_type_name<wpi::units::nanosteradians<>> {
  static constexpr auto name = _("wpimath.units.nanosteradians");
};

template <> struct handle_type_name<wpi::units::nanosteradians_> {
  static constexpr auto name = _("wpimath.units.nanosteradians");
};

template <> struct handle_type_name<wpi::units::microsteradians<>> {
  static constexpr auto name = _("wpimath.units.microsteradians");
};

template <> struct handle_type_name<wpi::units::microsteradians_> {
  static constexpr auto name = _("wpimath.units.microsteradians");
};

template <> struct handle_type_name<wpi::units::millisteradians<>> {
  static constexpr auto name = _("wpimath.units.millisteradians");
};

template <> struct handle_type_name<wpi::units::millisteradians_> {
  static constexpr auto name = _("wpimath.units.millisteradians");
};

template <> struct handle_type_name<wpi::units::kilosteradians<>> {
  static constexpr auto name = _("wpimath.units.kilosteradians");
};

template <> struct handle_type_name<wpi::units::kilosteradians_> {
  static constexpr auto name = _("wpimath.units.kilosteradians");
};

template <> struct handle_type_name<wpi::units::degrees_squared<>> {
  static constexpr auto name = _("wpimath.units.degrees_squared");
};

template <> struct handle_type_name<wpi::units::degrees_squared_> {
  static constexpr auto name = _("wpimath.units.degrees_squared");
};

template <> struct handle_type_name<wpi::units::spats<>> {
  static constexpr auto name = _("wpimath.units.spats");
};

template <> struct handle_type_name<wpi::units::spats_> {
  static constexpr auto name = _("wpimath.units.spats");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
