#pragma once

#include <units/luminous_flux.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::lumen_t> {
  static constexpr auto name = _("wpimath.units.lumens");
};

template <> struct handle_type_name<units::lumens> {
  static constexpr auto name = _("wpimath.units.lumens");
};

template <> struct handle_type_name<units::nanolumen_t> {
  static constexpr auto name = _("wpimath.units.nanolumens");
};

template <> struct handle_type_name<units::nanolumens> {
  static constexpr auto name = _("wpimath.units.nanolumens");
};

template <> struct handle_type_name<units::microlumen_t> {
  static constexpr auto name = _("wpimath.units.microlumens");
};

template <> struct handle_type_name<units::microlumens> {
  static constexpr auto name = _("wpimath.units.microlumens");
};

template <> struct handle_type_name<units::millilumen_t> {
  static constexpr auto name = _("wpimath.units.millilumens");
};

template <> struct handle_type_name<units::millilumens> {
  static constexpr auto name = _("wpimath.units.millilumens");
};

template <> struct handle_type_name<units::kilolumen_t> {
  static constexpr auto name = _("wpimath.units.kilolumens");
};

template <> struct handle_type_name<units::kilolumens> {
  static constexpr auto name = _("wpimath.units.kilolumens");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
