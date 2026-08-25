// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/struct/Struct.hpp"

// Everything is converted into units for
// wpi::math::TrapezoidProfile<wpi::units::meters_> or
// wpi::math::TrapezoidProfile<wpi::units::radians_>

template <class Distance>
  requires wpi::units::Length<Distance> || wpi::units::Angle<Distance> ||
           wpi::units::traits::is_dimensionless_unit<Distance>::value
struct wpi::util::Struct<
    wpi::math::detail::TrapezoidProfileConstraints<Distance>> {
  static constexpr std::string_view GetTypeName() {
    if constexpr (wpi::units::Length<Distance>) {
      return "TrapezoidProfileConstraintsMeters";
    } else if constexpr (wpi::units::Angle<Distance>) {
      return "TrapezoidProfileConstraintsRadians";
    } else {
      return "TrapezoidProfileConstraints";
    }
  }
  static constexpr size_t GetSize() { return 16; }
  static constexpr std::string_view GetSchema() {
    return "double maxVelocity;double maxAcceleration";
  }

  static wpi::math::detail::TrapezoidProfileConstraints<Distance> Unpack(
      std::span<const uint8_t> data) {
    using BaseUnit = wpi::units::conversion_factor<
        std::ratio<1>, wpi::units::traits::dimension_of_t<Distance>>;
    using BaseConstraints =
        wpi::math::detail::TrapezoidProfileConstraints<BaseUnit>;
    constexpr size_t MAX_VELOCITY_OFF = 0;
    constexpr size_t MAX_ACCELERATION_OFF = MAX_VELOCITY_OFF + 8;
    return {typename BaseConstraints::Velocity_t{
                wpi::util::UnpackStruct<double, MAX_VELOCITY_OFF>(data)},
            typename BaseConstraints::Acceleration_t{
                wpi::util::UnpackStruct<double, MAX_ACCELERATION_OFF>(data)}};
  }

  static void Pack(
      std::span<uint8_t> data,
      const wpi::math::detail::TrapezoidProfileConstraints<Distance>& value) {
    using BaseUnit = wpi::units::conversion_factor<
        std::ratio<1>, wpi::units::traits::dimension_of_t<Distance>>;
    using BaseConstraints =
        wpi::math::detail::TrapezoidProfileConstraints<BaseUnit>;
    constexpr size_t MAX_VELOCITY_OFF = 0;
    constexpr size_t MAX_ACCELERATION_OFF = MAX_VELOCITY_OFF + 8;
    wpi::util::PackStruct<MAX_VELOCITY_OFF>(
        data, typename BaseConstraints::Velocity_t{value.maxVelocity}.value());
    wpi::util::PackStruct<MAX_ACCELERATION_OFF>(
        data, typename BaseConstraints::Acceleration_t{value.maxAcceleration}
                  .value());
  }
};

static_assert(wpi::util::StructSerializable<
              wpi::math::TrapezoidProfile<wpi::units::meters_>::Constraints>);
static_assert(wpi::util::StructSerializable<
              wpi::math::TrapezoidProfile<wpi::units::feet_>::Constraints>);
static_assert(wpi::util::StructSerializable<
              wpi::math::TrapezoidProfile<wpi::units::radians_>::Constraints>);
static_assert(
    wpi::util::StructSerializable<
        wpi::math::TrapezoidProfile<wpi::units::dimensionless_>::Constraints>);
