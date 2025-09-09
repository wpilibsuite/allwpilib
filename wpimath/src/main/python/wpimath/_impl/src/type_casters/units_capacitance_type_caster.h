#pragma once

#include <units/capacitance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::farad_t> {
  static constexpr auto name = _("wpimath.units.farads");
};

template <> struct handle_type_name<units::farads> {
  static constexpr auto name = _("wpimath.units.farads");
};

template <> struct handle_type_name<units::nanofarad_t> {
  static constexpr auto name = _("wpimath.units.nanofarads");
};

template <> struct handle_type_name<units::nanofarads> {
  static constexpr auto name = _("wpimath.units.nanofarads");
};

template <> struct handle_type_name<units::microfarad_t> {
  static constexpr auto name = _("wpimath.units.microfarads");
};

template <> struct handle_type_name<units::microfarads> {
  static constexpr auto name = _("wpimath.units.microfarads");
};

template <> struct handle_type_name<units::millifarad_t> {
  static constexpr auto name = _("wpimath.units.millifarads");
};

template <> struct handle_type_name<units::millifarads> {
  static constexpr auto name = _("wpimath.units.millifarads");
};

template <> struct handle_type_name<units::kilofarad_t> {
  static constexpr auto name = _("wpimath.units.kilofarads");
};

template <> struct handle_type_name<units::kilofarads> {
  static constexpr auto name = _("wpimath.units.kilofarads");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
