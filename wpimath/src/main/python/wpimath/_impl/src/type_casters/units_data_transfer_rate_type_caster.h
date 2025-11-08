#pragma once

#include "wpi/units/data_transfer_rate.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::exabytes_per_second_t> {
  static constexpr auto name = _("wpimath.units.exabytes_per_second");
};

template <> struct handle_type_name<wpi::units::exabytes_per_second> {
  static constexpr auto name = _("wpimath.units.exabytes_per_second");
};

template <> struct handle_type_name<wpi::units::exabits_per_second_t> {
  static constexpr auto name = _("wpimath.units.exabits_per_second");
};

template <> struct handle_type_name<wpi::units::exabits_per_second> {
  static constexpr auto name = _("wpimath.units.exabits_per_second");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
