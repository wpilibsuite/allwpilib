#pragma once

#include "wpi/units/mass.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::gram_t> {
  static constexpr auto name = _("wpimath.units.grams");
};

template <> struct handle_type_name<wpi::units::grams> {
  static constexpr auto name = _("wpimath.units.grams");
};

template <> struct handle_type_name<wpi::units::nanogram_t> {
  static constexpr auto name = _("wpimath.units.nanograms");
};

template <> struct handle_type_name<wpi::units::nanograms> {
  static constexpr auto name = _("wpimath.units.nanograms");
};

template <> struct handle_type_name<wpi::units::microgram_t> {
  static constexpr auto name = _("wpimath.units.micrograms");
};

template <> struct handle_type_name<wpi::units::micrograms> {
  static constexpr auto name = _("wpimath.units.micrograms");
};

template <> struct handle_type_name<wpi::units::milligram_t> {
  static constexpr auto name = _("wpimath.units.milligrams");
};

template <> struct handle_type_name<wpi::units::milligrams> {
  static constexpr auto name = _("wpimath.units.milligrams");
};

template <> struct handle_type_name<wpi::units::kilogram_t> {
  static constexpr auto name = _("wpimath.units.kilograms");
};

template <> struct handle_type_name<wpi::units::kilograms> {
  static constexpr auto name = _("wpimath.units.kilograms");
};

template <> struct handle_type_name<wpi::units::metric_ton_t> {
  static constexpr auto name = _("wpimath.units.metric_tons");
};

template <> struct handle_type_name<wpi::units::metric_tons> {
  static constexpr auto name = _("wpimath.units.metric_tons");
};

template <> struct handle_type_name<wpi::units::pound_t> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<wpi::units::pounds> {
  static constexpr auto name = _("wpimath.units.pounds");
};

template <> struct handle_type_name<wpi::units::long_ton_t> {
  static constexpr auto name = _("wpimath.units.long_tons");
};

template <> struct handle_type_name<wpi::units::long_tons> {
  static constexpr auto name = _("wpimath.units.long_tons");
};

template <> struct handle_type_name<wpi::units::short_ton_t> {
  static constexpr auto name = _("wpimath.units.short_tons");
};

template <> struct handle_type_name<wpi::units::short_tons> {
  static constexpr auto name = _("wpimath.units.short_tons");
};

template <> struct handle_type_name<wpi::units::stone_t> {
  static constexpr auto name = _("wpimath.units.stone");
};

template <> struct handle_type_name<wpi::units::stone> {
  static constexpr auto name = _("wpimath.units.stone");
};

template <> struct handle_type_name<wpi::units::ounce_t> {
  static constexpr auto name = _("wpimath.units.ounces");
};

template <> struct handle_type_name<wpi::units::ounces> {
  static constexpr auto name = _("wpimath.units.ounces");
};

template <> struct handle_type_name<wpi::units::carat_t> {
  static constexpr auto name = _("wpimath.units.carats");
};

template <> struct handle_type_name<wpi::units::carats> {
  static constexpr auto name = _("wpimath.units.carats");
};

template <> struct handle_type_name<wpi::units::slug_t> {
  static constexpr auto name = _("wpimath.units.slugs");
};

template <> struct handle_type_name<wpi::units::slugs> {
  static constexpr auto name = _("wpimath.units.slugs");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
