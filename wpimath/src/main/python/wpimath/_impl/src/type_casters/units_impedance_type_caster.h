#pragma once

#include <units/impedance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::ohm_t> {
  static constexpr auto name = _("wpimath.units.ohms");
};

template <> struct handle_type_name<units::ohms> {
  static constexpr auto name = _("wpimath.units.ohms");
};

template <> struct handle_type_name<units::nanoohm_t> {
  static constexpr auto name = _("wpimath.units.nanoohms");
};

template <> struct handle_type_name<units::nanoohms> {
  static constexpr auto name = _("wpimath.units.nanoohms");
};

template <> struct handle_type_name<units::microohm_t> {
  static constexpr auto name = _("wpimath.units.microohms");
};

template <> struct handle_type_name<units::microohms> {
  static constexpr auto name = _("wpimath.units.microohms");
};

template <> struct handle_type_name<units::milliohm_t> {
  static constexpr auto name = _("wpimath.units.milliohms");
};

template <> struct handle_type_name<units::milliohms> {
  static constexpr auto name = _("wpimath.units.milliohms");
};

template <> struct handle_type_name<units::kiloohm_t> {
  static constexpr auto name = _("wpimath.units.kiloohms");
};

template <> struct handle_type_name<units::kiloohms> {
  static constexpr auto name = _("wpimath.units.kiloohms");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
