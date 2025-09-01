#pragma once

#include <units/time.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::second_t> {
  static constexpr auto name = _("wpimath.units.seconds");
};

template <> struct handle_type_name<units::seconds> {
  static constexpr auto name = _("wpimath.units.seconds");
};

template <> struct handle_type_name<units::nanosecond_t> {
  static constexpr auto name = _("wpimath.units.nanoseconds");
};

template <> struct handle_type_name<units::nanoseconds> {
  static constexpr auto name = _("wpimath.units.nanoseconds");
};

template <> struct handle_type_name<units::microsecond_t> {
  static constexpr auto name = _("wpimath.units.microseconds");
};

template <> struct handle_type_name<units::microseconds> {
  static constexpr auto name = _("wpimath.units.microseconds");
};

template <> struct handle_type_name<units::millisecond_t> {
  static constexpr auto name = _("wpimath.units.milliseconds");
};

template <> struct handle_type_name<units::milliseconds> {
  static constexpr auto name = _("wpimath.units.milliseconds");
};

template <> struct handle_type_name<units::kilosecond_t> {
  static constexpr auto name = _("wpimath.units.kiloseconds");
};

template <> struct handle_type_name<units::kiloseconds> {
  static constexpr auto name = _("wpimath.units.kiloseconds");
};

template <> struct handle_type_name<units::minute_t> {
  static constexpr auto name = _("wpimath.units.minutes");
};

template <> struct handle_type_name<units::minutes> {
  static constexpr auto name = _("wpimath.units.minutes");
};

template <> struct handle_type_name<units::hour_t> {
  static constexpr auto name = _("wpimath.units.hours");
};

template <> struct handle_type_name<units::hours> {
  static constexpr auto name = _("wpimath.units.hours");
};

template <> struct handle_type_name<units::day_t> {
  static constexpr auto name = _("wpimath.units.days");
};

template <> struct handle_type_name<units::days> {
  static constexpr auto name = _("wpimath.units.days");
};

template <> struct handle_type_name<units::week_t> {
  static constexpr auto name = _("wpimath.units.weeks");
};

template <> struct handle_type_name<units::weeks> {
  static constexpr auto name = _("wpimath.units.weeks");
};

template <> struct handle_type_name<units::year_t> {
  static constexpr auto name = _("wpimath.units.years");
};

template <> struct handle_type_name<units::years> {
  static constexpr auto name = _("wpimath.units.years");
};

template <> struct handle_type_name<units::julian_year_t> {
  static constexpr auto name = _("wpimath.units.julian_years");
};

template <> struct handle_type_name<units::julian_years> {
  static constexpr auto name = _("wpimath.units.julian_years");
};

template <> struct handle_type_name<units::gregorian_year_t> {
  static constexpr auto name = _("wpimath.units.gregorian_years");
};

template <> struct handle_type_name<units::gregorian_years> {
  static constexpr auto name = _("wpimath.units.gregorian_years");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
