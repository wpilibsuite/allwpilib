#pragma once

#include <wpi/units/frequency.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::hertz<>> {
  static constexpr auto name = _("wpimath.units.hertz");
};

template <> struct handle_type_name<wpi::units::hertz_> {
  static constexpr auto name = _("wpimath.units.hertz");
};

template <> struct handle_type_name<wpi::units::nanohertz<>> {
  static constexpr auto name = _("wpimath.units.nanohertz");
};

template <> struct handle_type_name<wpi::units::nanohertz_> {
  static constexpr auto name = _("wpimath.units.nanohertz");
};

template <> struct handle_type_name<wpi::units::microhertz<>> {
  static constexpr auto name = _("wpimath.units.microhertz");
};

template <> struct handle_type_name<wpi::units::microhertz_> {
  static constexpr auto name = _("wpimath.units.microhertz");
};

template <> struct handle_type_name<wpi::units::millihertz<>> {
  static constexpr auto name = _("wpimath.units.millihertz");
};

template <> struct handle_type_name<wpi::units::millihertz_> {
  static constexpr auto name = _("wpimath.units.millihertz");
};

template <> struct handle_type_name<wpi::units::kilohertz<>> {
  static constexpr auto name = _("wpimath.units.kilohertz");
};

template <> struct handle_type_name<wpi::units::kilohertz_> {
  static constexpr auto name = _("wpimath.units.kilohertz");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
