#pragma once

#include <wpi/units/jerk.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::meters_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.meters_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::meters_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.meters_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::nanometers_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.nanometers_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::nanometers_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.nanometers_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::micrometers_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.micrometers_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::micrometers_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.micrometers_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::millimeters_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.millimeters_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::millimeters_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.millimeters_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::kilometers_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.kilometers_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::kilometers_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.kilometers_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::feet_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.feet_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::feet_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.feet_per_second_cubed");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
