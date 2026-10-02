#pragma once

#include "wpi/units/volume.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::cubic_meter_t> {
  static constexpr auto name = _("wpimath.units.cubic_meters");
};

template <> struct handle_type_name<wpi::units::cubic_meters> {
  static constexpr auto name = _("wpimath.units.cubic_meters");
};

template <> struct handle_type_name<wpi::units::cubic_millimeter_t> {
  static constexpr auto name = _("wpimath.units.cubic_millimeters");
};

template <> struct handle_type_name<wpi::units::cubic_millimeters> {
  static constexpr auto name = _("wpimath.units.cubic_millimeters");
};

template <> struct handle_type_name<wpi::units::cubic_kilometer_t> {
  static constexpr auto name = _("wpimath.units.cubic_kilometers");
};

template <> struct handle_type_name<wpi::units::cubic_kilometers> {
  static constexpr auto name = _("wpimath.units.cubic_kilometers");
};

template <> struct handle_type_name<wpi::units::liter_t> {
  static constexpr auto name = _("wpimath.units.liters");
};

template <> struct handle_type_name<wpi::units::liters> {
  static constexpr auto name = _("wpimath.units.liters");
};

template <> struct handle_type_name<wpi::units::nanoliter_t> {
  static constexpr auto name = _("wpimath.units.nanoliters");
};

template <> struct handle_type_name<wpi::units::nanoliters> {
  static constexpr auto name = _("wpimath.units.nanoliters");
};

template <> struct handle_type_name<wpi::units::microliter_t> {
  static constexpr auto name = _("wpimath.units.microliters");
};

template <> struct handle_type_name<wpi::units::microliters> {
  static constexpr auto name = _("wpimath.units.microliters");
};

template <> struct handle_type_name<wpi::units::milliliter_t> {
  static constexpr auto name = _("wpimath.units.milliliters");
};

template <> struct handle_type_name<wpi::units::milliliters> {
  static constexpr auto name = _("wpimath.units.milliliters");
};

template <> struct handle_type_name<wpi::units::kiloliter_t> {
  static constexpr auto name = _("wpimath.units.kiloliters");
};

template <> struct handle_type_name<wpi::units::kiloliters> {
  static constexpr auto name = _("wpimath.units.kiloliters");
};

template <> struct handle_type_name<wpi::units::cubic_inch_t> {
  static constexpr auto name = _("wpimath.units.cubic_inches");
};

template <> struct handle_type_name<wpi::units::cubic_inches> {
  static constexpr auto name = _("wpimath.units.cubic_inches");
};

template <> struct handle_type_name<wpi::units::cubic_foot_t> {
  static constexpr auto name = _("wpimath.units.cubic_feet");
};

template <> struct handle_type_name<wpi::units::cubic_feet> {
  static constexpr auto name = _("wpimath.units.cubic_feet");
};

template <> struct handle_type_name<wpi::units::cubic_yard_t> {
  static constexpr auto name = _("wpimath.units.cubic_yards");
};

template <> struct handle_type_name<wpi::units::cubic_yards> {
  static constexpr auto name = _("wpimath.units.cubic_yards");
};

template <> struct handle_type_name<wpi::units::cubic_mile_t> {
  static constexpr auto name = _("wpimath.units.cubic_miles");
};

template <> struct handle_type_name<wpi::units::cubic_miles> {
  static constexpr auto name = _("wpimath.units.cubic_miles");
};

template <> struct handle_type_name<wpi::units::gallon_t> {
  static constexpr auto name = _("wpimath.units.gallons");
};

template <> struct handle_type_name<wpi::units::gallons> {
  static constexpr auto name = _("wpimath.units.gallons");
};

template <> struct handle_type_name<wpi::units::quart_t> {
  static constexpr auto name = _("wpimath.units.quarts");
};

template <> struct handle_type_name<wpi::units::quarts> {
  static constexpr auto name = _("wpimath.units.quarts");
};

template <> struct handle_type_name<wpi::units::pint_t> {
  static constexpr auto name = _("wpimath.units.pints");
};

template <> struct handle_type_name<wpi::units::pints> {
  static constexpr auto name = _("wpimath.units.pints");
};

template <> struct handle_type_name<wpi::units::cup_t> {
  static constexpr auto name = _("wpimath.units.cups");
};

template <> struct handle_type_name<wpi::units::cups> {
  static constexpr auto name = _("wpimath.units.cups");
};

