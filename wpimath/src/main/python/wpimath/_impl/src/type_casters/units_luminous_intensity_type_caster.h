#pragma once

#include "wpi/units/luminous_intensity.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::candela_t> {
  static constexpr auto name = _("wpimath.units.candelas");
};

template <> struct handle_type_name<wpi::units::candelas> {
  static constexpr auto name = _("wpimath.units.candelas");
};

template <> struct handle_type_name<wpi::units::nanocandela_t> {
  static constexpr auto name = _("wpimath.units.nanocandelas");
};

template <> struct handle_type_name<wpi::units::nanocandelas> {
  static constexpr auto name = _("wpimath.units.nanocandelas");
};

template <> struct handle_type_name<wpi::units::microcandela_t> {
  static constexpr auto name = _("wpimath.units.microcandelas");
};

template <> struct handle_type_name<wpi::units::microcandelas> {
  static constexpr auto name = _("wpimath.units.microcandelas");
};

template <> struct handle_type_name<wpi::units::millicandela_t> {
  static constexpr auto name = _("wpimath.units.millicandelas");
};

template <> struct handle_type_name<wpi::units::millicandelas> {
  static constexpr auto name = _("wpimath.units.millicandelas");
};

template <> struct handle_type_name<wpi::units::kilocandela_t> {
  static constexpr auto name = _("wpimath.units.kilocandelas");
};

template <> struct handle_type_name<wpi::units::kilocandelas> {
  static constexpr auto name = _("wpimath.units.kilocandelas");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
