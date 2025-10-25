#pragma once

#include <units/conductance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<units::siemens_t> {
  static constexpr auto name = _("wpimath.units.siemens");
};

template <> struct handle_type_name<units::siemens> {
  static constexpr auto name = _("wpimath.units.siemens");
};

template <> struct handle_type_name<units::nanosiemens_t> {
  static constexpr auto name = _("wpimath.units.nanosiemens");
};

template <> struct handle_type_name<units::nanosiemens> {
  static constexpr auto name = _("wpimath.units.nanosiemens");
};

template <> struct handle_type_name<units::microsiemens_t> {
  static constexpr auto name = _("wpimath.units.microsiemens");
};

template <> struct handle_type_name<units::microsiemens> {
  static constexpr auto name = _("wpimath.units.microsiemens");
};

template <> struct handle_type_name<units::millisiemens_t> {
  static constexpr auto name = _("wpimath.units.millisiemens");
};

template <> struct handle_type_name<units::millisiemens> {
  static constexpr auto name = _("wpimath.units.millisiemens");
};

template <> struct handle_type_name<units::kilosiemens_t> {
  static constexpr auto name = _("wpimath.units.kilosiemens");
};

template <> struct handle_type_name<units::kilosiemens> {
  static constexpr auto name = _("wpimath.units.kilosiemens");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
