#pragma once

#include <wpi/units/substance_mass.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::grams_per_mole<>> {
  static constexpr auto name = _("wpimath.units.grams_per_mole");
};

template <> struct handle_type_name<wpi::units::grams_per_mole_> {
  static constexpr auto name = _("wpimath.units.grams_per_mole");
};

template <> struct handle_type_name<wpi::units::nanograms_per_mole<>> {
  static constexpr auto name = _("wpimath.units.nanograms_per_mole");
};

template <> struct handle_type_name<wpi::units::nanograms_per_mole_> {
  static constexpr auto name = _("wpimath.units.nanograms_per_mole");
};

template <> struct handle_type_name<wpi::units::micrograms_per_mole<>> {
  static constexpr auto name = _("wpimath.units.micrograms_per_mole");
};

template <> struct handle_type_name<wpi::units::micrograms_per_mole_> {
  static constexpr auto name = _("wpimath.units.micrograms_per_mole");
};

template <> struct handle_type_name<wpi::units::milligrams_per_mole<>> {
  static constexpr auto name = _("wpimath.units.milligrams_per_mole");
};

template <> struct handle_type_name<wpi::units::milligrams_per_mole_> {
  static constexpr auto name = _("wpimath.units.milligrams_per_mole");
};

template <> struct handle_type_name<wpi::units::kilograms_per_mole<>> {
  static constexpr auto name = _("wpimath.units.kilograms_per_mole");
};

template <> struct handle_type_name<wpi::units::kilograms_per_mole_> {
  static constexpr auto name = _("wpimath.units.kilograms_per_mole");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
