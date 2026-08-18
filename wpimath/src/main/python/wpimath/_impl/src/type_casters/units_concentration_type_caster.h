#pragma once

#include "wpi/units/concentration.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::ppm_t> {
  static constexpr auto name = _("wpimath.units.parts_per_million");
};

template <> struct handle_type_name<wpi::units::parts_per_million> {
  static constexpr auto name = _("wpimath.units.parts_per_million");
};

template <> struct handle_type_name<wpi::units::ppb_t> {
  static constexpr auto name = _("wpimath.units.parts_per_billion");
};

template <> struct handle_type_name<wpi::units::parts_per_billion> {
  static constexpr auto name = _("wpimath.units.parts_per_billion");
};

template <> struct handle_type_name<wpi::units::ppt_t> {
  static constexpr auto name = _("wpimath.units.parts_per_trillion");
};

template <> struct handle_type_name<wpi::units::parts_per_trillion> {
  static constexpr auto name = _("wpimath.units.parts_per_trillion");
};

template <> struct handle_type_name<wpi::units::percent_t> {
  static constexpr auto name = _("wpimath.units.percent");
};

template <> struct handle_type_name<wpi::units::percent> {
  static constexpr auto name = _("wpimath.units.percent");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
