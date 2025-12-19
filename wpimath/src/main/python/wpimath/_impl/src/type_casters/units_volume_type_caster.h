#pragma once

#include <wpi/units/volume.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::cubic_meters<>> {
  static constexpr auto name = _("wpimath.units.cubic_meters");
};

template <> struct handle_type_name<wpi::units::cubic_meters_> {
  static constexpr auto name = _("wpimath.units.cubic_meters");
};

template <> struct handle_type_name<wpi::units::cubic_millimeters<>> {
  static constexpr auto name = _("wpimath.units.cubic_millimeters");
};

template <> struct handle_type_name<wpi::units::cubic_millimeters_> {
  static constexpr auto name = _("wpimath.units.cubic_millimeters");
};

template <> struct handle_type_name<wpi::units::cubic_kilometers<>> {
  static constexpr auto name = _("wpimath.units.cubic_kilometers");
};

template <> struct handle_type_name<wpi::units::cubic_kilometers_> {
  static constexpr auto name = _("wpimath.units.cubic_kilometers");
};

template <> struct handle_type_name<wpi::units::liters<>> {
  static constexpr auto name = _("wpimath.units.liters");
};

template <> struct handle_type_name<wpi::units::liters_> {
  static constexpr auto name = _("wpimath.units.liters");
};

template <> struct handle_type_name<wpi::units::nanoliters<>> {
  static constexpr auto name = _("wpimath.units.nanoliters");
};

template <> struct handle_type_name<wpi::units::nanoliters_> {
  static constexpr auto name = _("wpimath.units.nanoliters");
};

template <> struct handle_type_name<wpi::units::microliters<>> {
  static constexpr auto name = _("wpimath.units.microliters");
};

template <> struct handle_type_name<wpi::units::microliters_> {
  static constexpr auto name = _("wpimath.units.microliters");
};

template <> struct handle_type_name<wpi::units::milliliters<>> {
  static constexpr auto name = _("wpimath.units.milliliters");
};

template <> struct handle_type_name<wpi::units::milliliters_> {
  static constexpr auto name = _("wpimath.units.milliliters");
};

template <> struct handle_type_name<wpi::units::kiloliters<>> {
  static constexpr auto name = _("wpimath.units.kiloliters");
};

template <> struct handle_type_name<wpi::units::kiloliters_> {
  static constexpr auto name = _("wpimath.units.kiloliters");
};

template <> struct handle_type_name<wpi::units::cubic_inches<>> {
  static constexpr auto name = _("wpimath.units.cubic_inches");
};

template <> struct handle_type_name<wpi::units::cubic_inches_> {
  static constexpr auto name = _("wpimath.units.cubic_inches");
};

template <> struct handle_type_name<wpi::units::cubic_feet<>> {
  static constexpr auto name = _("wpimath.units.cubic_feet");
};

template <> struct handle_type_name<wpi::units::cubic_feet_> {
  static constexpr auto name = _("wpimath.units.cubic_feet");
};

template <> struct handle_type_name<wpi::units::cubic_yards<>> {
  static constexpr auto name = _("wpimath.units.cubic_yards");
};

template <> struct handle_type_name<wpi::units::cubic_yards_> {
  static constexpr auto name = _("wpimath.units.cubic_yards");
};

template <> struct handle_type_name<wpi::units::cubic_miles<>> {
  static constexpr auto name = _("wpimath.units.cubic_miles");
};

template <> struct handle_type_name<wpi::units::cubic_miles_> {
  static constexpr auto name = _("wpimath.units.cubic_miles");
};

template <> struct handle_type_name<wpi::units::gallons<>> {
  static constexpr auto name = _("wpimath.units.gallons");
};

template <> struct handle_type_name<wpi::units::gallons_> {
  static constexpr auto name = _("wpimath.units.gallons");
};

template <> struct handle_type_name<wpi::units::quarts<>> {
  static constexpr auto name = _("wpimath.units.quarts");
};

template <> struct handle_type_name<wpi::units::quarts_> {
  static constexpr auto name = _("wpimath.units.quarts");
};

template <> struct handle_type_name<wpi::units::pints<>> {
  static constexpr auto name = _("wpimath.units.pints");
};

template <> struct handle_type_name<wpi::units::pints_> {
  static constexpr auto name = _("wpimath.units.pints");
};

template <> struct handle_type_name<wpi::units::cups<>> {
  static constexpr auto name = _("wpimath.units.cups");
};

template <> struct handle_type_name<wpi::units::cups_> {
  static constexpr auto name = _("wpimath.units.cups");
};

