#pragma once

#include <wpi/units/impedance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::ohms<>> {
  static constexpr auto name = _("wpimath.units.ohms");
};

template <> struct handle_type_name<wpi::units::ohms_> {
  static constexpr auto name = _("wpimath.units.ohms");
};

template <> struct handle_type_name<wpi::units::nanoohms<>> {
  static constexpr auto name = _("wpimath.units.nanoohms");
};

template <> struct handle_type_name<wpi::units::nanoohms_> {
  static constexpr auto name = _("wpimath.units.nanoohms");
};

template <> struct handle_type_name<wpi::units::microohms<>> {
  static constexpr auto name = _("wpimath.units.microohms");
};

template <> struct handle_type_name<wpi::units::microohms_> {
  static constexpr auto name = _("wpimath.units.microohms");
};

template <> struct handle_type_name<wpi::units::milliohms<>> {
  static constexpr auto name = _("wpimath.units.milliohms");
};

template <> struct handle_type_name<wpi::units::milliohms_> {
  static constexpr auto name = _("wpimath.units.milliohms");
};

template <> struct handle_type_name<wpi::units::kiloohms<>> {
  static constexpr auto name = _("wpimath.units.kiloohms");
};

template <> struct handle_type_name<wpi::units::kiloohms_> {
  static constexpr auto name = _("wpimath.units.kiloohms");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