template <> struct handle_type_name<wpi::units::fluid_ounce_t> {
  static constexpr auto name = _("wpimath.units.fluid_ounces");
};

template <> struct handle_type_name<wpi::units::fluid_ounces> {
  static constexpr auto name = _("wpimath.units.fluid_ounces");
};

template <> struct handle_type_name<wpi::units::barrel_t> {
  static constexpr auto name = _("wpimath.units.barrels");
};

template <> struct handle_type_name<wpi::units::barrels> {
  static constexpr auto name = _("wpimath.units.barrels");
};

template <> struct handle_type_name<wpi::units::bushel_t> {
  static constexpr auto name = _("wpimath.units.bushels");
};

template <> struct handle_type_name<wpi::units::bushels> {
  static constexpr auto name = _("wpimath.units.bushels");
};

template <> struct handle_type_name<wpi::units::cord_t> {
  static constexpr auto name = _("wpimath.units.cords");
};

template <> struct handle_type_name<wpi::units::cords> {
  static constexpr auto name = _("wpimath.units.cords");
};

template <> struct handle_type_name<wpi::units::cubic_fathom_t> {
  static constexpr auto name = _("wpimath.units.cubic_fathoms");
};

template <> struct handle_type_name<wpi::units::cubic_fathoms> {
  static constexpr auto name = _("wpimath.units.cubic_fathoms");
};

template <> struct handle_type_name<wpi::units::tablespoon_t> {
  static constexpr auto name = _("wpimath.units.tablespoons");
};

template <> struct handle_type_name<wpi::units::tablespoons> {
  static constexpr auto name = _("wpimath.units.tablespoons");
};

template <> struct handle_type_name<wpi::units::teaspoon_t> {
  static constexpr auto name = _("wpimath.units.teaspoons");
};

template <> struct handle_type_name<wpi::units::teaspoons> {
  static constexpr auto name = _("wpimath.units.teaspoons");
};

template <> struct handle_type_name<wpi::units::pinch_t> {
  static constexpr auto name = _("wpimath.units.pinches");
};

template <> struct handle_type_name<wpi::units::pinches> {
  static constexpr auto name = _("wpimath.units.pinches");
};

template <> struct handle_type_name<wpi::units::dash_t> {
  static constexpr auto name = _("wpimath.units.dashes");
};

template <> struct handle_type_name<wpi::units::dashes> {
  static constexpr auto name = _("wpimath.units.dashes");
};

template <> struct handle_type_name<wpi::units::drop_t> {
  static constexpr auto name = _("wpimath.units.drops");
};

template <> struct handle_type_name<wpi::units::drops> {
  static constexpr auto name = _("wpimath.units.drops");
};

template <> struct handle_type_name<wpi::units::fifth_t> {
  static constexpr auto name = _("wpimath.units.fifths");
};

template <> struct handle_type_name<wpi::units::fifths> {
  static constexpr auto name = _("wpimath.units.fifths");
};

template <> struct handle_type_name<wpi::units::dram_t> {
  static constexpr auto name = _("wpimath.units.drams");
};

template <> struct handle_type_name<wpi::units::drams> {
  static constexpr auto name = _("wpimath.units.drams");
};

template <> struct handle_type_name<wpi::units::gill_t> {
  static constexpr auto name = _("wpimath.units.gills");
};

template <> struct handle_type_name<wpi::units::gills> {
  static constexpr auto name = _("wpimath.units.gills");
};

template <> struct handle_type_name<wpi::units::peck_t> {
  static constexpr auto name = _("wpimath.units.pecks");
};

template <> struct handle_type_name<wpi::units::pecks> {
  static constexpr auto name = _("wpimath.units.pecks");
};

template <> struct handle_type_name<wpi::units::sack_t> {
  static constexpr auto name = _("wpimath.units.sacks");
};

template <> struct handle_type_name<wpi::units::sacks> {
  static constexpr auto name = _("wpimath.units.sacks");
};

template <> struct handle_type_name<wpi::units::shot_t> {
  static constexpr auto name = _("wpimath.units.shots");
};

template <> struct handle_type_name<wpi::units::shots> {
  static constexpr auto name = _("wpimath.units.shots");
};

template <> struct handle_type_name<wpi::units::strike_t> {
  static constexpr auto name = _("wpimath.units.strikes");
};

template <> struct handle_type_name<wpi::units::strikes> {
  static constexpr auto name = _("wpimath.units.strikes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
