#pragma once

#include "wpi/units/data.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::exabyte_t> {
  static constexpr auto name = _("wpimath.units.exabytes");
};

template <> struct handle_type_name<wpi::units::exabytes> {
  static constexpr auto name = _("wpimath.units.exabytes");
};

template <> struct handle_type_name<wpi::units::exabit_t> {
  static constexpr auto name = _("wpimath.units.exabits");
};

template <> struct handle_type_name<wpi::units::exabits> {
  static constexpr auto name = _("wpimath.units.exabits");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
