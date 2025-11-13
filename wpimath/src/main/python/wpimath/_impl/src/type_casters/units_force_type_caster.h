#pragma once

#include <units/force.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::newton_t> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<units::newtons> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<units::nanonewton_t> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<units::nanonewtons> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<units::micronewton_t> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<units::micronewtons> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<units::millinewton_t> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<units::millinewtons> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<units::kilonewton_t> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<units::kilonewtons> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<units::pound_t> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<units::pounds> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<units::dyne_t> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<units::dynes> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<units::kilopond_t> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<units::kiloponds> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<units::poundal_t> {
  static constexpr auto name = _("wpimath.units.poundals");
};

template <> struct handle_type_name<units::poundals> {
  static constexpr auto name = _("wpimath.units.poundals");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
