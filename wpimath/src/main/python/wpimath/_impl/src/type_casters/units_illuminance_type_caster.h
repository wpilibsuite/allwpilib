#pragma once

#include <wpi/units/illuminance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::lux<>> {
  static constexpr auto name = _("wpimath.units.lux");
};

template <> struct handle_type_name<wpi::units::lux_> {
  static constexpr auto name = _("wpimath.units.lux");
};

template <> struct handle_type_name<wpi::units::nanolux<>> {
  static constexpr auto name = _("wpimath.units.nanolux");
};

template <> struct handle_type_name<wpi::units::nanolux_> {
  static constexpr auto name = _("wpimath.units.nanolux");
};

template <> struct handle_type_name<wpi::units::microlux<>> {
  static constexpr auto name = _("wpimath.units.microlux");
};

template <> struct handle_type_name<wpi::units::microlux_> {
  static constexpr auto name = _("wpimath.units.microlux");
};

template <> struct handle_type_name<wpi::units::millilux<>> {
  static constexpr auto name = _("wpimath.units.millilux");
};

template <> struct handle_type_name<wpi::units::millilux_> {
  static constexpr auto name = _("wpimath.units.millilux");
};

template <> struct handle_type_name<wpi::units::kilolux<>> {
  static constexpr auto name = _("wpimath.units.kilolux");
};

template <> struct handle_type_name<wpi::units::kilolux_> {
  static constexpr auto name = _("wpimath.units.kilolux");
};

template <> struct handle_type_name<wpi::units::footcandles<>> {
  static constexpr auto name = _("wpimath.units.footcandles");
};

template <> struct handle_type_name<wpi::units::footcandles_> {
  static constexpr auto name = _("wpimath.units.footcandles");
};

template <> struct handle_type_name<wpi::units::lumens_per_square_inch<>> {
  static constexpr auto name = _("wpimath.units.lumens_per_square_inch");
};

template <> struct handle_type_name<wpi::units::lumens_per_square_inch_> {
  static constexpr auto name = _("wpimath.units.lumens_per_square_inch");
};

template <> struct handle_type_name<wpi::units::phots<>> {
  static constexpr auto name = _("wpimath.units.phots");
};

template <> struct handle_type_name<wpi::units::phots_> {
  static constexpr auto name = _("wpimath.units.phots");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
