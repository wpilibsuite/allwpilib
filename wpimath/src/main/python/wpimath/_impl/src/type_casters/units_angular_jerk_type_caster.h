#pragma once

#include <wpi/units/angular_jerk.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::radians_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.radians_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::radians_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.radians_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::degrees_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.degrees_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::degrees_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.degrees_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::turns_per_second_cubed<>> {
  static constexpr auto name = _("wpimath.units.turns_per_second_cubed");
};

template <> struct handle_type_name<wpi::units::turns_per_second_cubed_> {
  static constexpr auto name = _("wpimath.units.turns_per_second_cubed");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
