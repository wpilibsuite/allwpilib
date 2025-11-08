#pragma once

#include "wpi/units/luminous_flux.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::lumen_t> {
  static constexpr auto name = _("wpimath.units.lumens");
};

template <> struct handle_type_name<wpi::units::lumens> {
  static constexpr auto name = _("wpimath.units.lumens");
};

template <> struct handle_type_name<wpi::units::nanolumen_t> {
  static constexpr auto name = _("wpimath.units.nanolumens");
};

template <> struct handle_type_name<wpi::units::nanolumens> {
  static constexpr auto name = _("wpimath.units.nanolumens");
};

template <> struct handle_type_name<wpi::units::microlumen_t> {
  static constexpr auto name = _("wpimath.units.microlumens");
};

template <> struct handle_type_name<wpi::units::microlumens> {
  static constexpr auto name = _("wpimath.units.microlumens");
};

template <> struct handle_type_name<wpi::units::millilumen_t> {
  static constexpr auto name = _("wpimath.units.millilumens");
};

template <> struct handle_type_name<wpi::units::millilumens> {
  static constexpr auto name = _("wpimath.units.millilumens");
};

template <> struct handle_type_name<wpi::units::kilolumen_t> {
  static constexpr auto name = _("wpimath.units.kilolumens");
};

template <> struct handle_type_name<wpi::units::kilolumens> {
  static constexpr auto name = _("wpimath.units.kilolumens");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
