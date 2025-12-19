#pragma once

#include <wpi/units/area.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::square_meters<>> {
  static constexpr auto name = _("wpimath.units.square_meters");
};

template <> struct handle_type_name<wpi::units::square_meters_> {
  static constexpr auto name = _("wpimath.units.square_meters");
};

template <> struct handle_type_name<wpi::units::square_feet<>> {
  static constexpr auto name = _("wpimath.units.square_feet");
};

template <> struct handle_type_name<wpi::units::square_feet_> {
  static constexpr auto name = _("wpimath.units.square_feet");
};

template <> struct handle_type_name<wpi::units::square_inches<>> {
  static constexpr auto name = _("wpimath.units.square_inches");
};

template <> struct handle_type_name<wpi::units::square_inches_> {
  static constexpr auto name = _("wpimath.units.square_inches");
};

template <> struct handle_type_name<wpi::units::square_miles<>> {
  static constexpr auto name = _("wpimath.units.square_miles");
};

template <> struct handle_type_name<wpi::units::square_miles_> {
  static constexpr auto name = _("wpimath.units.square_miles");
};

template <> struct handle_type_name<wpi::units::square_kilometers<>> {
  static constexpr auto name = _("wpimath.units.square_kilometers");
};

template <> struct handle_type_name<wpi::units::square_kilometers_> {
  static constexpr auto name = _("wpimath.units.square_kilometers");
};

template <> struct handle_type_name<wpi::units::hectares<>> {
  static constexpr auto name = _("wpimath.units.hectares");
};

template <> struct handle_type_name<wpi::units::hectares_> {
  static constexpr auto name = _("wpimath.units.hectares");
};

template <> struct handle_type_name<wpi::units::acres<>> {
  static constexpr auto name = _("wpimath.units.acres");
};

template <> struct handle_type_name<wpi::units::acres_> {
  static constexpr auto name = _("wpimath.units.acres");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
