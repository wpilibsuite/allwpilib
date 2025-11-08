#pragma once

#include "wpi/units/impedance.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::ohm_t> {
  static constexpr auto name = _("wpimath.units.ohms");
};

template <> struct handle_type_name<wpi::units::ohms> {
  static constexpr auto name = _("wpimath.units.ohms");
};

template <> struct handle_type_name<wpi::units::nanoohm_t> {
  static constexpr auto name = _("wpimath.units.nanoohms");
};

template <> struct handle_type_name<wpi::units::nanoohms> {
  static constexpr auto name = _("wpimath.units.nanoohms");
};

template <> struct handle_type_name<wpi::units::microohm_t> {
  static constexpr auto name = _("wpimath.units.microohms");
};

template <> struct handle_type_name<wpi::units::microohms> {
  static constexpr auto name = _("wpimath.units.microohms");
};

template <> struct handle_type_name<wpi::units::milliohm_t> {
  static constexpr auto name = _("wpimath.units.milliohms");
};

template <> struct handle_type_name<wpi::units::milliohms> {
  static constexpr auto name = _("wpimath.units.milliohms");
};

template <> struct handle_type_name<wpi::units::kiloohm_t> {
  static constexpr auto name = _("wpimath.units.kiloohms");
};

template <> struct handle_type_name<wpi::units::kiloohms> {
  static constexpr auto name = _("wpimath.units.kiloohms");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
