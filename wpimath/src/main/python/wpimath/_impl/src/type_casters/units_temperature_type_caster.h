#pragma once

#include "wpi/units/temperature.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::kelvin_t> {
  static constexpr auto name = _("wpimath.units.kelvin");
};

template <> struct handle_type_name<wpi::units::kelvin> {
  static constexpr auto name = _("wpimath.units.kelvin");
};

template <> struct handle_type_name<wpi::units::celsius_t> {
  static constexpr auto name = _("wpimath.units.celsius");
};

template <> struct handle_type_name<wpi::units::celsius> {
  static constexpr auto name = _("wpimath.units.celsius");
};

template <> struct handle_type_name<wpi::units::fahrenheit_t> {
  static constexpr auto name = _("wpimath.units.fahrenheit");
};

template <> struct handle_type_name<wpi::units::fahrenheit> {
  static constexpr auto name = _("wpimath.units.fahrenheit");
};

template <> struct handle_type_name<wpi::units::reaumur_t> {
  static constexpr auto name = _("wpimath.units.reaumur");
};

template <> struct handle_type_name<wpi::units::reaumur> {
  static constexpr auto name = _("wpimath.units.reaumur");
};

template <> struct handle_type_name<wpi::units::rankine_t> {
  static constexpr auto name = _("wpimath.units.rankine");
};

template <> struct handle_type_name<wpi::units::rankine> {
  static constexpr auto name = _("wpimath.units.rankine");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
