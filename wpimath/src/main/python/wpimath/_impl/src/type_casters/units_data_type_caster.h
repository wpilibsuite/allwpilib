#pragma once

#include <wpi/units/data.hpp>

namespace pybind11 {
namespace detail {
template <> struct handle_type_name<wpi::units::bytes<>> {
  static constexpr auto name = _("wpimath.units.bytes");
};

template <> struct handle_type_name<wpi::units::bytes_> {
  static constexpr auto name = _("wpimath.units.bytes");
};

template <> struct handle_type_name<wpi::units::kilobytes<>> {
  static constexpr auto name = _("wpimath.units.kilobytes");
};

template <> struct handle_type_name<wpi::units::kilobytes_> {
  static constexpr auto name = _("wpimath.units.kilobytes");
};

template <> struct handle_type_name<wpi::units::megabytes<>> {
  static constexpr auto name = _("wpimath.units.megabytes");
};

template <> struct handle_type_name<wpi::units::megabytes_> {
  static constexpr auto name = _("wpimath.units.megabytes");
};

template <> struct handle_type_name<wpi::units::gigabytes<>> {
  static constexpr auto name = _("wpimath.units.gigabytes");
};

template <> struct handle_type_name<wpi::units::gigabytes_> {
  static constexpr auto name = _("wpimath.units.gigabytes");
};

template <> struct handle_type_name<wpi::units::terabytes<>> {
  static constexpr auto name = _("wpimath.units.terabytes");
};

template <> struct handle_type_name<wpi::units::terabytes_> {
  static constexpr auto name = _("wpimath.units.terabytes");
};

template <> struct handle_type_name<wpi::units::petabytes<>> {
  static constexpr auto name = _("wpimath.units.petabytes");
};

template <> struct handle_type_name<wpi::units::petabytes_> {
  static constexpr auto name = _("wpimath.units.petabytes");
};

template <> struct handle_type_name<wpi::units::exabytes<>> {
  static constexpr auto name = _("wpimath.units.exabytes");
};

template <> struct handle_type_name<wpi::units::exabytes_> {
  static constexpr auto name = _("wpimath.units.exabytes");
};

template <> struct handle_type_name<wpi::units::kibibytes<>> {
  static constexpr auto name = _("wpimath.units.kibibytes");
};

template <> struct handle_type_name<wpi::units::kibibytes_> {
  static constexpr auto name = _("wpimath.units.kibibytes");
};

template <> struct handle_type_name<wpi::units::mebibytes<>> {
  static constexpr auto name = _("wpimath.units.mebibytes");
};

template <> struct handle_type_name<wpi::units::mebibytes_> {
  static constexpr auto name = _("wpimath.units.mebibytes");
};

template <> struct handle_type_name<wpi::units::gibibytes<>> {
  static constexpr auto name = _("wpimath.units.gibibytes");
};

template <> struct handle_type_name<wpi::units::gibibytes_> {
  static constexpr auto name = _("wpimath.units.gibibytes");
};

template <> struct handle_type_name<wpi::units::tebibytes<>> {
  static constexpr auto name = _("wpimath.units.tebibytes");
};

template <> struct handle_type_name<wpi::units::tebibytes_> {
  static constexpr auto name = _("wpimath.units.tebibytes");
};

template <> struct handle_type_name<wpi::units::pebibytes<>> {
  static constexpr auto name = _("wpimath.units.pebibytes");
};

template <> struct handle_type_name<wpi::units::pebibytes_> {
  static constexpr auto name = _("wpimath.units.pebibytes");
};

template <> struct handle_type_name<wpi::units::exbibytes<>> {
  static constexpr auto name = _("wpimath.units.exbibytes");
};

template <> struct handle_type_name<wpi::units::exbibytes_> {
  static constexpr auto name = _("wpimath.units.exbibytes");
};

template <> struct handle_type_name<wpi::units::bits<>> {
  static constexpr auto name = _("wpimath.units.bits");
};

template <> struct handle_type_name<wpi::units::bits_> {
  static constexpr auto name = _("wpimath.units.bits");
};

template <> struct handle_type_name<wpi::units::kilobits<>> {
  static constexpr auto name = _("wpimath.units.kilobits");
};

template <> struct handle_type_name<wpi::units::kilobits_> {
  static constexpr auto name = _("wpimath.units.kilobits");
};

template <> struct handle_type_name<wpi::units::megabits<>> {
  static constexpr auto name = _("wpimath.units.megabits");
};

template <> struct handle_type_name<wpi::units::megabits_> {
  static constexpr auto name = _("wpimath.units.megabits");
};

template <> struct handle_type_name<wpi::units::gigabits<>> {
  static constexpr auto name = _("wpimath.units.gigabits");
};

template <> struct handle_type_name<wpi::units::gigabits_> {
  static constexpr auto name = _("wpimath.units.gigabits");
};

template <> struct handle_type_name<wpi::units::terabits<>> {
  static constexpr auto name = _("wpimath.units.terabits");
};

template <> struct handle_type_name<wpi::units::terabits_> {
  static constexpr auto name = _("wpimath.units.terabits");
};

template <> struct handle_type_name<wpi::units::petabits<>> {
  static constexpr auto name = _("wpimath.units.petabits");
};

template <> struct handle_type_name<wpi::units::petabits_> {
  static constexpr auto name = _("wpimath.units.petabits");
};

template <> struct handle_type_name<wpi::units::exabits<>> {
  static constexpr auto name = _("wpimath.units.exabits");
};

template <> struct handle_type_name<wpi::units::exabits_> {
  static constexpr auto name = _("wpimath.units.exabits");
};

template <> struct handle_type_name<wpi::units::kibibits<>> {
  static constexpr auto name = _("wpimath.units.kibibits");
};

template <> struct handle_type_name<wpi::units::kibibits_> {
  static constexpr auto name = _("wpimath.units.kibibits");
};

template <> struct handle_type_name<wpi::units::mebibits<>> {
  static constexpr auto name = _("wpimath.units.mebibits");
};

template <> struct handle_type_name<wpi::units::mebibits_> {
  static constexpr auto name = _("wpimath.units.mebibits");
};

template <> struct handle_type_name<wpi::units::gibibits<>> {
  static constexpr auto name = _("wpimath.units.gibibits");
};

template <> struct handle_type_name<wpi::units::gibibits_> {
  static constexpr auto name = _("wpimath.units.gibibits");
};

template <> struct handle_type_name<wpi::units::tebibits<>> {
  static constexpr auto name = _("wpimath.units.tebibits");
};

template <> struct handle_type_name<wpi::units::tebibits_> {
  static constexpr auto name = _("wpimath.units.tebibits");
};

template <> struct handle_type_name<wpi::units::pebibits<>> {
  static constexpr auto name = _("wpimath.units.pebibits");
};

template <> struct handle_type_name<wpi::units::pebibits_> {
  static constexpr auto name = _("wpimath.units.pebibits");
};

template <> struct handle_type_name<wpi::units::exbibits<>> {
  static constexpr auto name = _("wpimath.units.exbibits");
};

template <> struct handle_type_name<wpi::units::exbibits_> {
  static constexpr auto name = _("wpimath.units.exbibits");
};

template <> struct handle_type_name<wpi::units::nibbles<>> {
  static constexpr auto name = _("wpimath.units.nibbles");
};

template <> struct handle_type_name<wpi::units::nibbles_> {
  static constexpr auto name = _("wpimath.units.nibbles");
};

} // namespace detail
} // namespace pybind11

#include "_units_base_type_caster.h"
