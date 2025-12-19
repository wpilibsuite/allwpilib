#pragma once

#include <wpi/units/substance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::mols<>> {
  static constexpr auto name = _("wpimath.units.mols");
};

template <> struct handle_type_name<wpi::units::mols_> {
  static constexpr auto name = _("wpimath.units.mols");
};

template <> struct handle_type_name<wpi::units::nanomols<>> {
  static constexpr auto name = _("wpimath.units.nanomols");
};

template <> struct handle_type_name<wpi::units::nanomols_> {
  static constexpr auto name = _("wpimath.units.nanomols");
};

template <> struct handle_type_name<wpi::units::micromols<>> {
  static constexpr auto name = _("wpimath.units.micromols");
};

template <> struct handle_type_name<wpi::units::micromols_> {
  static constexpr auto name = _("wpimath.units.micromols");
};

template <> struct handle_type_name<wpi::units::millimols<>> {
  static constexpr auto name = _("wpimath.units.millimols");
};

template <> struct handle_type_name<wpi::units::millimols_> {
  static constexpr auto name = _("wpimath.units.millimols");
};

template <> struct handle_type_name<wpi::units::kilomols<>> {
  static constexpr auto name = _("wpimath.units.kilomols");
};

template <> struct handle_type_name<wpi::units::kilomols_> {
  static constexpr auto name = _("wpimath.units.kilomols");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
