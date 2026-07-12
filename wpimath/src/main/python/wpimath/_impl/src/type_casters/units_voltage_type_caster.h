#pragma once

#include "wpi/units/voltage.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::volt_t> {
  static constexpr auto name = _("wpimath.units.volts");
};

template <> struct handle_type_name<wpi::units::volts> {
  static constexpr auto name = _("wpimath.units.volts");
};

template <> struct handle_type_name<wpi::units::nanovolt_t> {
  static constexpr auto name = _("wpimath.units.nanovolts");
};

template <> struct handle_type_name<wpi::units::nanovolts> {
  static constexpr auto name = _("wpimath.units.nanovolts");
};

template <> struct handle_type_name<wpi::units::microvolt_t> {
  static constexpr auto name = _("wpimath.units.microvolts");
};

template <> struct handle_type_name<wpi::units::microvolts> {
  static constexpr auto name = _("wpimath.units.microvolts");
};

template <> struct handle_type_name<wpi::units::millivolt_t> {
  static constexpr auto name = _("wpimath.units.millivolts");
};

template <> struct handle_type_name<wpi::units::millivolts> {
  static constexpr auto name = _("wpimath.units.millivolts");
};

template <> struct handle_type_name<wpi::units::kilovolt_t> {
  static constexpr auto name = _("wpimath.units.kilovolts");
};

template <> struct handle_type_name<wpi::units::kilovolts> {
  static constexpr auto name = _("wpimath.units.kilovolts");
};

template <> struct handle_type_name<wpi::units::statvolt_t> {
  static constexpr auto name = _("wpimath.units.statvolts");
};

template <> struct handle_type_name<wpi::units::statvolts> {
  static constexpr auto name = _("wpimath.units.statvolts");
};

template <> struct handle_type_name<wpi::units::abvolt_t> {
  static constexpr auto name = _("wpimath.units.abvolts");
};

template <> struct handle_type_name<wpi::units::abvolts> {
  static constexpr auto name = _("wpimath.units.abvolts");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
