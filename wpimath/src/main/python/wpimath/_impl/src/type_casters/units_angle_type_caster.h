#pragma once

#include "wpi/units/angle.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::radian_t> {
  static constexpr auto name = _("wpimath.units.radians");
};

template <> struct handle_type_name<wpi::units::radians> {
  static constexpr auto name = _("wpimath.units.radians");
};

template <> struct handle_type_name<wpi::units::nanoradian_t> {
  static constexpr auto name = _("wpimath.units.nanoradians");
};

template <> struct handle_type_name<wpi::units::nanoradians> {
  static constexpr auto name = _("wpimath.units.nanoradians");
};

template <> struct handle_type_name<wpi::units::microradian_t> {
  static constexpr auto name = _("wpimath.units.microradians");
};

template <> struct handle_type_name<wpi::units::microradians> {
  static constexpr auto name = _("wpimath.units.microradians");
};

template <> struct handle_type_name<wpi::units::milliradian_t> {
  static constexpr auto name = _("wpimath.units.milliradians");
};

template <> struct handle_type_name<wpi::units::milliradians> {
  static constexpr auto name = _("wpimath.units.milliradians");
};

template <> struct handle_type_name<wpi::units::kiloradian_t> {
  static constexpr auto name = _("wpimath.units.kiloradians");
};

template <> struct handle_type_name<wpi::units::kiloradians> {
  static constexpr auto name = _("wpimath.units.kiloradians");
};

template <> struct handle_type_name<wpi::units::degree_t> {
  static constexpr auto name = _("wpimath.units.degrees");
};

template <> struct handle_type_name<wpi::units::degrees> {
  static constexpr auto name = _("wpimath.units.degrees");
};

template <> struct handle_type_name<wpi::units::arcminute_t> {
  static constexpr auto name = _("wpimath.units.arcminutes");
};

template <> struct handle_type_name<wpi::units::arcminutes> {
  static constexpr auto name = _("wpimath.units.arcminutes");
};

template <> struct handle_type_name<wpi::units::arcsecond_t> {
  static constexpr auto name = _("wpimath.units.arcseconds");
};

template <> struct handle_type_name<wpi::units::arcseconds> {
  static constexpr auto name = _("wpimath.units.arcseconds");
};

template <> struct handle_type_name<wpi::units::milliarcsecond_t> {
  static constexpr auto name = _("wpimath.units.milliarcseconds");
};

template <> struct handle_type_name<wpi::units::milliarcseconds> {
  static constexpr auto name = _("wpimath.units.milliarcseconds");
};

template <> struct handle_type_name<wpi::units::turn_t> {
  static constexpr auto name = _("wpimath.units.turns");
};

template <> struct handle_type_name<wpi::units::turns> {
  static constexpr auto name = _("wpimath.units.turns");
};

template <> struct handle_type_name<wpi::units::gradian_t> {
  static constexpr auto name = _("wpimath.units.gradians");
};

template <> struct handle_type_name<wpi::units::gradians> {
  static constexpr auto name = _("wpimath.units.gradians");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
