#pragma once

#include <wpi/units/density.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::kilograms_per_cubic_meter<>> {
  static constexpr auto name = _("wpimath.units.kilograms_per_cubic_meter");
};

template <> struct handle_type_name<wpi::units::kilograms_per_cubic_meter_> {
  static constexpr auto name = _("wpimath.units.kilograms_per_cubic_meter");
};

template <> struct handle_type_name<wpi::units::grams_per_milliliter<>> {
  static constexpr auto name = _("wpimath.units.grams_per_milliliter");
};

template <> struct handle_type_name<wpi::units::grams_per_milliliter_> {
  static constexpr auto name = _("wpimath.units.grams_per_milliliter");
};

template <> struct handle_type_name<wpi::units::kilograms_per_liter<>> {
  static constexpr auto name = _("wpimath.units.kilograms_per_liter");
};

template <> struct handle_type_name<wpi::units::kilograms_per_liter_> {
  static constexpr auto name = _("wpimath.units.kilograms_per_liter");
};

template <> struct handle_type_name<wpi::units::ounces_per_cubic_foot<>> {
  static constexpr auto name = _("wpimath.units.ounces_per_cubic_foot");
};

template <> struct handle_type_name<wpi::units::ounces_per_cubic_foot_> {
  static constexpr auto name = _("wpimath.units.ounces_per_cubic_foot");
};

template <> struct handle_type_name<wpi::units::ounces_per_cubic_inch<>> {
  static constexpr auto name = _("wpimath.units.ounces_per_cubic_inch");
};

template <> struct handle_type_name<wpi::units::ounces_per_cubic_inch_> {
  static constexpr auto name = _("wpimath.units.ounces_per_cubic_inch");
};

template <> struct handle_type_name<wpi::units::ounces_per_gallon<>> {
  static constexpr auto name = _("wpimath.units.ounces_per_gallon");
};

template <> struct handle_type_name<wpi::units::ounces_per_gallon_> {
  static constexpr auto name = _("wpimath.units.ounces_per_gallon");
};

template <> struct handle_type_name<wpi::units::pounds_per_cubic_foot<>> {
  static constexpr auto name = _("wpimath.units.pounds_per_cubic_foot");
};

template <> struct handle_type_name<wpi::units::pounds_per_cubic_foot_> {
  static constexpr auto name = _("wpimath.units.pounds_per_cubic_foot");
};

template <> struct handle_type_name<wpi::units::pounds_per_cubic_inch<>> {
  static constexpr auto name = _("wpimath.units.pounds_per_cubic_inch");
};

template <> struct handle_type_name<wpi::units::pounds_per_cubic_inch_> {
  static constexpr auto name = _("wpimath.units.pounds_per_cubic_inch");
};

template <> struct handle_type_name<wpi::units::pounds_per_gallon<>> {
  static constexpr auto name = _("wpimath.units.pounds_per_gallon");
};

template <> struct handle_type_name<wpi::units::pounds_per_gallon_> {
  static constexpr auto name = _("wpimath.units.pounds_per_gallon");
};

template <> struct handle_type_name<wpi::units::slugs_per_cubic_foot<>> {
  static constexpr auto name = _("wpimath.units.slugs_per_cubic_foot");
};

template <> struct handle_type_name<wpi::units::slugs_per_cubic_foot_> {
  static constexpr auto name = _("wpimath.units.slugs_per_cubic_foot");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
