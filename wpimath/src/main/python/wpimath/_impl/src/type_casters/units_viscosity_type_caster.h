#pragma once

#include <wpi/units/viscosity.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::pascal_seconds<>> {
  static constexpr auto name = _("wpimath.units.pascal_seconds");
};

template <> struct handle_type_name<wpi::units::pascal_seconds_> {
  static constexpr auto name = _("wpimath.units.pascal_seconds");
};

template <> struct handle_type_name<wpi::units::poise<>> {
  static constexpr auto name = _("wpimath.units.poise");
};

template <> struct handle_type_name<wpi::units::poise_> {
  static constexpr auto name = _("wpimath.units.poise");
};

template <> struct handle_type_name<wpi::units::centipoise<>> {
  static constexpr auto name = _("wpimath.units.centipoise");
};

template <> struct handle_type_name<wpi::units::centipoise_> {
  static constexpr auto name = _("wpimath.units.centipoise");
};

template <> struct handle_type_name<wpi::units::square_meters_per_second<>> {
  static constexpr auto name = _("wpimath.units.square_meters_per_second");
};

template <> struct handle_type_name<wpi::units::square_meters_per_second_> {
  static constexpr auto name = _("wpimath.units.square_meters_per_second");
};

template <> struct handle_type_name<wpi::units::stokes<>> {
  static constexpr auto name = _("wpimath.units.stokes");
};

template <> struct handle_type_name<wpi::units::stokes_> {
  static constexpr auto name = _("wpimath.units.stokes");
};

template <> struct handle_type_name<wpi::units::centistokes<>> {
  static constexpr auto name = _("wpimath.units.centistokes");
};

template <> struct handle_type_name<wpi::units::centistokes_> {
  static constexpr auto name = _("wpimath.units.centistokes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
