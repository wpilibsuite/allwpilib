// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include <fmt/format.h>

#include "units/base.h"

// FIXME: Replace enable_if with requires clause and remove <type_traits>
// include once using GCC >= 12. GCC 11 incorrectly emits a struct redefinition
// error because it doesn't use the requires clause to disambiguate.

/**
 * Formatter for unit types.
 */
template <typename Unit, typename CharT>
struct fmt::formatter<Unit, CharT,
                      std::enable_if_t<units::traits::is_unit_t_v<Unit>>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return m_underlying.parse(ctx);
  }

  /**
   * Writes out a formatted unit.
   *
   * @param obj Unit instance.
   * @param ctx Format string context.
   */
  template <typename FmtContext>
  auto format(const Unit& obj, FmtContext& ctx) const {
    using Units = typename Unit::unit_type;
    using BaseUnits =
        units::unit<std::ratio<1>,
                    typename units::traits::unit_traits<Units>::base_unit_type>;

    auto out = ctx.out();

    out = m_underlying.format(units::convert<Units, BaseUnits>(obj()), ctx);

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::meter_ratio::num != 0) {
      out = fmt::format_to(out, " m");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::meter_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::meter_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::meter_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::meter_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::meter_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::kilogram_ratio::num != 0) {
      out = fmt::format_to(out, " kg");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::kilogram_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::kilogram_ratio::num != 1) {
      out = fmt::format_to(out, "^{}",
                           units::traits::unit_traits<
                               Units>::base_unit_type::kilogram_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::kilogram_ratio::den != 1) {
      out = fmt::format_to(out, "/{}",
                           units::traits::unit_traits<
                               Units>::base_unit_type::kilogram_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::second_ratio::num != 0) {
      out = fmt::format_to(out, " s");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::second_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::second_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::second_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::second_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::second_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::ampere_ratio::num != 0) {
      out = fmt::format_to(out, " A");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::ampere_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::ampere_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::ampere_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::ampere_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::ampere_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::kelvin_ratio::num != 0) {
      out = fmt::format_to(out, " K");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::kelvin_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::kelvin_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::kelvin_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::kelvin_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::kelvin_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::mole_ratio::num != 0) {
      out = fmt::format_to(out, " mol");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::mole_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::mole_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::mole_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::mole_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::mole_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::candela_ratio::num != 0) {
      out = fmt::format_to(out, " cd");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::candela_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::candela_ratio::num != 1) {
      out = fmt::format_to(out, "^{}",
                           units::traits::unit_traits<
                               Units>::base_unit_type::candela_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::candela_ratio::den != 1) {
      out = fmt::format_to(out, "/{}",
                           units::traits::unit_traits<
                               Units>::base_unit_type::candela_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::radian_ratio::num != 0) {
      out = fmt::format_to(out, " rad");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::radian_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::radian_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::radian_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::radian_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::radian_ratio::den);
    }

    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::byte_ratio::num != 0) {
      out = fmt::format_to(out, " b");
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::byte_ratio::num != 0 &&
                  units::traits::unit_traits<
                      Units>::base_unit_type::byte_ratio::num != 1) {
      out = fmt::format_to(
          out, "^{}",
          units::traits::unit_traits<Units>::base_unit_type::byte_ratio::num);
    }
    if constexpr (units::traits::unit_traits<
                      Units>::base_unit_type::byte_ratio::den != 1) {
      out = fmt::format_to(
          out, "/{}",
          units::traits::unit_traits<Units>::base_unit_type::byte_ratio::den);
    }

    return out;
  }

 private:
  fmt::formatter<typename Unit::underlying_type, CharT> m_underlying;
};
