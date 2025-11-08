#pragma once

#include "wpi/units/substance.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::mole_t> {
  static constexpr auto name = _("wpimath.units.moles");
};

template <> struct handle_type_name<wpi::units::moles> {
  static constexpr auto name = _("wpimath.units.moles");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
