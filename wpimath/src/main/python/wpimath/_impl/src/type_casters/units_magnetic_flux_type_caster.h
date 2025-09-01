#pragma once

#include <units/magnetic_flux.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::weber_t> {
  static constexpr auto name = _("wpimath.units.webers");
};

template <> struct handle_type_name<units::webers> {
  static constexpr auto name = _("wpimath.units.webers");
};

template <> struct handle_type_name<units::nanoweber_t> {
  static constexpr auto name = _("wpimath.units.nanowebers");
};

template <> struct handle_type_name<units::nanowebers> {
  static constexpr auto name = _("wpimath.units.nanowebers");
};

template <> struct handle_type_name<units::microweber_t> {
  static constexpr auto name = _("wpimath.units.microwebers");
};

template <> struct handle_type_name<units::microwebers> {
  static constexpr auto name = _("wpimath.units.microwebers");
};

template <> struct handle_type_name<units::milliweber_t> {
  static constexpr auto name = _("wpimath.units.milliwebers");
};

template <> struct handle_type_name<units::milliwebers> {
  static constexpr auto name = _("wpimath.units.milliwebers");
};

template <> struct handle_type_name<units::kiloweber_t> {
  static constexpr auto name = _("wpimath.units.kilowebers");
};

template <> struct handle_type_name<units::kilowebers> {
  static constexpr auto name = _("wpimath.units.kilowebers");
};

template <> struct handle_type_name<units::maxwell_t> {
  static constexpr auto name = _("wpimath.units.maxwells");
};

template <> struct handle_type_name<units::maxwells> {
  static constexpr auto name = _("wpimath.units.maxwells");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
