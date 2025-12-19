#pragma once

#include <wpi/units/capacitance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::farads<>> {
  static constexpr auto name = _("wpimath.units.farads");
};

template <> struct handle_type_name<wpi::units::farads_> {
  static constexpr auto name = _("wpimath.units.farads");
};

template <> struct handle_type_name<wpi::units::nanofarads<>> {
  static constexpr auto name = _("wpimath.units.nanofarads");
};

template <> struct handle_type_name<wpi::units::nanofarads_> {
  static constexpr auto name = _("wpimath.units.nanofarads");
};

template <> struct handle_type_name<wpi::units::microfarads<>> {
  static constexpr auto name = _("wpimath.units.microfarads");
};

template <> struct handle_type_name<wpi::units::microfarads_> {
  static constexpr auto name = _("wpimath.units.microfarads");
};

template <> struct handle_type_name<wpi::units::millifarads<>> {
  static constexpr auto name = _("wpimath.units.millifarads");
};

template <> struct handle_type_name<wpi::units::millifarads_> {
  static constexpr auto name = _("wpimath.units.millifarads");
};

template <> struct handle_type_name<wpi::units::kilofarads<>> {
  static constexpr auto name = _("wpimath.units.kilofarads");
};

template <> struct handle_type_name<wpi::units::kilofarads_> {
  static constexpr auto name = _("wpimath.units.kilofarads");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
