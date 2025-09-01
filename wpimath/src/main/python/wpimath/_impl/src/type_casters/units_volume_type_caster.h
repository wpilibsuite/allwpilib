#pragma once

#include <units/volume.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::cubic_meter_t> {
  static constexpr auto name = _("wpimath.units.cubic_meters");
};

template <> struct handle_type_name<units::cubic_meters> {
  static constexpr auto name = _("wpimath.units.cubic_meters");
};

template <> struct handle_type_name<units::cubic_millimeter_t> {
  static constexpr auto name = _("wpimath.units.cubic_millimeters");
};

template <> struct handle_type_name<units::cubic_millimeters> {
  static constexpr auto name = _("wpimath.units.cubic_millimeters");
};

template <> struct handle_type_name<units::cubic_kilometer_t> {
  static constexpr auto name = _("wpimath.units.cubic_kilometers");
};

template <> struct handle_type_name<units::cubic_kilometers> {
  static constexpr auto name = _("wpimath.units.cubic_kilometers");
};

template <> struct handle_type_name<units::liter_t> {
  static constexpr auto name = _("wpimath.units.liters");
};

template <> struct handle_type_name<units::liters> {
  static constexpr auto name = _("wpimath.units.liters");
};

template <> struct handle_type_name<units::nanoliter_t> {
  static constexpr auto name = _("wpimath.units.nanoliters");
};

template <> struct handle_type_name<units::nanoliters> {
  static constexpr auto name = _("wpimath.units.nanoliters");
};

template <> struct handle_type_name<units::microliter_t> {
  static constexpr auto name = _("wpimath.units.microliters");
};

template <> struct handle_type_name<units::microliters> {
  static constexpr auto name = _("wpimath.units.microliters");
};

template <> struct handle_type_name<units::milliliter_t> {
  static constexpr auto name = _("wpimath.units.milliliters");
};

template <> struct handle_type_name<units::milliliters> {
  static constexpr auto name = _("wpimath.units.milliliters");
};

template <> struct handle_type_name<units::kiloliter_t> {
  static constexpr auto name = _("wpimath.units.kiloliters");
};

template <> struct handle_type_name<units::kiloliters> {
  static constexpr auto name = _("wpimath.units.kiloliters");
};

template <> struct handle_type_name<units::cubic_inch_t> {
  static constexpr auto name = _("wpimath.units.cubic_inches");
};

template <> struct handle_type_name<units::cubic_inches> {
  static constexpr auto name = _("wpimath.units.cubic_inches");
};

template <> struct handle_type_name<units::cubic_foot_t> {
  static constexpr auto name = _("wpimath.units.cubic_feet");
};

template <> struct handle_type_name<units::cubic_feet> {
  static constexpr auto name = _("wpimath.units.cubic_feet");
};

template <> struct handle_type_name<units::cubic_yard_t> {
  static constexpr auto name = _("wpimath.units.cubic_yards");
};

template <> struct handle_type_name<units::cubic_yards> {
  static constexpr auto name = _("wpimath.units.cubic_yards");
};

template <> struct handle_type_name<units::cubic_mile_t> {
  static constexpr auto name = _("wpimath.units.cubic_miles");
};

template <> struct handle_type_name<units::cubic_miles> {
  static constexpr auto name = _("wpimath.units.cubic_miles");
};

template <> struct handle_type_name<units::gallon_t> {
  static constexpr auto name = _("wpimath.units.gallons");
};

template <> struct handle_type_name<units::gallons> {
  static constexpr auto name = _("wpimath.units.gallons");
};

template <> struct handle_type_name<units::quart_t> {
  static constexpr auto name = _("wpimath.units.quarts");
};

template <> struct handle_type_name<units::quarts> {
  static constexpr auto name = _("wpimath.units.quarts");
};

template <> struct handle_type_name<units::pint_t> {
  static constexpr auto name = _("wpimath.units.pints");
};

template <> struct handle_type_name<units::pints> {
  static constexpr auto name = _("wpimath.units.pints");
};

template <> struct handle_type_name<units::cup_t> {
  static constexpr auto name = _("wpimath.units.cups");
};

