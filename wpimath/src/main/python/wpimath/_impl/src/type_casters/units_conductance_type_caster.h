#pragma once

#include "wpi/units/conductance.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::siemens_t> {
  static constexpr auto name = _("wpimath.units.siemens");
};

template <> struct handle_type_name<wpi::units::siemens> {
  static constexpr auto name = _("wpimath.units.siemens");
};

template <> struct handle_type_name<wpi::units::nanosiemens_t> {
  static constexpr auto name = _("wpimath.units.nanosiemens");
};

template <> struct handle_type_name<wpi::units::nanosiemens> {
  static constexpr auto name = _("wpimath.units.nanosiemens");
};

template <> struct handle_type_name<wpi::units::microsiemens_t> {
  static constexpr auto name = _("wpimath.units.microsiemens");
};

template <> struct handle_type_name<wpi::units::microsiemens> {
  static constexpr auto name = _("wpimath.units.microsiemens");
};

template <> struct handle_type_name<wpi::units::millisiemens_t> {
  static constexpr auto name = _("wpimath.units.millisiemens");
};

template <> struct handle_type_name<wpi::units::millisiemens> {
  static constexpr auto name = _("wpimath.units.millisiemens");
};

template <> struct handle_type_name<wpi::units::kilosiemens_t> {
  static constexpr auto name = _("wpimath.units.kilosiemens");
};

template <> struct handle_type_name<wpi::units::kilosiemens> {
  static constexpr auto name = _("wpimath.units.kilosiemens");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
