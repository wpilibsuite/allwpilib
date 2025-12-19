#pragma once

#include <wpi/units/voltage.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::volts<>> {
  static constexpr auto name = _("wpimath.units.volts");
};

template <> struct handle_type_name<wpi::units::volts_> {
  static constexpr auto name = _("wpimath.units.volts");
};

template <> struct handle_type_name<wpi::units::nanovolts<>> {
  static constexpr auto name = _("wpimath.units.nanovolts");
};

template <> struct handle_type_name<wpi::units::nanovolts_> {
  static constexpr auto name = _("wpimath.units.nanovolts");
};

template <> struct handle_type_name<wpi::units::microvolts<>> {
  static constexpr auto name = _("wpimath.units.microvolts");
};

template <> struct handle_type_name<wpi::units::microvolts_> {
  static constexpr auto name = _("wpimath.units.microvolts");
};

template <> struct handle_type_name<wpi::units::millivolts<>> {
  static constexpr auto name = _("wpimath.units.millivolts");
};

template <> struct handle_type_name<wpi::units::millivolts_> {
  static constexpr auto name = _("wpimath.units.millivolts");
};

template <> struct handle_type_name<wpi::units::kilovolts<>> {
  static constexpr auto name = _("wpimath.units.kilovolts");
};

template <> struct handle_type_name<wpi::units::kilovolts_> {
  static constexpr auto name = _("wpimath.units.kilovolts");
};

template <> struct handle_type_name<wpi::units::statvolts<>> {
  static constexpr auto name = _("wpimath.units.statvolts");
};

template <> struct handle_type_name<wpi::units::statvolts_> {
  static constexpr auto name = _("wpimath.units.statvolts");
};

template <> struct handle_type_name<wpi::units::abvolts<>> {
  static constexpr auto name = _("wpimath.units.abvolts");
};

template <> struct handle_type_name<wpi::units::abvolts_> {
  static constexpr auto name = _("wpimath.units.abvolts");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