template <> struct handle_type_name<wpi::units::fluid_ounces<>> {
  static constexpr auto name = _("wpimath.units.fluid_ounces");
};

template <> struct handle_type_name<wpi::units::fluid_ounces_> {
  static constexpr auto name = _("wpimath.units.fluid_ounces");
};

template <> struct handle_type_name<wpi::units::barrels<>> {
  static constexpr auto name = _("wpimath.units.barrels");
};

template <> struct handle_type_name<wpi::units::barrels_> {
  static constexpr auto name = _("wpimath.units.barrels");
};

template <> struct handle_type_name<wpi::units::bushels<>> {
  static constexpr auto name = _("wpimath.units.bushels");
};

template <> struct handle_type_name<wpi::units::bushels_> {
  static constexpr auto name = _("wpimath.units.bushels");
};

template <> struct handle_type_name<wpi::units::cords<>> {
  static constexpr auto name = _("wpimath.units.cords");
};

template <> struct handle_type_name<wpi::units::cords_> {
  static constexpr auto name = _("wpimath.units.cords");
};

template <> struct handle_type_name<wpi::units::cubic_fathoms<>> {
  static constexpr auto name = _("wpimath.units.cubic_fathoms");
};

template <> struct handle_type_name<wpi::units::cubic_fathoms_> {
  static constexpr auto name = _("wpimath.units.cubic_fathoms");
};

template <> struct handle_type_name<wpi::units::tablespoons<>> {
  static constexpr auto name = _("wpimath.units.tablespoons");
};

template <> struct handle_type_name<wpi::units::tablespoons_> {
  static constexpr auto name = _("wpimath.units.tablespoons");
};

template <> struct handle_type_name<wpi::units::teaspoons<>> {
  static constexpr auto name = _("wpimath.units.teaspoons");
};

template <> struct handle_type_name<wpi::units::teaspoons_> {
  static constexpr auto name = _("wpimath.units.teaspoons");
};

template <> struct handle_type_name<wpi::units::pinches<>> {
  static constexpr auto name = _("wpimath.units.pinches");
};

template <> struct handle_type_name<wpi::units::pinches_> {
  static constexpr auto name = _("wpimath.units.pinches");
};

template <> struct handle_type_name<wpi::units::dashes<>> {
  static constexpr auto name = _("wpimath.units.dashes");
};

template <> struct handle_type_name<wpi::units::dashes_> {
  static constexpr auto name = _("wpimath.units.dashes");
};

template <> struct handle_type_name<wpi::units::drops<>> {
  static constexpr auto name = _("wpimath.units.drops");
};

template <> struct handle_type_name<wpi::units::drops_> {
  static constexpr auto name = _("wpimath.units.drops");
};

template <> struct handle_type_name<wpi::units::fifths<>> {
  static constexpr auto name = _("wpimath.units.fifths");
};

template <> struct handle_type_name<wpi::units::fifths_> {
  static constexpr auto name = _("wpimath.units.fifths");
};

template <> struct handle_type_name<wpi::units::drams<>> {
  static constexpr auto name = _("wpimath.units.drams");
};

template <> struct handle_type_name<wpi::units::drams_> {
  static constexpr auto name = _("wpimath.units.drams");
};

template <> struct handle_type_name<wpi::units::gills<>> {
  static constexpr auto name = _("wpimath.units.gills");
};

template <> struct handle_type_name<wpi::units::gills_> {
  static constexpr auto name = _("wpimath.units.gills");
};

template <> struct handle_type_name<wpi::units::pecks<>> {
  static constexpr auto name = _("wpimath.units.pecks");
};

template <> struct handle_type_name<wpi::units::pecks_> {
  static constexpr auto name = _("wpimath.units.pecks");
};

template <> struct handle_type_name<wpi::units::sacks<>> {
  static constexpr auto name = _("wpimath.units.sacks");
};

template <> struct handle_type_name<wpi::units::sacks_> {
  static constexpr auto name = _("wpimath.units.sacks");
};

template <> struct handle_type_name<wpi::units::shots<>> {
  static constexpr auto name = _("wpimath.units.shots");
};

template <> struct handle_type_name<wpi::units::shots_> {
  static constexpr auto name = _("wpimath.units.shots");
};

template <> struct handle_type_name<wpi::units::strikes<>> {
  static constexpr auto name = _("wpimath.units.strikes");
};

template <> struct handle_type_name<wpi::units::strikes_> {
  static constexpr auto name = _("wpimath.units.strikes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
