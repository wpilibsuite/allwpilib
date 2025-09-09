#pragma once

#include <units/angular_acceleration.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::radians_per_second_squared_t> {
  static constexpr auto name = _("wpimath.units.radians_per_second_squared");
};

template <> struct handle_type_name<units::radians_per_second_squared> {
  static constexpr auto name = _("wpimath.units.radians_per_second_squared");
};

template <> struct handle_type_name<units::degrees_per_second_squared_t> {
  static constexpr auto name = _("wpimath.units.degrees_per_second_squared");
};

template <> struct handle_type_name<units::degrees_per_second_squared> {
  static constexpr auto name = _("wpimath.units.degrees_per_second_squared");
};

template <> struct handle_type_name<units::turns_per_second_squared_t> {
  static constexpr auto name = _("wpimath.units.turns_per_second_squared");
};

template <> struct handle_type_name<units::turns_per_second_squared> {
  static constexpr auto name = _("wpimath.units.turns_per_second_squared");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
