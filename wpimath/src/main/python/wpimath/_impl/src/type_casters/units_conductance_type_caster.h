#pragma once

#include <wpi/units/conductance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::siemens<>> {
  static constexpr auto name = _("wpimath.units.siemens");
};

template <> struct handle_type_name<wpi::units::siemens_> {
  static constexpr auto name = _("wpimath.units.siemens");
};

template <> struct handle_type_name<wpi::units::nanosiemens<>> {
  static constexpr auto name = _("wpimath.units.nanosiemens");
};

template <> struct handle_type_name<wpi::units::nanosiemens_> {
  static constexpr auto name = _("wpimath.units.nanosiemens");
};

template <> struct handle_type_name<wpi::units::microsiemens<>> {
  static constexpr auto name = _("wpimath.units.microsiemens");
};

template <> struct handle_type_name<wpi::units::microsiemens_> {
  static constexpr auto name = _("wpimath.units.microsiemens");
};

template <> struct handle_type_name<wpi::units::millisiemens<>> {
  static constexpr auto name = _("wpimath.units.millisiemens");
};

template <> struct handle_type_name<wpi::units::millisiemens_> {
  static constexpr auto name = _("wpimath.units.millisiemens");
};

template <> struct handle_type_name<wpi::units::kilosiemens<>> {
  static constexpr auto name = _("wpimath.units.kilosiemens");
};

template <> struct handle_type_name<wpi::units::kilosiemens_> {
  static constexpr auto name = _("wpimath.units.kilosiemens");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
