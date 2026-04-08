#pragma once

#include "wpi/units/current.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::ampere_t> {
  static constexpr auto name = _("wpimath.units.amperes");
};

template <> struct handle_type_name<wpi::units::amperes> {
  static constexpr auto name = _("wpimath.units.amperes");
};

template <> struct handle_type_name<wpi::units::nanoampere_t> {
  static constexpr auto name = _("wpimath.units.nanoamperes");
};

template <> struct handle_type_name<wpi::units::nanoamperes> {
  static constexpr auto name = _("wpimath.units.nanoamperes");
};

template <> struct handle_type_name<wpi::units::microampere_t> {
  static constexpr auto name = _("wpimath.units.microamperes");
};

template <> struct handle_type_name<wpi::units::microamperes> {
  static constexpr auto name = _("wpimath.units.microamperes");
};

template <> struct handle_type_name<wpi::units::milliampere_t> {
  static constexpr auto name = _("wpimath.units.milliamperes");
};

template <> struct handle_type_name<wpi::units::milliamperes> {
  static constexpr auto name = _("wpimath.units.milliamperes");
};

template <> struct handle_type_name<wpi::units::kiloampere_t> {
  static constexpr auto name = _("wpimath.units.kiloamperes");
};

template <> struct handle_type_name<wpi::units::kiloamperes> {
  static constexpr auto name = _("wpimath.units.kiloamperes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
