#pragma once

#include <units/length.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::meter_t> {
  static constexpr auto name = _("wpimath.units.meters");
};

template <> struct handle_type_name<units::meters> {
  static constexpr auto name = _("wpimath.units.meters");
};

template <> struct handle_type_name<units::nanometer_t> {
  static constexpr auto name = _("wpimath.units.nanometers");
};

template <> struct handle_type_name<units::nanometers> {
  static constexpr auto name = _("wpimath.units.nanometers");
};

template <> struct handle_type_name<units::micrometer_t> {
  static constexpr auto name = _("wpimath.units.micrometers");
};

template <> struct handle_type_name<units::micrometers> {
  static constexpr auto name = _("wpimath.units.micrometers");
};

template <> struct handle_type_name<units::millimeter_t> {
  static constexpr auto name = _("wpimath.units.millimeters");
};

template <> struct handle_type_name<units::millimeters> {
  static constexpr auto name = _("wpimath.units.millimeters");
};

template <> struct handle_type_name<units::centimeter_t> {
  static constexpr auto name = _("wpimath.units.centimeters");
};

template <> struct handle_type_name<units::centimeters> {
  static constexpr auto name = _("wpimath.units.centimeters");
};

template <> struct handle_type_name<units::kilometer_t> {
  static constexpr auto name = _("wpimath.units.kilometers");
};

template <> struct handle_type_name<units::kilometers> {
  static constexpr auto name = _("wpimath.units.kilometers");
};

template <> struct handle_type_name<units::foot_t> {
  static constexpr auto name = _("wpimath.units.feet");
};

template <> struct handle_type_name<units::feet> {
  static constexpr auto name = _("wpimath.units.feet");
};

template <> struct handle_type_name<units::mil_t> {
  static constexpr auto name = _("wpimath.units.mils");
};

template <> struct handle_type_name<units::mils> {
  static constexpr auto name = _("wpimath.units.mils");
};

template <> struct handle_type_name<units::inch_t> {
  static constexpr auto name = _("wpimath.units.inches");
};

template <> struct handle_type_name<units::inches> {
  static constexpr auto name = _("wpimath.units.inches");
};

template <> struct handle_type_name<units::mile_t> {
  static constexpr auto name = _("wpimath.units.miles");
};

template <> struct handle_type_name<units::miles> {
  static constexpr auto name = _("wpimath.units.miles");
};

template <> struct handle_type_name<units::nauticalMile_t> {
  static constexpr auto name = _("wpimath.units.nauticalMiles");
};

template <> struct handle_type_name<units::nauticalMiles> {
  static constexpr auto name = _("wpimath.units.nauticalMiles");
};

template <> struct handle_type_name<units::astronicalUnit_t> {
  static constexpr auto name = _("wpimath.units.astronicalUnits");
};

template <> struct handle_type_name<units::astronicalUnits> {
  static constexpr auto name = _("wpimath.units.astronicalUnits");
};

template <> struct handle_type_name<units::lightyear_t> {
  static constexpr auto name = _("wpimath.units.lightyears");
};

template <> struct handle_type_name<units::lightyears> {
  static constexpr auto name = _("wpimath.units.lightyears");
};

template <> struct handle_type_name<units::parsec_t> {
  static constexpr auto name = _("wpimath.units.parsecs");
};

template <> struct handle_type_name<units::parsecs> {
  static constexpr auto name = _("wpimath.units.parsecs");
};

template <> struct handle_type_name<units::angstrom_t> {
  static constexpr auto name = _("wpimath.units.angstroms");
};

template <> struct handle_type_name<units::angstroms> {
  static constexpr auto name = _("wpimath.units.angstroms");
};

template <> struct handle_type_name<units::cubit_t> {
  static constexpr auto name = _("wpimath.units.cubits");
};

template <> struct handle_type_name<units::cubits> {
  static constexpr auto name = _("wpimath.units.cubits");
};

template <> struct handle_type_name<units::fathom_t> {
  static constexpr auto name = _("wpimath.units.fathoms");
};

template <> struct handle_type_name<units::fathoms> {
  static constexpr auto name = _("wpimath.units.fathoms");
};

template <> struct handle_type_name<units::chain_t> {
  static constexpr auto name = _("wpimath.units.chains");
};

template <> struct handle_type_name<units::chains> {
  static constexpr auto name = _("wpimath.units.chains");
};

template <> struct handle_type_name<units::furlong_t> {
  static constexpr auto name = _("wpimath.units.furlongs");
};

template <> struct handle_type_name<units::furlongs> {
  static constexpr auto name = _("wpimath.units.furlongs");
};

template <> struct handle_type_name<units::hand_t> {
  static constexpr auto name = _("wpimath.units.hands");
};

template <> struct handle_type_name<units::hands> {
  static constexpr auto name = _("wpimath.units.hands");
};

template <> struct handle_type_name<units::league_t> {
  static constexpr auto name = _("wpimath.units.leagues");
};

template <> struct handle_type_name<units::leagues> {
  static constexpr auto name = _("wpimath.units.leagues");
};

template <> struct handle_type_name<units::nauticalLeague_t> {
  static constexpr auto name = _("wpimath.units.nauticalLeagues");
};

template <> struct handle_type_name<units::nauticalLeagues> {
  static constexpr auto name = _("wpimath.units.nauticalLeagues");
};

template <> struct handle_type_name<units::yard_t> {
  static constexpr auto name = _("wpimath.units.yards");
};

template <> struct handle_type_name<units::yards> {
  static constexpr auto name = _("wpimath.units.yards");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
