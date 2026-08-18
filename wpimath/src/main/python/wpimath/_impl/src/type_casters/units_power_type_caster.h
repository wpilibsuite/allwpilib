#pragma once

#include "wpi/units/power.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::watt_t> {
  static constexpr auto name = _("wpimath.units.watts");
};

template <> struct handle_type_name<wpi::units::watts> {
  static constexpr auto name = _("wpimath.units.watts");
};

template <> struct handle_type_name<wpi::units::nanowatt_t> {
  static constexpr auto name = _("wpimath.units.nanowatts");
};

template <> struct handle_type_name<wpi::units::nanowatts> {
  static constexpr auto name = _("wpimath.units.nanowatts");
};

template <> struct handle_type_name<wpi::units::microwatt_t> {
  static constexpr auto name = _("wpimath.units.microwatts");
};

template <> struct handle_type_name<wpi::units::microwatts> {
  static constexpr auto name = _("wpimath.units.microwatts");
};

template <> struct handle_type_name<wpi::units::milliwatt_t> {
  static constexpr auto name = _("wpimath.units.milliwatts");
};

template <> struct handle_type_name<wpi::units::milliwatts> {
  static constexpr auto name = _("wpimath.units.milliwatts");
};

template <> struct handle_type_name<wpi::units::kilowatt_t> {
  static constexpr auto name = _("wpimath.units.kilowatts");
};

template <> struct handle_type_name<wpi::units::kilowatts> {
  static constexpr auto name = _("wpimath.units.kilowatts");
};

template <> struct handle_type_name<wpi::units::horsepower_t> {
  static constexpr auto name = _("wpimath.units.horsepower");
};

template <> struct handle_type_name<wpi::units::horsepower> {
  static constexpr auto name = _("wpimath.units.horsepower");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
