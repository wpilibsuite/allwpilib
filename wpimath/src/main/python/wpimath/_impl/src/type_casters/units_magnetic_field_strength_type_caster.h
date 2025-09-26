#pragma once

#include <units/magnetic_field_strength.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::tesla_t> {
  static constexpr auto name = _("wpimath.units.teslas");
};

template <> struct handle_type_name<units::teslas> {
  static constexpr auto name = _("wpimath.units.teslas");
};

template <> struct handle_type_name<units::nanotesla_t> {
  static constexpr auto name = _("wpimath.units.nanoteslas");
};

template <> struct handle_type_name<units::nanoteslas> {
  static constexpr auto name = _("wpimath.units.nanoteslas");
};

template <> struct handle_type_name<units::microtesla_t> {
  static constexpr auto name = _("wpimath.units.microteslas");
};

template <> struct handle_type_name<units::microteslas> {
  static constexpr auto name = _("wpimath.units.microteslas");
};

template <> struct handle_type_name<units::millitesla_t> {
  static constexpr auto name = _("wpimath.units.milliteslas");
};

template <> struct handle_type_name<units::milliteslas> {
  static constexpr auto name = _("wpimath.units.milliteslas");
};

template <> struct handle_type_name<units::kilotesla_t> {
  static constexpr auto name = _("wpimath.units.kiloteslas");
};

template <> struct handle_type_name<units::kiloteslas> {
  static constexpr auto name = _("wpimath.units.kiloteslas");
};

template <> struct handle_type_name<units::gauss_t> {
  static constexpr auto name = _("wpimath.units.gauss");
};

template <> struct handle_type_name<units::gauss> {
  static constexpr auto name = _("wpimath.units.gauss");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
