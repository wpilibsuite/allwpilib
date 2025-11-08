#pragma once

#include "wpi/units/illuminance.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::lux_t> {
  static constexpr auto name = _("wpimath.units.luxes");
};

template <> struct handle_type_name<wpi::units::luxes> {
  static constexpr auto name = _("wpimath.units.luxes");
};

template <> struct handle_type_name<wpi::units::nanolux_t> {
  static constexpr auto name = _("wpimath.units.nanoluxes");
};

template <> struct handle_type_name<wpi::units::nanoluxes> {
  static constexpr auto name = _("wpimath.units.nanoluxes");
};

template <> struct handle_type_name<wpi::units::microlux_t> {
  static constexpr auto name = _("wpimath.units.microluxes");
};

template <> struct handle_type_name<wpi::units::microluxes> {
  static constexpr auto name = _("wpimath.units.microluxes");
};

template <> struct handle_type_name<wpi::units::millilux_t> {
  static constexpr auto name = _("wpimath.units.milliluxes");
};

template <> struct handle_type_name<wpi::units::milliluxes> {
  static constexpr auto name = _("wpimath.units.milliluxes");
};

template <> struct handle_type_name<wpi::units::kilolux_t> {
  static constexpr auto name = _("wpimath.units.kiloluxes");
};

template <> struct handle_type_name<wpi::units::kiloluxes> {
  static constexpr auto name = _("wpimath.units.kiloluxes");
};

template <> struct handle_type_name<wpi::units::footcandle_t> {
  static constexpr auto name = _("wpimath.units.footcandles");
};

template <> struct handle_type_name<wpi::units::footcandles> {
  static constexpr auto name = _("wpimath.units.footcandles");
};

template <> struct handle_type_name<wpi::units::lumens_per_square_inch_t> {
  static constexpr auto name = _("wpimath.units.lumens_per_square_inch");
};

template <> struct handle_type_name<wpi::units::lumens_per_square_inch> {
  static constexpr auto name = _("wpimath.units.lumens_per_square_inch");
};

template <> struct handle_type_name<wpi::units::phot_t> {
  static constexpr auto name = _("wpimath.units.phots");
};

template <> struct handle_type_name<wpi::units::phots> {
  static constexpr auto name = _("wpimath.units.phots");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
