#pragma once

#include <wpi/units/current.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::amperes<>> {
  static constexpr auto name = _("wpimath.units.amperes");
};

template <> struct handle_type_name<wpi::units::amperes_> {
  static constexpr auto name = _("wpimath.units.amperes");
};

template <> struct handle_type_name<wpi::units::nanoamperes<>> {
  static constexpr auto name = _("wpimath.units.nanoamperes");
};

template <> struct handle_type_name<wpi::units::nanoamperes_> {
  static constexpr auto name = _("wpimath.units.nanoamperes");
};

template <> struct handle_type_name<wpi::units::microamperes<>> {
  static constexpr auto name = _("wpimath.units.microamperes");
};

template <> struct handle_type_name<wpi::units::microamperes_> {
  static constexpr auto name = _("wpimath.units.microamperes");
};

template <> struct handle_type_name<wpi::units::milliamperes<>> {
  static constexpr auto name = _("wpimath.units.milliamperes");
};

template <> struct handle_type_name<wpi::units::milliamperes_> {
  static constexpr auto name = _("wpimath.units.milliamperes");
};

template <> struct handle_type_name<wpi::units::kiloamperes<>> {
  static constexpr auto name = _("wpimath.units.kiloamperes");
};

template <> struct handle_type_name<wpi::units::kiloamperes_> {
  static constexpr auto name = _("wpimath.units.kiloamperes");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
