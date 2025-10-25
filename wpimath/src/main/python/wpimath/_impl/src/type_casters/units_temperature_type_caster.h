#pragma once

#include <units/temperature.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::kelvin_t> {
  static constexpr auto name = _("wpimath.units.kelvin");
};

template <> struct handle_type_name<units::kelvin> {
  static constexpr auto name = _("wpimath.units.kelvin");
};

template <> struct handle_type_name<units::celsius_t> {
  static constexpr auto name = _("wpimath.units.celsius");
};

template <> struct handle_type_name<units::celsius> {
  static constexpr auto name = _("wpimath.units.celsius");
};

template <> struct handle_type_name<units::fahrenheit_t> {
  static constexpr auto name = _("wpimath.units.fahrenheit");
};

template <> struct handle_type_name<units::fahrenheit> {
  static constexpr auto name = _("wpimath.units.fahrenheit");
};

template <> struct handle_type_name<units::reaumur_t> {
  static constexpr auto name = _("wpimath.units.reaumur");
};

template <> struct handle_type_name<units::reaumur> {
  static constexpr auto name = _("wpimath.units.reaumur");
};

template <> struct handle_type_name<units::rankine_t> {
  static constexpr auto name = _("wpimath.units.rankine");
};

template <> struct handle_type_name<units::rankine> {
  static constexpr auto name = _("wpimath.units.rankine");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
