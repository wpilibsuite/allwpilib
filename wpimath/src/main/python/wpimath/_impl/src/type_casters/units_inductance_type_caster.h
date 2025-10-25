#pragma once

#include <units/inductance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::henry_t> {
  static constexpr auto name = _("wpimath.units.henries");
};

template <> struct handle_type_name<units::henries> {
  static constexpr auto name = _("wpimath.units.henries");
};

template <> struct handle_type_name<units::nanohenry_t> {
  static constexpr auto name = _("wpimath.units.nanohenries");
};

template <> struct handle_type_name<units::nanohenries> {
  static constexpr auto name = _("wpimath.units.nanohenries");
};

template <> struct handle_type_name<units::microhenry_t> {
  static constexpr auto name = _("wpimath.units.microhenries");
};

template <> struct handle_type_name<units::microhenries> {
  static constexpr auto name = _("wpimath.units.microhenries");
};

template <> struct handle_type_name<units::millihenry_t> {
  static constexpr auto name = _("wpimath.units.millihenries");
};

template <> struct handle_type_name<units::millihenries> {
  static constexpr auto name = _("wpimath.units.millihenries");
};

template <> struct handle_type_name<units::kilohenry_t> {
  static constexpr auto name = _("wpimath.units.kilohenries");
};

template <> struct handle_type_name<units::kilohenries> {
  static constexpr auto name = _("wpimath.units.kilohenries");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
