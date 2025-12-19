#pragma once

#include <wpi/units/radiant_intensity.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::watts_per_steradian<>> {
  static constexpr auto name = _("wpimath.units.watts_per_steradian");
};

template <> struct handle_type_name<wpi::units::watts_per_steradian_> {
  static constexpr auto name = _("wpimath.units.watts_per_steradian");
};

template <> struct handle_type_name<wpi::units::nanowatts_per_steradian<>> {
  static constexpr auto name = _("wpimath.units.nanowatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::nanowatts_per_steradian_> {
  static constexpr auto name = _("wpimath.units.nanowatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::microwatts_per_steradian<>> {
  static constexpr auto name = _("wpimath.units.microwatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::microwatts_per_steradian_> {
  static constexpr auto name = _("wpimath.units.microwatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::milliwatts_per_steradian<>> {
  static constexpr auto name = _("wpimath.units.milliwatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::milliwatts_per_steradian_> {
  static constexpr auto name = _("wpimath.units.milliwatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::kilowatts_per_steradian<>> {
  static constexpr auto name = _("wpimath.units.kilowatts_per_steradian");
};

template <> struct handle_type_name<wpi::units::kilowatts_per_steradian_> {
  static constexpr auto name = _("wpimath.units.kilowatts_per_steradian");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
