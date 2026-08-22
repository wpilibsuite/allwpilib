#pragma once

#include <wpi/units/magnetic_flux.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::webers<>> {
  static constexpr auto name = _("wpimath.units.webers");
};

template <> struct handle_type_name<wpi::units::webers_> {
  static constexpr auto name = _("wpimath.units.webers");
};

template <> struct handle_type_name<wpi::units::nanowebers<>> {
  static constexpr auto name = _("wpimath.units.nanowebers");
};

template <> struct handle_type_name<wpi::units::nanowebers_> {
  static constexpr auto name = _("wpimath.units.nanowebers");
};

template <> struct handle_type_name<wpi::units::microwebers<>> {
  static constexpr auto name = _("wpimath.units.microwebers");
};

template <> struct handle_type_name<wpi::units::microwebers_> {
  static constexpr auto name = _("wpimath.units.microwebers");
};

template <> struct handle_type_name<wpi::units::milliwebers<>> {
  static constexpr auto name = _("wpimath.units.milliwebers");
};

template <> struct handle_type_name<wpi::units::milliwebers_> {
  static constexpr auto name = _("wpimath.units.milliwebers");
};

template <> struct handle_type_name<wpi::units::kilowebers<>> {
  static constexpr auto name = _("wpimath.units.kilowebers");
};

template <> struct handle_type_name<wpi::units::kilowebers_> {
  static constexpr auto name = _("wpimath.units.kilowebers");
};

template <> struct handle_type_name<wpi::units::maxwells<>> {
  static constexpr auto name = _("wpimath.units.maxwells");
};

template <> struct handle_type_name<wpi::units::maxwells_> {
  static constexpr auto name = _("wpimath.units.maxwells");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
