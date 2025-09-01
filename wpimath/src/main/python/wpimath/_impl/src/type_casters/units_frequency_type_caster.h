#pragma once

#include <units/frequency.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::hertz_t> {
  static constexpr auto name = _("wpimath.units.hertz");
};

template <> struct handle_type_name<units::hertz> {
  static constexpr auto name = _("wpimath.units.hertz");
};

template <> struct handle_type_name<units::nanohertz_t> {
  static constexpr auto name = _("wpimath.units.nanohertz");
};

template <> struct handle_type_name<units::nanohertz> {
  static constexpr auto name = _("wpimath.units.nanohertz");
};

template <> struct handle_type_name<units::microhertz_t> {
  static constexpr auto name = _("wpimath.units.microhertz");
};

template <> struct handle_type_name<units::microhertz> {
  static constexpr auto name = _("wpimath.units.microhertz");
};

template <> struct handle_type_name<units::millihertz_t> {
  static constexpr auto name = _("wpimath.units.millihertz");
};

template <> struct handle_type_name<units::millihertz> {
  static constexpr auto name = _("wpimath.units.millihertz");
};

template <> struct handle_type_name<units::kilohertz_t> {
  static constexpr auto name = _("wpimath.units.kilohertz");
};

template <> struct handle_type_name<units::kilohertz> {
  static constexpr auto name = _("wpimath.units.kilohertz");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
