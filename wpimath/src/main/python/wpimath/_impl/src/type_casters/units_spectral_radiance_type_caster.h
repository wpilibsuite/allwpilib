#pragma once

#include <wpi/units/spectral_radiance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::watts_per_steradian_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.watts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::watts_per_steradian_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.watts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::nanowatts_per_steradian_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.nanowatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::nanowatts_per_steradian_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.nanowatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::microwatts_per_steradian_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.microwatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::microwatts_per_steradian_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.microwatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::milliwatts_per_steradian_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.milliwatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::milliwatts_per_steradian_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.milliwatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::kilowatts_per_steradian_per_meter_cubed<>> {
  static constexpr auto name = _("wpimath.units.kilowatts_per_steradian_per_meter_cubed");
};

template <> struct handle_type_name<wpi::units::kilowatts_per_steradian_per_meter_cubed_> {
  static constexpr auto name = _("wpimath.units.kilowatts_per_steradian_per_meter_cubed");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
