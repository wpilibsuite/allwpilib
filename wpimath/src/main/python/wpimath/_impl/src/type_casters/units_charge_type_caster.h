#pragma once

#include "wpi/units/charge.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::coulomb_t> {
  static constexpr auto name = _("wpimath.units.coulombs");
};

template <> struct handle_type_name<wpi::units::coulombs> {
  static constexpr auto name = _("wpimath.units.coulombs");
};

template <> struct handle_type_name<wpi::units::nanocoulomb_t> {
  static constexpr auto name = _("wpimath.units.nanocoulombs");
};

template <> struct handle_type_name<wpi::units::nanocoulombs> {
  static constexpr auto name = _("wpimath.units.nanocoulombs");
};

template <> struct handle_type_name<wpi::units::microcoulomb_t> {
  static constexpr auto name = _("wpimath.units.microcoulombs");
};

template <> struct handle_type_name<wpi::units::microcoulombs> {
  static constexpr auto name = _("wpimath.units.microcoulombs");
};

template <> struct handle_type_name<wpi::units::millicoulomb_t> {
  static constexpr auto name = _("wpimath.units.millicoulombs");
};

template <> struct handle_type_name<wpi::units::millicoulombs> {
  static constexpr auto name = _("wpimath.units.millicoulombs");
};

template <> struct handle_type_name<wpi::units::kilocoulomb_t> {
  static constexpr auto name = _("wpimath.units.kilocoulombs");
};

template <> struct handle_type_name<wpi::units::kilocoulombs> {
  static constexpr auto name = _("wpimath.units.kilocoulombs");
};

template <> struct handle_type_name<wpi::units::ampere_hour_t> {
  static constexpr auto name = _("wpimath.units.ampere_hours");
};

template <> struct handle_type_name<wpi::units::ampere_hours> {
  static constexpr auto name = _("wpimath.units.ampere_hours");
};

template <> struct handle_type_name<wpi::units::nanoampere_hour_t> {
  static constexpr auto name = _("wpimath.units.nanoampere_hours");
};

template <> struct handle_type_name<wpi::units::nanoampere_hours> {
  static constexpr auto name = _("wpimath.units.nanoampere_hours");
};

template <> struct handle_type_name<wpi::units::microampere_hour_t> {
  static constexpr auto name = _("wpimath.units.microampere_hours");
};

template <> struct handle_type_name<wpi::units::microampere_hours> {
  static constexpr auto name = _("wpimath.units.microampere_hours");
};

template <> struct handle_type_name<wpi::units::milliampere_hour_t> {
  static constexpr auto name = _("wpimath.units.milliampere_hours");
};

template <> struct handle_type_name<wpi::units::milliampere_hours> {
  static constexpr auto name = _("wpimath.units.milliampere_hours");
};

template <> struct handle_type_name<wpi::units::kiloampere_hour_t> {
  static constexpr auto name = _("wpimath.units.kiloampere_hours");
};

template <> struct handle_type_name<wpi::units::kiloampere_hours> {
  static constexpr auto name = _("wpimath.units.kiloampere_hours");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
