#pragma once

#include <wpi/units/angle.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::radians<>> {
  static constexpr auto name = _("wpimath.units.radians");
};

template <> struct handle_type_name<wpi::units::radians_> {
  static constexpr auto name = _("wpimath.units.radians");
};

template <> struct handle_type_name<wpi::units::nanoradians<>> {
  static constexpr auto name = _("wpimath.units.nanoradians");
};

template <> struct handle_type_name<wpi::units::nanoradians_> {
  static constexpr auto name = _("wpimath.units.nanoradians");
};

template <> struct handle_type_name<wpi::units::microradians<>> {
  static constexpr auto name = _("wpimath.units.microradians");
};

template <> struct handle_type_name<wpi::units::microradians_> {
  static constexpr auto name = _("wpimath.units.microradians");
};

template <> struct handle_type_name<wpi::units::milliradians<>> {
  static constexpr auto name = _("wpimath.units.milliradians");
};

template <> struct handle_type_name<wpi::units::milliradians_> {
  static constexpr auto name = _("wpimath.units.milliradians");
};

template <> struct handle_type_name<wpi::units::kiloradians<>> {
  static constexpr auto name = _("wpimath.units.kiloradians");
};

template <> struct handle_type_name<wpi::units::kiloradians_> {
  static constexpr auto name = _("wpimath.units.kiloradians");
};

template <> struct handle_type_name<wpi::units::degrees<>> {
  static constexpr auto name = _("wpimath.units.degrees");
};

template <> struct handle_type_name<wpi::units::degrees_> {
  static constexpr auto name = _("wpimath.units.degrees");
};

template <> struct handle_type_name<wpi::units::arcminutes<>> {
  static constexpr auto name = _("wpimath.units.arcminutes");
};

template <> struct handle_type_name<wpi::units::arcminutes_> {
  static constexpr auto name = _("wpimath.units.arcminutes");
};

template <> struct handle_type_name<wpi::units::arcseconds<>> {
  static constexpr auto name = _("wpimath.units.arcseconds");
};

template <> struct handle_type_name<wpi::units::arcseconds_> {
  static constexpr auto name = _("wpimath.units.arcseconds");
};

template <> struct handle_type_name<wpi::units::milliarcseconds<>> {
  static constexpr auto name = _("wpimath.units.milliarcseconds");
};

template <> struct handle_type_name<wpi::units::milliarcseconds_> {
  static constexpr auto name = _("wpimath.units.milliarcseconds");
};

template <> struct handle_type_name<wpi::units::turns<>> {
  static constexpr auto name = _("wpimath.units.turns");
};

template <> struct handle_type_name<wpi::units::turns_> {
  static constexpr auto name = _("wpimath.units.turns");
};

template <> struct handle_type_name<wpi::units::gradians<>> {
  static constexpr auto name = _("wpimath.units.gradians");
};

template <> struct handle_type_name<wpi::units::gradians_> {
  static constexpr auto name = _("wpimath.units.gradians");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
