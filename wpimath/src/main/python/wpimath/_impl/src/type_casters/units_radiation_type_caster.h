#pragma once

#include "wpi/units/radiation.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::becquerel_t> {
  static constexpr auto name = _("wpimath.units.becquerels");
};

template <> struct handle_type_name<wpi::units::becquerels> {
  static constexpr auto name = _("wpimath.units.becquerels");
};

template <> struct handle_type_name<wpi::units::nanobecquerel_t> {
  static constexpr auto name = _("wpimath.units.nanobecquerels");
};

template <> struct handle_type_name<wpi::units::nanobecquerels> {
  static constexpr auto name = _("wpimath.units.nanobecquerels");
};

template <> struct handle_type_name<wpi::units::microbecquerel_t> {
  static constexpr auto name = _("wpimath.units.microbecquerels");
};

template <> struct handle_type_name<wpi::units::microbecquerels> {
  static constexpr auto name = _("wpimath.units.microbecquerels");
};

template <> struct handle_type_name<wpi::units::millibecquerel_t> {
  static constexpr auto name = _("wpimath.units.millibecquerels");
};

template <> struct handle_type_name<wpi::units::millibecquerels> {
  static constexpr auto name = _("wpimath.units.millibecquerels");
};

template <> struct handle_type_name<wpi::units::kilobecquerel_t> {
  static constexpr auto name = _("wpimath.units.kilobecquerels");
};

template <> struct handle_type_name<wpi::units::kilobecquerels> {
  static constexpr auto name = _("wpimath.units.kilobecquerels");
};

template <> struct handle_type_name<wpi::units::gray_t> {
  static constexpr auto name = _("wpimath.units.grays");
};

template <> struct handle_type_name<wpi::units::grays> {
  static constexpr auto name = _("wpimath.units.grays");
};

template <> struct handle_type_name<wpi::units::nanogray_t> {
  static constexpr auto name = _("wpimath.units.nanograys");
};

template <> struct handle_type_name<wpi::units::nanograys> {
  static constexpr auto name = _("wpimath.units.nanograys");
};

template <> struct handle_type_name<wpi::units::microgray_t> {
  static constexpr auto name = _("wpimath.units.micrograys");
};

template <> struct handle_type_name<wpi::units::micrograys> {
  static constexpr auto name = _("wpimath.units.micrograys");
};

template <> struct handle_type_name<wpi::units::milligray_t> {
  static constexpr auto name = _("wpimath.units.milligrays");
};

template <> struct handle_type_name<wpi::units::milligrays> {
  static constexpr auto name = _("wpimath.units.milligrays");
};

template <> struct handle_type_name<wpi::units::kilogray_t> {
  static constexpr auto name = _("wpimath.units.kilograys");
};

template <> struct handle_type_name<wpi::units::kilograys> {
  static constexpr auto name = _("wpimath.units.kilograys");
};

template <> struct handle_type_name<wpi::units::sievert_t> {
  static constexpr auto name = _("wpimath.units.sieverts");
};

template <> struct handle_type_name<wpi::units::sieverts> {
  static constexpr auto name = _("wpimath.units.sieverts");
};

template <> struct handle_type_name<wpi::units::nanosievert_t> {
  static constexpr auto name = _("wpimath.units.nanosieverts");
};

template <> struct handle_type_name<wpi::units::nanosieverts> {
  static constexpr auto name = _("wpimath.units.nanosieverts");
};

template <> struct handle_type_name<wpi::units::microsievert_t> {
  static constexpr auto name = _("wpimath.units.microsieverts");
};

template <> struct handle_type_name<wpi::units::microsieverts> {
  static constexpr auto name = _("wpimath.units.microsieverts");
};

template <> struct handle_type_name<wpi::units::millisievert_t> {
  static constexpr auto name = _("wpimath.units.millisieverts");
};

template <> struct handle_type_name<wpi::units::millisieverts> {
  static constexpr auto name = _("wpimath.units.millisieverts");
};

template <> struct handle_type_name<wpi::units::kilosievert_t> {
  static constexpr auto name = _("wpimath.units.kilosieverts");
};

template <> struct handle_type_name<wpi::units::kilosieverts> {
  static constexpr auto name = _("wpimath.units.kilosieverts");
};

template <> struct handle_type_name<wpi::units::curie_t> {
  static constexpr auto name = _("wpimath.units.curies");
};

template <> struct handle_type_name<wpi::units::curies> {
  static constexpr auto name = _("wpimath.units.curies");
};

template <> struct handle_type_name<wpi::units::rutherford_t> {
  static constexpr auto name = _("wpimath.units.rutherfords");
};

template <> struct handle_type_name<wpi::units::rutherfords> {
  static constexpr auto name = _("wpimath.units.rutherfords");
};

template <> struct handle_type_name<wpi::units::rad_t> {
  static constexpr auto name = _("wpimath.units.rads");
};

template <> struct handle_type_name<wpi::units::rads> {
  static constexpr auto name = _("wpimath.units.rads");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
