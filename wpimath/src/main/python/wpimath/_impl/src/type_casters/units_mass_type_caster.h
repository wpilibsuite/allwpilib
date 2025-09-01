#pragma once

#include <units/mass.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::gram_t> {
  static constexpr auto name = _("wpimath.units.grams");
};

template <> struct handle_type_name<units::grams> {
  static constexpr auto name = _("wpimath.units.grams");
};

template <> struct handle_type_name<units::nanogram_t> {
  static constexpr auto name = _("wpimath.units.nanograms");
};

template <> struct handle_type_name<units::nanograms> {
  static constexpr auto name = _("wpimath.units.nanograms");
};

template <> struct handle_type_name<units::microgram_t> {
  static constexpr auto name = _("wpimath.units.micrograms");
};

template <> struct handle_type_name<units::micrograms> {
  static constexpr auto name = _("wpimath.units.micrograms");
};

template <> struct handle_type_name<units::milligram_t> {
  static constexpr auto name = _("wpimath.units.milligrams");
};

template <> struct handle_type_name<units::milligrams> {
  static constexpr auto name = _("wpimath.units.milligrams");
};

template <> struct handle_type_name<units::kilogram_t> {
  static constexpr auto name = _("wpimath.units.kilograms");
};

template <> struct handle_type_name<units::kilograms> {
  static constexpr auto name = _("wpimath.units.kilograms");
};

template <> struct handle_type_name<units::metric_ton_t> {
  static constexpr auto name = _("wpimath.units.metric_tons");
};

template <> struct handle_type_name<units::metric_tons> {
  static constexpr auto name = _("wpimath.units.metric_tons");
};

template <> struct handle_type_name<units::pound_t> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<units::pounds> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<units::long_ton_t> {
  static constexpr auto name = _("wpimath.units.long_tons");
};

template <> struct handle_type_name<units::long_tons> {
  static constexpr auto name = _("wpimath.units.long_tons");
};

template <> struct handle_type_name<units::short_ton_t> {
  static constexpr auto name = _("wpimath.units.short_tons");
};

template <> struct handle_type_name<units::short_tons> {
  static constexpr auto name = _("wpimath.units.short_tons");
};

template <> struct handle_type_name<units::stone_t> {
  static constexpr auto name = _("wpimath.units.stone");
};

template <> struct handle_type_name<units::stone> {
  static constexpr auto name = _("wpimath.units.stone");
};

template <> struct handle_type_name<units::ounce_t> {
  static constexpr auto name = _("wpimath.units.ounces");
};

template <> struct handle_type_name<units::ounces> {
  static constexpr auto name = _("wpimath.units.ounces");
};

template <> struct handle_type_name<units::carat_t> {
  static constexpr auto name = _("wpimath.units.carats");
};

template <> struct handle_type_name<units::carats> {
  static constexpr auto name = _("wpimath.units.carats");
};

template <> struct handle_type_name<units::slug_t> {
  static constexpr auto name = _("wpimath.units.slugs");
};

template <> struct handle_type_name<units::slugs> {
  static constexpr auto name = _("wpimath.units.slugs");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
