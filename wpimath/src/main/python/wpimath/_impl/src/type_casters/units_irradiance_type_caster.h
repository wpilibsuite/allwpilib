#pragma once

#include <wpi/units/irradiance.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::watts_per_meter_squared<>> {
  static constexpr auto name = _("wpimath.units.watts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::watts_per_meter_squared_> {
  static constexpr auto name = _("wpimath.units.watts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::nanowatts_per_meter_squared<>> {
  static constexpr auto name = _("wpimath.units.nanowatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::nanowatts_per_meter_squared_> {
  static constexpr auto name = _("wpimath.units.nanowatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::microwatts_per_meter_squared<>> {
  static constexpr auto name = _("wpimath.units.microwatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::microwatts_per_meter_squared_> {
  static constexpr auto name = _("wpimath.units.microwatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::milliwatts_per_meter_squared<>> {
  static constexpr auto name = _("wpimath.units.milliwatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::milliwatts_per_meter_squared_> {
  static constexpr auto name = _("wpimath.units.milliwatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::kilowatts_per_meter_squared<>> {
  static constexpr auto name = _("wpimath.units.kilowatts_per_meter_squared");
};

template <> struct handle_type_name<wpi::units::kilowatts_per_meter_squared_> {
  static constexpr auto name = _("wpimath.units.kilowatts_per_meter_squared");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
