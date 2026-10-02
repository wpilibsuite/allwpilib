#pragma once

#include "wpi/units/length.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::meter_t> {
  static constexpr auto name = _("wpimath.units.meters");
};

template <> struct handle_type_name<wpi::units::meters> {
  static constexpr auto name = _("wpimath.units.meters");
};

template <> struct handle_type_name<wpi::units::nanometer_t> {
  static constexpr auto name = _("wpimath.units.nanometers");
};

template <> struct handle_type_name<wpi::units::nanometers> {
  static constexpr auto name = _("wpimath.units.nanometers");
};

template <> struct handle_type_name<wpi::units::micrometer_t> {
  static constexpr auto name = _("wpimath.units.micrometers");
};

template <> struct handle_type_name<wpi::units::micrometers> {
  static constexpr auto name = _("wpimath.units.micrometers");
};

template <> struct handle_type_name<wpi::units::millimeter_t> {
  static constexpr auto name = _("wpimath.units.millimeters");
};

template <> struct handle_type_name<wpi::units::millimeters> {
  static constexpr auto name = _("wpimath.units.millimeters");
};

template <> struct handle_type_name<wpi::units::centimeter_t> {
  static constexpr auto name = _("wpimath.units.centimeters");
};

template <> struct handle_type_name<wpi::units::centimeters> {
  static constexpr auto name = _("wpimath.units.centimeters");
};

template <> struct handle_type_name<wpi::units::kilometer_t> {
  static constexpr auto name = _("wpimath.units.kilometers");
};

template <> struct handle_type_name<wpi::units::kilometers> {
  static constexpr auto name = _("wpimath.units.kilometers");
};

template <> struct handle_type_name<wpi::units::foot_t> {
  static constexpr auto name = _("wpimath.units.feet");
};

template <> struct handle_type_name<wpi::units::feet> {
  static constexpr auto name = _("wpimath.units.feet");
};

template <> struct handle_type_name<wpi::units::mil_t> {
  static constexpr auto name = _("wpimath.units.mils");
};

template <> struct handle_type_name<wpi::units::mils> {
  static constexpr auto name = _("wpimath.units.mils");
};

template <> struct handle_type_name<wpi::units::inch_t> {
  static constexpr auto name = _("wpimath.units.inches");
};

template <> struct handle_type_name<wpi::units::inches> {
  static constexpr auto name = _("wpimath.units.inches");
};

template <> struct handle_type_name<wpi::units::mile_t> {
  static constexpr auto name = _("wpimath.units.miles");
};

template <> struct handle_type_name<wpi::units::miles> {
  static constexpr auto name = _("wpimath.units.miles");
};

template <> struct handle_type_name<wpi::units::nauticalMile_t> {
  static constexpr auto name = _("wpimath.units.nauticalMiles");
};

template <> struct handle_type_name<wpi::units::nauticalMiles> {
  static constexpr auto name = _("wpimath.units.nauticalMiles");
};

template <> struct handle_type_name<wpi::units::astronicalUnit_t> {
  static constexpr auto name = _("wpimath.units.astronicalUnits");
};

template <> struct handle_type_name<wpi::units::astronicalUnits> {
  static constexpr auto name = _("wpimath.units.astronicalUnits");
};

template <> struct handle_type_name<wpi::units::lightyear_t> {
  static constexpr auto name = _("wpimath.units.lightyears");
};

template <> struct handle_type_name<wpi::units::lightyears> {
  static constexpr auto name = _("wpimath.units.lightyears");
};

template <> struct handle_type_name<wpi::units::parsec_t> {
  static constexpr auto name = _("wpimath.units.parsecs");
};

template <> struct handle_type_name<wpi::units::parsecs> {
  static constexpr auto name = _("wpimath.units.parsecs");
};

template <> struct handle_type_name<wpi::units::angstrom_t> {
  static constexpr auto name = _("wpimath.units.angstroms");
};

template <> struct handle_type_name<wpi::units::angstroms> {
  static constexpr auto name = _("wpimath.units.angstroms");
};

template <> struct handle_type_name<wpi::units::cubit_t> {
  static constexpr auto name = _("wpimath.units.cubits");
};

template <> struct handle_type_name<wpi::units::cubits> {
  static constexpr auto name = _("wpimath.units.cubits");
};

template <> struct handle_type_name<wpi::units::fathom_t> {
  static constexpr auto name = _("wpimath.units.fathoms");
};

template <> struct handle_type_name<wpi::units::fathoms> {
  static constexpr auto name = _("wpimath.units.fathoms");
};

template <> struct handle_type_name<wpi::units::chain_t> {
  static constexpr auto name = _("wpimath.units.chains");
};

template <> struct handle_type_name<wpi::units::chains> {
  static constexpr auto name = _("wpimath.units.chains");
};

template <> struct handle_type_name<wpi::units::furlong_t> {
  static constexpr auto name = _("wpimath.units.furlongs");
};

template <> struct handle_type_name<wpi::units::furlongs> {
  static constexpr auto name = _("wpimath.units.furlongs");
};

template <> struct handle_type_name<wpi::units::hand_t> {
  static constexpr auto name = _("wpimath.units.hands");
};

template <> struct handle_type_name<wpi::units::hands> {
  static constexpr auto name = _("wpimath.units.hands");
};

template <> struct handle_type_name<wpi::units::league_t> {
  static constexpr auto name = _("wpimath.units.leagues");
};

template <> struct handle_type_name<wpi::units::leagues> {
  static constexpr auto name = _("wpimath.units.leagues");
};

template <> struct handle_type_name<wpi::units::nauticalLeague_t> {
  static constexpr auto name = _("wpimath.units.nauticalLeagues");
};

template <> struct handle_type_name<wpi::units::nauticalLeagues> {
  static constexpr auto name = _("wpimath.units.nauticalLeagues");
};

template <> struct handle_type_name<wpi::units::yard_t> {
  static constexpr auto name = _("wpimath.units.yards");
};

template <> struct handle_type_name<wpi::units::yards> {
  static constexpr auto name = _("wpimath.units.yards");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
