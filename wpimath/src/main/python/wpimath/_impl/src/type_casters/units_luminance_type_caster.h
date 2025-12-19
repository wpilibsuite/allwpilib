#pragma once

#include <wpi/units/luminance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::candelas_per_square_meter<>> {
  static constexpr auto name = _("wpimath.units.candelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::candelas_per_square_meter_> {
  static constexpr auto name = _("wpimath.units.candelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::nanocandelas_per_square_meter<>> {
  static constexpr auto name = _("wpimath.units.nanocandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::nanocandelas_per_square_meter_> {
  static constexpr auto name = _("wpimath.units.nanocandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::microcandelas_per_square_meter<>> {
  static constexpr auto name = _("wpimath.units.microcandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::microcandelas_per_square_meter_> {
  static constexpr auto name = _("wpimath.units.microcandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::millicandelas_per_square_meter<>> {
  static constexpr auto name = _("wpimath.units.millicandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::millicandelas_per_square_meter_> {
  static constexpr auto name = _("wpimath.units.millicandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::kilocandelas_per_square_meter<>> {
  static constexpr auto name = _("wpimath.units.kilocandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::kilocandelas_per_square_meter_> {
  static constexpr auto name = _("wpimath.units.kilocandelas_per_square_meter");
};

template <> struct handle_type_name<wpi::units::stilbs<>> {
  static constexpr auto name = _("wpimath.units.stilbs");
};

template <> struct handle_type_name<wpi::units::stilbs_> {
  static constexpr auto name = _("wpimath.units.stilbs");
};

template <> struct handle_type_name<wpi::units::apostilbs<>> {
  static constexpr auto name = _("wpimath.units.apostilbs");
};

template <> struct handle_type_name<wpi::units::apostilbs_> {
  static constexpr auto name = _("wpimath.units.apostilbs");
};

template <> struct handle_type_name<wpi::units::brils<>> {
  static constexpr auto name = _("wpimath.units.brils");
};

template <> struct handle_type_name<wpi::units::brils_> {
  static constexpr auto name = _("wpimath.units.brils");
};

template <> struct handle_type_name<wpi::units::skots<>> {
  static constexpr auto name = _("wpimath.units.skots");
};

template <> struct handle_type_name<wpi::units::skots_> {
  static constexpr auto name = _("wpimath.units.skots");
};

template <> struct handle_type_name<wpi::units::lamberts<>> {
  static constexpr auto name = _("wpimath.units.lamberts");
};

template <> struct handle_type_name<wpi::units::lamberts_> {
  static constexpr auto name = _("wpimath.units.lamberts");
};

template <> struct handle_type_name<wpi::units::millilamberts<>> {
  static constexpr auto name = _("wpimath.units.millilamberts");
};

template <> struct handle_type_name<wpi::units::millilamberts_> {
  static constexpr auto name = _("wpimath.units.millilamberts");
};

template <> struct handle_type_name<wpi::units::foot_lamberts<>> {
  static constexpr auto name = _("wpimath.units.foot_lamberts");
};

template <> struct handle_type_name<wpi::units::foot_lamberts_> {
  static constexpr auto name = _("wpimath.units.foot_lamberts");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
