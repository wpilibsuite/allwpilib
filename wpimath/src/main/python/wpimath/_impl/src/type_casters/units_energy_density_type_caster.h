#pragma once

#include <wpi/units/energy_density.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::joules_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.joules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::joules_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.joules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::nanojoules_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.nanojoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::nanojoules_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.nanojoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::microjoules_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.microjoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::microjoules_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.microjoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::millijoules_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.millijoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::millijoules_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.millijoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::kilojoules_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.kilojoules_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::kilojoules_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.kilojoules_per_meter_cubed");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
