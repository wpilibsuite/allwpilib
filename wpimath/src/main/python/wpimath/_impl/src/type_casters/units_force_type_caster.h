#pragma once

#include <wpi/units/force.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::newtons<>> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<wpi::units::newtons_> {
  static constexpr auto name = _("wpimath.units.newtons");
};

template <> struct handle_type_name<wpi::units::nanonewtons<>> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<wpi::units::nanonewtons_> {
  static constexpr auto name = _("wpimath.units.nanonewtons");
};

template <> struct handle_type_name<wpi::units::micronewtons<>> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<wpi::units::micronewtons_> {
  static constexpr auto name = _("wpimath.units.micronewtons");
};

template <> struct handle_type_name<wpi::units::millinewtons<>> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<wpi::units::millinewtons_> {
  static constexpr auto name = _("wpimath.units.millinewtons");
};

template <> struct handle_type_name<wpi::units::kilonewtons<>> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<wpi::units::kilonewtons_> {
  static constexpr auto name = _("wpimath.units.kilonewtons");
};

template <> struct handle_type_name<wpi::units::force::pounds<>> {
  static constexpr auto name = _("wpimath.units.pounds_force");
};

template <> struct handle_type_name<wpi::units::force::pounds_> {
  static constexpr auto name = _("wpimath.units.pounds_force");
};

template <> struct handle_type_name<wpi::units::dynes<>> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<wpi::units::dynes_> {
  static constexpr auto name = _("wpimath.units.dynes");
};

template <> struct handle_type_name<wpi::units::kiloponds<>> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<wpi::units::kiloponds_> {
  static constexpr auto name = _("wpimath.units.kiloponds");
};

template <> struct handle_type_name<wpi::units::poundals<>> {
  static constexpr auto name = _("wpimath.units.poundals");
};

template <> struct handle_type_name<wpi::units::poundals_> {
  static constexpr auto name = _("wpimath.units.poundals");
};

template <> struct handle_type_name<wpi::units::kips<>> {
  static constexpr auto name = _("wpimath.units.kips");
};

template <> struct handle_type_name<wpi::units::kips_> {
  static constexpr auto name = _("wpimath.units.kips");
};

template <> struct handle_type_name<wpi::units::ounces_force<>> {
  static constexpr auto name = _("wpimath.units.ounces_force");
};

template <> struct handle_type_name<wpi::units::ounces_force_> {
  static constexpr auto name = _("wpimath.units.ounces_force");
};

template <> struct handle_type_name<wpi::units::grams_force<>> {
  static constexpr auto name = _("wpimath.units.grams_force");
};

template <> struct handle_type_name<wpi::units::grams_force_> {
  static constexpr auto name = _("wpimath.units.grams_force");
};

template <> struct handle_type_name<wpi::units::short_tons_force<>> {
  static constexpr auto name = _("wpimath.units.short_tons_force");
};

template <> struct handle_type_name<wpi::units::short_tons_force_> {
  static constexpr auto name = _("wpimath.units.short_tons_force");
};

template <> struct handle_type_name<wpi::units::long_tons_force<>> {
  static constexpr auto name = _("wpimath.units.long_tons_force");
};

template <> struct handle_type_name<wpi::units::long_tons_force_> {
  static constexpr auto name = _("wpimath.units.long_tons_force");
};

template <> struct handle_type_name<wpi::units::sthenes<>> {
  static constexpr auto name = _("wpimath.units.sthenes");
};

template <> struct handle_type_name<wpi::units::sthenes_> {
  static constexpr auto name = _("wpimath.units.sthenes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
