#pragma once

#include <wpi/units/pressure.h>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::pascals<>> {
  static constexpr auto name = _("wpimath.units.pascals");
};

template <> struct handle_type_name<wpi::units::pascals_> {
  static constexpr auto name = _("wpimath.units.pascals");
};

template <> struct handle_type_name<wpi::units::nanopascals<>> {
  static constexpr auto name = _("wpimath.units.nanopascals");
};

template <> struct handle_type_name<wpi::units::nanopascals_> {
  static constexpr auto name = _("wpimath.units.nanopascals");
};

template <> struct handle_type_name<wpi::units::micropascals<>> {
  static constexpr auto name = _("wpimath.units.micropascals");
};

template <> struct handle_type_name<wpi::units::micropascals_> {
  static constexpr auto name = _("wpimath.units.micropascals");
};

template <> struct handle_type_name<wpi::units::millipascals<>> {
  static constexpr auto name = _("wpimath.units.millipascals");
};

template <> struct handle_type_name<wpi::units::millipascals_> {
  static constexpr auto name = _("wpimath.units.millipascals");
};

template <> struct handle_type_name<wpi::units::kilopascals<>> {
  static constexpr auto name = _("wpimath.units.kilopascals");
};

template <> struct handle_type_name<wpi::units::kilopascals_> {
  static constexpr auto name = _("wpimath.units.kilopascals");
};

template <> struct handle_type_name<wpi::units::bars<>> {
  static constexpr auto name = _("wpimath.units.bars");
};

template <> struct handle_type_name<wpi::units::bars_> {
  static constexpr auto name = _("wpimath.units.bars");
};

template <> struct handle_type_name<wpi::units::millibars<>> {
  static constexpr auto name = _("wpimath.units.millibars");
};

template <> struct handle_type_name<wpi::units::millibars_> {
  static constexpr auto name = _("wpimath.units.millibars");
};

template <> struct handle_type_name<wpi::units::atmospheres<>> {
  static constexpr auto name = _("wpimath.units.atmospheres");
};

template <> struct handle_type_name<wpi::units::atmospheres_> {
  static constexpr auto name = _("wpimath.units.atmospheres");
};

template <> struct handle_type_name<wpi::units::pounds_per_square_inch<>> {
  static constexpr auto name = _("wpimath.units.pounds_per_square_inch");
};

template <> struct handle_type_name<wpi::units::pounds_per_square_inch_> {
  static constexpr auto name = _("wpimath.units.pounds_per_square_inch");
};

template <> struct handle_type_name<wpi::units::torrs<>> {
  static constexpr auto name = _("wpimath.units.torrs");
};

template <> struct handle_type_name<wpi::units::torrs_> {
  static constexpr auto name = _("wpimath.units.torrs");
};

template <> struct handle_type_name<wpi::units::millimeters_of_mercury<>> {
  static constexpr auto name = _("wpimath.units.millimeters_of_mercury");
};

template <> struct handle_type_name<wpi::units::millimeters_of_mercury_> {
  static constexpr auto name = _("wpimath.units.millimeters_of_mercury");
};

template <> struct handle_type_name<wpi::units::inches_of_mercury<>> {
  static constexpr auto name = _("wpimath.units.inches_of_mercury");
};

template <> struct handle_type_name<wpi::units::inches_of_mercury_> {
  static constexpr auto name = _("wpimath.units.inches_of_mercury");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
