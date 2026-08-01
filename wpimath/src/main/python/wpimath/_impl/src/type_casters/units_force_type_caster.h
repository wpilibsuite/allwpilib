#pragma once

#include "wpi/units/force.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::force::newton_t> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<wpi::units::force::newtons> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<wpi::units::force::nanonewton_t> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<wpi::units::force::nanonewtons> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<wpi::units::force::micronewton_t> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<wpi::units::force::micronewtons> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<wpi::units::force::millinewton_t> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<wpi::units::force::millinewtons> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<wpi::units::force::kilonewton_t> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<wpi::units::force::kilonewtons> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<wpi::units::force::pound_t> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<wpi::units::force::pounds> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<wpi::units::force::dyne_t> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<wpi::units::force::dynes> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<wpi::units::force::kilopond_t> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<wpi::units::force::kiloponds> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<wpi::units::force::poundal_t> {
  static constexpr auto name = _("wpimath.units.poundals");
};

template <> struct handle_type_name<wpi::units::force::poundals> {
  static constexpr auto name = _("wpimath.units.poundals");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
