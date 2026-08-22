#pragma once

#include <wpi/units/concentration.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::parts_per_million<>> {
  static constexpr auto name = _("wpimath.units.parts_per_million");
};

template <> struct handle_type_name<wpi::units::parts_per_million_> {
  static constexpr auto name = _("wpimath.units.parts_per_million");
};

template <> struct handle_type_name<wpi::units::parts_per_billion<>> {
  static constexpr auto name = _("wpimath.units.parts_per_billion");
};

template <> struct handle_type_name<wpi::units::parts_per_billion_> {
  static constexpr auto name = _("wpimath.units.parts_per_billion");
};

template <> struct handle_type_name<wpi::units::parts_per_trillion<>> {
  static constexpr auto name = _("wpimath.units.parts_per_trillion");
};

template <> struct handle_type_name<wpi::units::parts_per_trillion_> {
  static constexpr auto name = _("wpimath.units.parts_per_trillion");
};

template <> struct handle_type_name<wpi::units::percent<>> {
  static constexpr auto name = _("wpimath.units.percent");
};

template <> struct handle_type_name<wpi::units::percent_> {
  static constexpr auto name = _("wpimath.units.percent");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
