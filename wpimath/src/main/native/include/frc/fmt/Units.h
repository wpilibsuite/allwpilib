// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>

#include "units/base.h"

/**
 * Formatter for unit types.
 *
 * @tparam Units Unit tag for which type of units the `unit_t` represents (e.g.
 *               meters).
 * @tparam T Underlying type of the storage. Defaults to double.
 * @tparam NonLinearScale Optional scale class for the units. Defaults to linear
 *                        (i.e. does not scale the unit value). Examples of
 *                        non-linear scales could be logarithmic, decibel, or
 *                        richter scales. Non-linear scales must adhere to the
 *                        non-linear-scale concept.
 */
template <class Units, typename T, template <typename> class NonLinearScale>
struct fmt::formatter<units::unit_t<Units, T, NonLinearScale>>
    : fmt::formatter<double> {
  /**
   * Writes out a formatted unit.
   *
   * @tparam FormatContext Format string context type.
   * @param obj Unit instance.
   * @param ctx Format string context.
   */
  template <typename FormatContext>
  auto format(const units::unit_t<Units, T, NonLinearScale>& obj,
              FormatContext& ctx) {
    using BaseUnits =
        units::unit<std::ratio<1>,
                    typename units::traits::unit_traits<Units>::base_unit_type>;

    auto out = ctx.out();

    out = fmt::formatter<double>::format(
        units::convert<Units, BaseUnits>(obj()), ctx);

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
};
