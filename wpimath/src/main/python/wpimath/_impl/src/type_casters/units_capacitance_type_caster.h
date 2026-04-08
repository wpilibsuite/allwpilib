#pragma once

#include "wpi/units/capacitance.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::farad_t> {
  static constexpr auto name = _("wpimath.units.farads");
};

template <> struct handle_type_name<wpi::units::farads> {
  static constexpr auto name = _("wpimath.units.farads");
};

template <> struct handle_type_name<wpi::units::nanofarad_t> {
  static constexpr auto name = _("wpimath.units.nanofarads");
};

template <> struct handle_type_name<wpi::units::nanofarads> {
  static constexpr auto name = _("wpimath.units.nanofarads");
};

template <> struct handle_type_name<wpi::units::microfarad_t> {
  static constexpr auto name = _("wpimath.units.microfarads");
};

template <> struct handle_type_name<wpi::units::microfarads> {
  static constexpr auto name = _("wpimath.units.microfarads");
};

template <> struct handle_type_name<wpi::units::millifarad_t> {
  static constexpr auto name = _("wpimath.units.millifarads");
};

template <> struct handle_type_name<wpi::units::millifarads> {
  static constexpr auto name = _("wpimath.units.millifarads");
};

template <> struct handle_type_name<wpi::units::kilofarad_t> {
  static constexpr auto name = _("wpimath.units.kilofarads");
};

template <> struct handle_type_name<wpi::units::kilofarads> {
  static constexpr auto name = _("wpimath.units.kilofarads");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
