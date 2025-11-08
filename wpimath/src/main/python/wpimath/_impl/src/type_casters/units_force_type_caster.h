#pragma once

#include "wpi/units/force.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::newton_t> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<wpi::units::newtons> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<wpi::units::nanonewton_t> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<wpi::units::nanonewtons> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<wpi::units::micronewton_t> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<wpi::units::micronewtons> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<wpi::units::millinewton_t> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<wpi::units::millinewtons> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<wpi::units::kilonewton_t> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<wpi::units::kilonewtons> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<wpi::units::pound_t> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<wpi::units::pounds> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<wpi::units::dyne_t> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<wpi::units::dynes> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<wpi::units::kilopond_t> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<wpi::units::kiloponds> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<wpi::units::poundal_t> {
  static constexpr auto name = _("wpimath.units.poundals");
};

template <> struct handle_type_name<wpi::units::poundals> {
  static constexpr auto name = _("wpimath.units.poundals");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
