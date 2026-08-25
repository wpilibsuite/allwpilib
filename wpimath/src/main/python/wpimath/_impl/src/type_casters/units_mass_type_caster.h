#pragma once

#include <wpi/units/mass.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::grams<>> {
  static constexpr auto name = _("wpimath.units.grams");
};

template <> struct handle_type_name<wpi::units::grams_> {
  static constexpr auto name = _("wpimath.units.grams");
};

template <> struct handle_type_name<wpi::units::nanograms<>> {
  static constexpr auto name = _("wpimath.units.nanograms");
};

template <> struct handle_type_name<wpi::units::nanograms_> {
  static constexpr auto name = _("wpimath.units.nanograms");
};

template <> struct handle_type_name<wpi::units::micrograms<>> {
  static constexpr auto name = _("wpimath.units.micrograms");
};

template <> struct handle_type_name<wpi::units::micrograms_> {
  static constexpr auto name = _("wpimath.units.micrograms");
};

template <> struct handle_type_name<wpi::units::milligrams<>> {
  static constexpr auto name = _("wpimath.units.milligrams");
};

template <> struct handle_type_name<wpi::units::milligrams_> {
  static constexpr auto name = _("wpimath.units.milligrams");
};

template <> struct handle_type_name<wpi::units::kilograms<>> {
  static constexpr auto name = _("wpimath.units.kilograms");
};

template <> struct handle_type_name<wpi::units::kilograms_> {
  static constexpr auto name = _("wpimath.units.kilograms");
};

template <> struct handle_type_name<wpi::units::tonnes<>> {
  static constexpr auto name = _("wpimath.units.tonnes");
};

template <> struct handle_type_name<wpi::units::tonnes_> {
  static constexpr auto name = _("wpimath.units.tonnes");
};

template <> struct handle_type_name<wpi::units::mass::pounds<>> {
  static constexpr auto name = _("wpimath.units.pounds_mass");
};

template <> struct handle_type_name<wpi::units::mass::pounds_> {
  static constexpr auto name = _("wpimath.units.pounds_mass");
};

template <> struct handle_type_name<wpi::units::long_tons<>> {
  static constexpr auto name = _("wpimath.units.long_tons");
};

template <> struct handle_type_name<wpi::units::long_tons_> {
  static constexpr auto name = _("wpimath.units.long_tons");
};

template <> struct handle_type_name<wpi::units::short_tons<>> {
  static constexpr auto name = _("wpimath.units.short_tons");
};

template <> struct handle_type_name<wpi::units::short_tons_> {
  static constexpr auto name = _("wpimath.units.short_tons");
};

template <> struct handle_type_name<wpi::units::stone<>> {
  static constexpr auto name = _("wpimath.units.stone");
};

template <> struct handle_type_name<wpi::units::stone_> {
  static constexpr auto name = _("wpimath.units.stone");
};

template <> struct handle_type_name<wpi::units::ounces<>> {
  static constexpr auto name = _("wpimath.units.ounces");
};

template <> struct handle_type_name<wpi::units::ounces_> {
  static constexpr auto name = _("wpimath.units.ounces");
};

template <> struct handle_type_name<wpi::units::carats<>> {
  static constexpr auto name = _("wpimath.units.carats");
};

template <> struct handle_type_name<wpi::units::carats_> {
  static constexpr auto name = _("wpimath.units.carats");
};

template <> struct handle_type_name<wpi::units::slugs<>> {
  static constexpr auto name = _("wpimath.units.slugs");
};

template <> struct handle_type_name<wpi::units::slugs_> {
  static constexpr auto name = _("wpimath.units.slugs");
};

template <> struct handle_type_name<wpi::units::grains<>> {
  static constexpr auto name = _("wpimath.units.grains");
};

template <> struct handle_type_name<wpi::units::grains_> {
  static constexpr auto name = _("wpimath.units.grains");
};

template <> struct handle_type_name<wpi::units::avoirdupois_drams<>> {
  static constexpr auto name = _("wpimath.units.avoirdupois_drams");
};

template <> struct handle_type_name<wpi::units::avoirdupois_drams_> {
  static constexpr auto name = _("wpimath.units.avoirdupois_drams");
};

template <> struct handle_type_name<wpi::units::pennyweights<>> {
  static constexpr auto name = _("wpimath.units.pennyweights");
};

template <> struct handle_type_name<wpi::units::pennyweights_> {
  static constexpr auto name = _("wpimath.units.pennyweights");
};

template <> struct handle_type_name<wpi::units::troy_ounces<>> {
  static constexpr auto name = _("wpimath.units.troy_ounces");
};

template <> struct handle_type_name<wpi::units::troy_ounces_> {
  static constexpr auto name = _("wpimath.units.troy_ounces");
};

template <> struct handle_type_name<wpi::units::troy_pounds<>> {
  static constexpr auto name = _("wpimath.units.troy_pounds");
};

template <> struct handle_type_name<wpi::units::troy_pounds_> {
  static constexpr auto name = _("wpimath.units.troy_pounds");
};

template <> struct handle_type_name<wpi::units::hundredweights<>> {
  static constexpr auto name = _("wpimath.units.hundredweights");
};

template <> struct handle_type_name<wpi::units::hundredweights_> {
  static constexpr auto name = _("wpimath.units.hundredweights");
};

template <> struct handle_type_name<wpi::units::short_hundredweights<>> {
  static constexpr auto name = _("wpimath.units.short_hundredweights");
};

template <> struct handle_type_name<wpi::units::short_hundredweights_> {
  static constexpr auto name = _("wpimath.units.short_hundredweights");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
