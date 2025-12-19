#pragma once

#include <wpi/units/moment_of_inertia.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::kilogram_square_meters<>> {
  static constexpr auto name = _("wpimath.units.kilogram_square_meters");
};

template <> struct handle_type_name<wpi::units::kilogram_square_meters_> {
  static constexpr auto name = _("wpimath.units.kilogram_square_meters");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
