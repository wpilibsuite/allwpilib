#pragma once

#include "wpi/units/time.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::second_t> {
  static constexpr auto name = _("wpimath.units.seconds");
};

template <> struct handle_type_name<wpi::units::seconds> {
  static constexpr auto name = _("wpimath.units.seconds");
};

template <> struct handle_type_name<wpi::units::nanosecond_t> {
  static constexpr auto name = _("wpimath.units.nanoseconds");
};

template <> struct handle_type_name<wpi::units::nanoseconds> {
  static constexpr auto name = _("wpimath.units.nanoseconds");
};

template <> struct handle_type_name<wpi::units::microsecond_t> {
  static constexpr auto name = _("wpimath.units.microseconds");
};

template <> struct handle_type_name<wpi::units::microseconds> {
  static constexpr auto name = _("wpimath.units.microseconds");
};

template <> struct handle_type_name<wpi::units::millisecond_t> {
  static constexpr auto name = _("wpimath.units.milliseconds");
};

template <> struct handle_type_name<wpi::units::milliseconds> {
  static constexpr auto name = _("wpimath.units.milliseconds");
};

template <> struct handle_type_name<wpi::units::kilosecond_t> {
  static constexpr auto name = _("wpimath.units.kiloseconds");
};

template <> struct handle_type_name<wpi::units::kiloseconds> {
  static constexpr auto name = _("wpimath.units.kiloseconds");
};

template <> struct handle_type_name<wpi::units::minute_t> {
  static constexpr auto name = _("wpimath.units.minutes");
};

template <> struct handle_type_name<wpi::units::minutes> {
  static constexpr auto name = _("wpimath.units.minutes");
};

template <> struct handle_type_name<wpi::units::hour_t> {
  static constexpr auto name = _("wpimath.units.hours");
};

template <> struct handle_type_name<wpi::units::hours> {
  static constexpr auto name = _("wpimath.units.hours");
};

template <> struct handle_type_name<wpi::units::day_t> {
  static constexpr auto name = _("wpimath.units.days");
};

template <> struct handle_type_name<wpi::units::days> {
  static constexpr auto name = _("wpimath.units.days");
};

template <> struct handle_type_name<wpi::units::week_t> {
  static constexpr auto name = _("wpimath.units.weeks");
};

template <> struct handle_type_name<wpi::units::weeks> {
  static constexpr auto name = _("wpimath.units.weeks");
};

template <> struct handle_type_name<wpi::units::year_t> {
  static constexpr auto name = _("wpimath.units.years");
};

template <> struct handle_type_name<wpi::units::years> {
  static constexpr auto name = _("wpimath.units.years");
};

template <> struct handle_type_name<wpi::units::julian_year_t> {
  static constexpr auto name = _("wpimath.units.julian_years");
};

template <> struct handle_type_name<wpi::units::julian_years> {
  static constexpr auto name = _("wpimath.units.julian_years");
};

template <> struct handle_type_name<wpi::units::gregorian_year_t> {
  static constexpr auto name = _("wpimath.units.gregorian_years");
};

template <> struct handle_type_name<wpi::units::gregorian_years> {
  static constexpr auto name = _("wpimath.units.gregorian_years");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
