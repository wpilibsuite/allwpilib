#pragma once

#include <wpi/units/substance_concentration.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::molars<>> {
  static constexpr auto name = _("wpimath.units.molars");
};

template <> struct handle_type_name<wpi::units::molars_> {
  static constexpr auto name = _("wpimath.units.molars");
};

template <> struct handle_type_name<wpi::units::nanomolars<>> {
  static constexpr auto name = _("wpimath.units.nanomolars");
};

template <> struct handle_type_name<wpi::units::nanomolars_> {
  static constexpr auto name = _("wpimath.units.nanomolars");
};

template <> struct handle_type_name<wpi::units::micromolars<>> {
  static constexpr auto name = _("wpimath.units.micromolars");
};

template <> struct handle_type_name<wpi::units::micromolars_> {
  static constexpr auto name = _("wpimath.units.micromolars");
};

template <> struct handle_type_name<wpi::units::millimolars<>> {
  static constexpr auto name = _("wpimath.units.millimolars");
};

template <> struct handle_type_name<wpi::units::millimolars_> {
  static constexpr auto name = _("wpimath.units.millimolars");
};

template <> struct handle_type_name<wpi::units::kilomolars<>> {
  static constexpr auto name = _("wpimath.units.kilomolars");
};

template <> struct handle_type_name<wpi::units::kilomolars_> {
  static constexpr auto name = _("wpimath.units.kilomolars");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