template <> struct handle_type_name<units::cups> {
  static constexpr auto name = _("wpimath.units.cups");
};

template <> struct handle_type_name<units::fluid_ounce_t> {
  static constexpr auto name = _("wpimath.units.fluid_ounces");
};

template <> struct handle_type_name<units::fluid_ounces> {
  static constexpr auto name = _("wpimath.units.fluid_ounces");
};

template <> struct handle_type_name<units::barrel_t> {
  static constexpr auto name = _("wpimath.units.barrels");
};

template <> struct handle_type_name<units::barrels> {
  static constexpr auto name = _("wpimath.units.barrels");
};

template <> struct handle_type_name<units::bushel_t> {
  static constexpr auto name = _("wpimath.units.bushels");
};

template <> struct handle_type_name<units::bushels> {
  static constexpr auto name = _("wpimath.units.bushels");
};

template <> struct handle_type_name<units::cord_t> {
  static constexpr auto name = _("wpimath.units.cords");
};

template <> struct handle_type_name<units::cords> {
  static constexpr auto name = _("wpimath.units.cords");
};

template <> struct handle_type_name<units::cubic_fathom_t> {
  static constexpr auto name = _("wpimath.units.cubic_fathoms");
};

template <> struct handle_type_name<units::cubic_fathoms> {
  static constexpr auto name = _("wpimath.units.cubic_fathoms");
};

template <> struct handle_type_name<units::tablespoon_t> {
  static constexpr auto name = _("wpimath.units.tablespoons");
};

template <> struct handle_type_name<units::tablespoons> {
  static constexpr auto name = _("wpimath.units.tablespoons");
};

template <> struct handle_type_name<units::teaspoon_t> {
  static constexpr auto name = _("wpimath.units.teaspoons");
};

template <> struct handle_type_name<units::teaspoons> {
  static constexpr auto name = _("wpimath.units.teaspoons");
};

template <> struct handle_type_name<units::pinch_t> {
  static constexpr auto name = _("wpimath.units.pinches");
};

template <> struct handle_type_name<units::pinches> {
  static constexpr auto name = _("wpimath.units.pinches");
};

template <> struct handle_type_name<units::dash_t> {
  static constexpr auto name = _("wpimath.units.dashes");
};

template <> struct handle_type_name<units::dashes> {
  static constexpr auto name = _("wpimath.units.dashes");
};

template <> struct handle_type_name<units::drop_t> {
  static constexpr auto name = _("wpimath.units.drops");
};

template <> struct handle_type_name<units::drops> {
  static constexpr auto name = _("wpimath.units.drops");
};

template <> struct handle_type_name<units::fifth_t> {
  static constexpr auto name = _("wpimath.units.fifths");
};

template <> struct handle_type_name<units::fifths> {
  static constexpr auto name = _("wpimath.units.fifths");
};

template <> struct handle_type_name<units::dram_t> {
  static constexpr auto name = _("wpimath.units.drams");
};

template <> struct handle_type_name<units::drams> {
  static constexpr auto name = _("wpimath.units.drams");
};

template <> struct handle_type_name<units::gill_t> {
  static constexpr auto name = _("wpimath.units.gills");
};

template <> struct handle_type_name<units::gills> {
  static constexpr auto name = _("wpimath.units.gills");
};

template <> struct handle_type_name<units::peck_t> {
  static constexpr auto name = _("wpimath.units.pecks");
};

template <> struct handle_type_name<units::pecks> {
  static constexpr auto name = _("wpimath.units.pecks");
};

template <> struct handle_type_name<units::sack_t> {
  static constexpr auto name = _("wpimath.units.sacks");
};

template <> struct handle_type_name<units::sacks> {
  static constexpr auto name = _("wpimath.units.sacks");
};

template <> struct handle_type_name<units::shot_t> {
  static constexpr auto name = _("wpimath.units.shots");
};

template <> struct handle_type_name<units::shots> {
  static constexpr auto name = _("wpimath.units.shots");
};

template <> struct handle_type_name<units::strike_t> {
  static constexpr auto name = _("wpimath.units.strikes");
};

template <> struct handle_type_name<units::strikes> {
  static constexpr auto name = _("wpimath.units.strikes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
