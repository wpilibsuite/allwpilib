#pragma once

#include <units/illuminance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::lux_t> {
  static constexpr auto name = _("wpimath.units.luxes");
};

template <> struct handle_type_name<units::luxes> {
  static constexpr auto name = _("wpimath.units.luxes");
};

template <> struct handle_type_name<units::nanolux_t> {
  static constexpr auto name = _("wpimath.units.nanoluxes");
};

template <> struct handle_type_name<units::nanoluxes> {
  static constexpr auto name = _("wpimath.units.nanoluxes");
};

template <> struct handle_type_name<units::microlux_t> {
  static constexpr auto name = _("wpimath.units.microluxes");
};

template <> struct handle_type_name<units::microluxes> {
  static constexpr auto name = _("wpimath.units.microluxes");
};

template <> struct handle_type_name<units::millilux_t> {
  static constexpr auto name = _("wpimath.units.milliluxes");
};

template <> struct handle_type_name<units::milliluxes> {
  static constexpr auto name = _("wpimath.units.milliluxes");
};

template <> struct handle_type_name<units::kilolux_t> {
  static constexpr auto name = _("wpimath.units.kiloluxes");
};

template <> struct handle_type_name<units::kiloluxes> {
  static constexpr auto name = _("wpimath.units.kiloluxes");
};

template <> struct handle_type_name<units::footcandle_t> {
  static constexpr auto name = _("wpimath.units.footcandles");
};

template <> struct handle_type_name<units::footcandles> {
  static constexpr auto name = _("wpimath.units.footcandles");
};

template <> struct handle_type_name<units::lumens_per_square_inch_t> {
  static constexpr auto name = _("wpimath.units.lumens_per_square_inch");
};

template <> struct handle_type_name<units::lumens_per_square_inch> {
  static constexpr auto name = _("wpimath.units.lumens_per_square_inch");
};

template <> struct handle_type_name<units::phot_t> {
  static constexpr auto name = _("wpimath.units.phots");
};

template <> struct handle_type_name<units::phots> {
  static constexpr auto name = _("wpimath.units.phots");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
