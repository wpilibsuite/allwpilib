#pragma once

#include "wpi/units/pressure.hpp"

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::pascal_t> {
  static constexpr auto name = _("wpimath.units.pascals");
};

template <> struct handle_type_name<wpi::units::pascals> {
  static constexpr auto name = _("wpimath.units.pascals");
};

template <> struct handle_type_name<wpi::units::nanopascal_t> {
  static constexpr auto name = _("wpimath.units.nanopascals");
};

template <> struct handle_type_name<wpi::units::nanopascals> {
  static constexpr auto name = _("wpimath.units.nanopascals");
};

template <> struct handle_type_name<wpi::units::micropascal_t> {
  static constexpr auto name = _("wpimath.units.micropascals");
};

template <> struct handle_type_name<wpi::units::micropascals> {
  static constexpr auto name = _("wpimath.units.micropascals");
};

template <> struct handle_type_name<wpi::units::millipascal_t> {
  static constexpr auto name = _("wpimath.units.millipascals");
};

template <> struct handle_type_name<wpi::units::millipascals> {
  static constexpr auto name = _("wpimath.units.millipascals");
};

template <> struct handle_type_name<wpi::units::kilopascal_t> {
  static constexpr auto name = _("wpimath.units.kilopascals");
};

template <> struct handle_type_name<wpi::units::kilopascals> {
  static constexpr auto name = _("wpimath.units.kilopascals");
};

template <> struct handle_type_name<wpi::units::bar_t> {
  static constexpr auto name = _("wpimath.units.bars");
};

template <> struct handle_type_name<wpi::units::bars> {
  static constexpr auto name = _("wpimath.units.bars");
};

template <> struct handle_type_name<wpi::units::mbar_t> {
  static constexpr auto name = _("wpimath.units.mbars");
};

template <> struct handle_type_name<wpi::units::mbars> {
  static constexpr auto name = _("wpimath.units.mbars");
};

template <> struct handle_type_name<wpi::units::atmosphere_t> {
  static constexpr auto name = _("wpimath.units.atmospheres");
};

template <> struct handle_type_name<wpi::units::atmospheres> {
  static constexpr auto name = _("wpimath.units.atmospheres");
};

template <> struct handle_type_name<wpi::units::pounds_per_square_inch_t> {
  static constexpr auto name = _("wpimath.units.pounds_per_square_inch");
};

template <> struct handle_type_name<wpi::units::pounds_per_square_inch> {
  static constexpr auto name = _("wpimath.units.pounds_per_square_inch");
};

template <> struct handle_type_name<wpi::units::torr_t> {
  static constexpr auto name = _("wpimath.units.torrs");
};

template <> struct handle_type_name<wpi::units::torrs> {
  static constexpr auto name = _("wpimath.units.torrs");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
