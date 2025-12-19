#pragma once

#include <wpi/units/length.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::meters<>> {
  static constexpr auto name = _("wpimath.units.meters");
};

template <> struct handle_type_name<wpi::units::meters_> {
  static constexpr auto name = _("wpimath.units.meters");
};

template <> struct handle_type_name<wpi::units::nanometers<>> {
  static constexpr auto name = _("wpimath.units.nanometers");
};

template <> struct handle_type_name<wpi::units::nanometers_> {
  static constexpr auto name = _("wpimath.units.nanometers");
};

template <> struct handle_type_name<wpi::units::micrometers<>> {
  static constexpr auto name = _("wpimath.units.micrometers");
};

template <> struct handle_type_name<wpi::units::micrometers_> {
  static constexpr auto name = _("wpimath.units.micrometers");
};

template <> struct handle_type_name<wpi::units::millimeters<>> {
  static constexpr auto name = _("wpimath.units.millimeters");
};

template <> struct handle_type_name<wpi::units::millimeters_> {
  static constexpr auto name = _("wpimath.units.millimeters");
};

template <> struct handle_type_name<wpi::units::centimeters<>> {
  static constexpr auto name = _("wpimath.units.centimeters");
};

template <> struct handle_type_name<wpi::units::centimeters_> {
  static constexpr auto name = _("wpimath.units.centimeters");
};

template <> struct handle_type_name<wpi::units::kilometers<>> {
  static constexpr auto name = _("wpimath.units.kilometers");
};

template <> struct handle_type_name<wpi::units::kilometers_> {
  static constexpr auto name = _("wpimath.units.kilometers");
};

template <> struct handle_type_name<wpi::units::feet<>> {
  static constexpr auto name = _("wpimath.units.feet");
};

template <> struct handle_type_name<wpi::units::feet_> {
  static constexpr auto name = _("wpimath.units.feet");
};

template <> struct handle_type_name<wpi::units::inches<>> {
  static constexpr auto name = _("wpimath.units.inches");
};

template <> struct handle_type_name<wpi::units::inches_> {
  static constexpr auto name = _("wpimath.units.inches");
};

template <> struct handle_type_name<wpi::units::mils<>> {
  static constexpr auto name = _("wpimath.units.mils");
};

template <> struct handle_type_name<wpi::units::mils_> {
  static constexpr auto name = _("wpimath.units.mils");
};

template <> struct handle_type_name<wpi::units::miles<>> {
  static constexpr auto name = _("wpimath.units.miles");
};

template <> struct handle_type_name<wpi::units::miles_> {
  static constexpr auto name = _("wpimath.units.miles");
};

template <> struct handle_type_name<wpi::units::nautical_miles<>> {
  static constexpr auto name = _("wpimath.units.nautical_miles");
};

template <> struct handle_type_name<wpi::units::nautical_miles_> {
  static constexpr auto name = _("wpimath.units.nautical_miles");
};

template <> struct handle_type_name<wpi::units::astronomical_units<>> {
  static constexpr auto name = _("wpimath.units.astronomical_units");
};

template <> struct handle_type_name<wpi::units::astronomical_units_> {
  static constexpr auto name = _("wpimath.units.astronomical_units");
};

template <> struct handle_type_name<wpi::units::lightyears<>> {
  static constexpr auto name = _("wpimath.units.lightyears");
};

template <> struct handle_type_name<wpi::units::lightyears_> {
  static constexpr auto name = _("wpimath.units.lightyears");
};

template <> struct handle_type_name<wpi::units::parsecs<>> {
  static constexpr auto name = _("wpimath.units.parsecs");
};

template <> struct handle_type_name<wpi::units::parsecs_> {
  static constexpr auto name = _("wpimath.units.parsecs");
};

template <> struct handle_type_name<wpi::units::angstroms<>> {
  static constexpr auto name = _("wpimath.units.angstroms");
};

template <> struct handle_type_name<wpi::units::angstroms_> {
  static constexpr auto name = _("wpimath.units.angstroms");
};

template <> struct handle_type_name<wpi::units::cubits<>> {
  static constexpr auto name = _("wpimath.units.cubits");
};

template <> struct handle_type_name<wpi::units::cubits_> {
  static constexpr auto name = _("wpimath.units.cubits");
};

template <> struct handle_type_name<wpi::units::fathoms<>> {
  static constexpr auto name = _("wpimath.units.fathoms");
};

template <> struct handle_type_name<wpi::units::fathoms_> {
  static constexpr auto name = _("wpimath.units.fathoms");
};

template <> struct handle_type_name<wpi::units::chains<>> {
  static constexpr auto name = _("wpimath.units.chains");
};

template <> struct handle_type_name<wpi::units::chains_> {
  static constexpr auto name = _("wpimath.units.chains");
};

template <> struct handle_type_name<wpi::units::furlongs<>> {
  static constexpr auto name = _("wpimath.units.furlongs");
};

template <> struct handle_type_name<wpi::units::furlongs_> {
  static constexpr auto name = _("wpimath.units.furlongs");
};

template <> struct handle_type_name<wpi::units::hands<>> {
  static constexpr auto name = _("wpimath.units.hands");
};

template <> struct handle_type_name<wpi::units::hands_> {
  static constexpr auto name = _("wpimath.units.hands");
};

template <> struct handle_type_name<wpi::units::leagues<>> {
  static constexpr auto name = _("wpimath.units.leagues");
};

template <> struct handle_type_name<wpi::units::leagues_> {
  static constexpr auto name = _("wpimath.units.leagues");
};

template <> struct handle_type_name<wpi::units::nautical_leagues<>> {
  static constexpr auto name = _("wpimath.units.nautical_leagues");
};

template <> struct handle_type_name<wpi::units::nautical_leagues_> {
  static constexpr auto name = _("wpimath.units.nautical_leagues");
};

template <> struct handle_type_name<wpi::units::yards<>> {
  static constexpr auto name = _("wpimath.units.yards");
};

template <> struct handle_type_name<wpi::units::yards_> {
  static constexpr auto name = _("wpimath.units.yards");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
