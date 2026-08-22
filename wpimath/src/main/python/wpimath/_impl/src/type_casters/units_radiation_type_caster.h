#pragma once

#include <wpi/units/radiation.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::becquerels<>> {
  static constexpr auto name = _("wpimath.units.becquerels");
};

template <> struct handle_type_name<wpi::units::becquerels_> {
  static constexpr auto name = _("wpimath.units.becquerels");
};

template <> struct handle_type_name<wpi::units::nanobecquerels<>> {
  static constexpr auto name = _("wpimath.units.nanobecquerels");
};

template <> struct handle_type_name<wpi::units::nanobecquerels_> {
  static constexpr auto name = _("wpimath.units.nanobecquerels");
};

template <> struct handle_type_name<wpi::units::microbecquerels<>> {
  static constexpr auto name = _("wpimath.units.microbecquerels");
};

template <> struct handle_type_name<wpi::units::microbecquerels_> {
  static constexpr auto name = _("wpimath.units.microbecquerels");
};

template <> struct handle_type_name<wpi::units::millibecquerels<>> {
  static constexpr auto name = _("wpimath.units.millibecquerels");
};

template <> struct handle_type_name<wpi::units::millibecquerels_> {
  static constexpr auto name = _("wpimath.units.millibecquerels");
};

template <> struct handle_type_name<wpi::units::kilobecquerels<>> {
  static constexpr auto name = _("wpimath.units.kilobecquerels");
};

template <> struct handle_type_name<wpi::units::kilobecquerels_> {
  static constexpr auto name = _("wpimath.units.kilobecquerels");
};

template <> struct handle_type_name<wpi::units::grays<>> {
  static constexpr auto name = _("wpimath.units.grays");
};

template <> struct handle_type_name<wpi::units::grays_> {
  static constexpr auto name = _("wpimath.units.grays");
};

template <> struct handle_type_name<wpi::units::nanograys<>> {
  static constexpr auto name = _("wpimath.units.nanograys");
};

template <> struct handle_type_name<wpi::units::nanograys_> {
  static constexpr auto name = _("wpimath.units.nanograys");
};

template <> struct handle_type_name<wpi::units::micrograys<>> {
  static constexpr auto name = _("wpimath.units.micrograys");
};

template <> struct handle_type_name<wpi::units::micrograys_> {
  static constexpr auto name = _("wpimath.units.micrograys");
};

template <> struct handle_type_name<wpi::units::milligrays<>> {
  static constexpr auto name = _("wpimath.units.milligrays");
};

template <> struct handle_type_name<wpi::units::milligrays_> {
  static constexpr auto name = _("wpimath.units.milligrays");
};

template <> struct handle_type_name<wpi::units::kilograys<>> {
  static constexpr auto name = _("wpimath.units.kilograys");
};

template <> struct handle_type_name<wpi::units::kilograys_> {
  static constexpr auto name = _("wpimath.units.kilograys");
};

template <> struct handle_type_name<wpi::units::sieverts<>> {
  static constexpr auto name = _("wpimath.units.sieverts");
};

template <> struct handle_type_name<wpi::units::sieverts_> {
  static constexpr auto name = _("wpimath.units.sieverts");
};

template <> struct handle_type_name<wpi::units::nanosieverts<>> {
  static constexpr auto name = _("wpimath.units.nanosieverts");
};

template <> struct handle_type_name<wpi::units::nanosieverts_> {
  static constexpr auto name = _("wpimath.units.nanosieverts");
};

template <> struct handle_type_name<wpi::units::microsieverts<>> {
  static constexpr auto name = _("wpimath.units.microsieverts");
};

template <> struct handle_type_name<wpi::units::microsieverts_> {
  static constexpr auto name = _("wpimath.units.microsieverts");
};

template <> struct handle_type_name<wpi::units::millisieverts<>> {
  static constexpr auto name = _("wpimath.units.millisieverts");
};

template <> struct handle_type_name<wpi::units::millisieverts_> {
  static constexpr auto name = _("wpimath.units.millisieverts");
};

template <> struct handle_type_name<wpi::units::kilosieverts<>> {
  static constexpr auto name = _("wpimath.units.kilosieverts");
};

template <> struct handle_type_name<wpi::units::kilosieverts_> {
  static constexpr auto name = _("wpimath.units.kilosieverts");
};

template <> struct handle_type_name<wpi::units::curies<>> {
  static constexpr auto name = _("wpimath.units.curies");
};

template <> struct handle_type_name<wpi::units::curies_> {
  static constexpr auto name = _("wpimath.units.curies");
};

template <> struct handle_type_name<wpi::units::rutherfords<>> {
  static constexpr auto name = _("wpimath.units.rutherfords");
};

template <> struct handle_type_name<wpi::units::rutherfords_> {
  static constexpr auto name = _("wpimath.units.rutherfords");
};

template <> struct handle_type_name<wpi::units::radiation_absorbed_dose<>> {
  static constexpr auto name = _("wpimath.units.radiation_absorbed_dose");
};

template <> struct handle_type_name<wpi::units::radiation_absorbed_dose_> {
  static constexpr auto name = _("wpimath.units.radiation_absorbed_dose");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
