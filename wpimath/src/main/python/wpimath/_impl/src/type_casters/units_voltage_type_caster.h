#pragma once

#include <units/voltage.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::volt_t> {
  static constexpr auto name = _("wpimath.units.volts");
};

template <> struct handle_type_name<units::volts> {
  static constexpr auto name = _("wpimath.units.volts");
};

template <> struct handle_type_name<units::nanovolt_t> {
  static constexpr auto name = _("wpimath.units.nanovolts");
};

template <> struct handle_type_name<units::nanovolts> {
  static constexpr auto name = _("wpimath.units.nanovolts");
};

template <> struct handle_type_name<units::microvolt_t> {
  static constexpr auto name = _("wpimath.units.microvolts");
};

template <> struct handle_type_name<units::microvolts> {
  static constexpr auto name = _("wpimath.units.microvolts");
};

template <> struct handle_type_name<units::millivolt_t> {
  static constexpr auto name = _("wpimath.units.millivolts");
};

template <> struct handle_type_name<units::millivolts> {
  static constexpr auto name = _("wpimath.units.millivolts");
};

template <> struct handle_type_name<units::kilovolt_t> {
  static constexpr auto name = _("wpimath.units.kilovolts");
};

template <> struct handle_type_name<units::kilovolts> {
  static constexpr auto name = _("wpimath.units.kilovolts");
};

template <> struct handle_type_name<units::statvolt_t> {
  static constexpr auto name = _("wpimath.units.statvolts");
};

template <> struct handle_type_name<units::statvolts> {
  static constexpr auto name = _("wpimath.units.statvolts");
};

template <> struct handle_type_name<units::abvolt_t> {
  static constexpr auto name = _("wpimath.units.abvolts");
};

template <> struct handle_type_name<units::abvolts> {
  static constexpr auto name = _("wpimath.units.abvolts");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
