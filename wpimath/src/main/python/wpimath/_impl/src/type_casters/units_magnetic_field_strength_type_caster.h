#pragma once

#include "wpi/units/magnetic_field_strength.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::tesla_t> {
  static constexpr auto name = _("wpimath.units.teslas");
};

template <> struct handle_type_name<wpi::units::teslas> {
  static constexpr auto name = _("wpimath.units.teslas");
};

template <> struct handle_type_name<wpi::units::nanotesla_t> {
  static constexpr auto name = _("wpimath.units.nanoteslas");
};

template <> struct handle_type_name<wpi::units::nanoteslas> {
  static constexpr auto name = _("wpimath.units.nanoteslas");
};

template <> struct handle_type_name<wpi::units::microtesla_t> {
  static constexpr auto name = _("wpimath.units.microteslas");
};

template <> struct handle_type_name<wpi::units::microteslas> {
  static constexpr auto name = _("wpimath.units.microteslas");
};

template <> struct handle_type_name<wpi::units::millitesla_t> {
  static constexpr auto name = _("wpimath.units.milliteslas");
};

template <> struct handle_type_name<wpi::units::milliteslas> {
  static constexpr auto name = _("wpimath.units.milliteslas");
};

template <> struct handle_type_name<wpi::units::kilotesla_t> {
  static constexpr auto name = _("wpimath.units.kiloteslas");
};

template <> struct handle_type_name<wpi::units::kiloteslas> {
  static constexpr auto name = _("wpimath.units.kiloteslas");
};

template <> struct handle_type_name<wpi::units::gauss_t> {
  static constexpr auto name = _("wpimath.units.gauss");
};

template <> struct handle_type_name<wpi::units::gauss> {
  static constexpr auto name = _("wpimath.units.gauss");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
