#pragma once

#include <wpi/units/power.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::watts<>> {
  static constexpr auto name = _("wpimath.units.watts");
};

template <> struct handle_type_name<wpi::units::watts_> {
  static constexpr auto name = _("wpimath.units.watts");
};

template <> struct handle_type_name<wpi::units::nanowatts<>> {
  static constexpr auto name = _("wpimath.units.nanowatts");
};

template <> struct handle_type_name<wpi::units::nanowatts_> {
  static constexpr auto name = _("wpimath.units.nanowatts");
};

template <> struct handle_type_name<wpi::units::microwatts<>> {
  static constexpr auto name = _("wpimath.units.microwatts");
};

template <> struct handle_type_name<wpi::units::microwatts_> {
  static constexpr auto name = _("wpimath.units.microwatts");
};

template <> struct handle_type_name<wpi::units::milliwatts<>> {
  static constexpr auto name = _("wpimath.units.milliwatts");
};

template <> struct handle_type_name<wpi::units::milliwatts_> {
  static constexpr auto name = _("wpimath.units.milliwatts");
};

template <> struct handle_type_name<wpi::units::kilowatts<>> {
  static constexpr auto name = _("wpimath.units.kilowatts");
};

template <> struct handle_type_name<wpi::units::kilowatts_> {
  static constexpr auto name = _("wpimath.units.kilowatts");
};

template <> struct handle_type_name<wpi::units::horsepower<>> {
  static constexpr auto name = _("wpimath.units.horsepower");
};

template <> struct handle_type_name<wpi::units::horsepower_> {
  static constexpr auto name = _("wpimath.units.horsepower");
};

template <> struct handle_type_name<wpi::units::metric_horsepower<>> {
  static constexpr auto name = _("wpimath.units.metric_horsepower");
};

template <> struct handle_type_name<wpi::units::metric_horsepower_> {
  static constexpr auto name = _("wpimath.units.metric_horsepower");
};

template <> struct handle_type_name<wpi::units::electrical_horsepower<>> {
  static constexpr auto name = _("wpimath.units.electrical_horsepower");
};

template <> struct handle_type_name<wpi::units::electrical_horsepower_> {
  static constexpr auto name = _("wpimath.units.electrical_horsepower");
};

template <> struct handle_type_name<wpi::units::tons_of_refrigeration<>> {
  static constexpr auto name = _("wpimath.units.tons_of_refrigeration");
};

template <> struct handle_type_name<wpi::units::tons_of_refrigeration_> {
  static constexpr auto name = _("wpimath.units.tons_of_refrigeration");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
