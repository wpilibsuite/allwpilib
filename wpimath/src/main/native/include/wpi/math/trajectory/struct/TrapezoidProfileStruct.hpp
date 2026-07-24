// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/TrapezoidProfile.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/struct/Struct.hpp"

// Everything is converted into units for
// wpi::math::TrapezoidProfile<wpi::units::meters> or
// wpi::math::TrapezoidProfile<wpi::units::radians>

template <class Distance>
  requires wpi::units::length_unit<Distance> ||
           wpi::units::angle_unit<Distance> ||
           wpi::units::traits::is_dimensionless_unit<Distance>::value
struct wpi::util::Struct<
    wpi::math::detail::TrapezoidProfileConstraints<Distance>> {
  static constexpr std::string_view GetTypeName() {
    if constexpr (wpi::units::length_unit<Distance>) {
      return "TrapezoidProfileConstraintsMeters";
    } else if constexpr (wpi::units::angle_unit<Distance>) {
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
    using BaseUnit =
        wpi::units::unit<std::ratio<1>,
                         wpi::units::traits::base_unit_of<Distance>>;
    using BaseConstraints =
        wpi::math::detail::TrapezoidProfileConstraints<BaseUnit>;
    constexpr size_t kMaxVelocityOff = 0;
    constexpr size_t kMaxAccelerationOff = kMaxVelocityOff + 8;
    return {typename BaseConstraints::Velocity_t{
                wpi::util::UnpackStruct<double, kMaxVelocityOff>(data)},
            typename BaseConstraints::Acceleration_t{
                wpi::util::UnpackStruct<double, kMaxAccelerationOff>(data)}};
  }

  static void Pack(
      std::span<uint8_t> data,
      const wpi::math::detail::TrapezoidProfileConstraints<Distance>& value) {
    using BaseUnit =
        wpi::units::unit<std::ratio<1>,
                         wpi::units::traits::base_unit_of<Distance>>;
    using BaseConstraints =
        wpi::math::detail::TrapezoidProfileConstraints<BaseUnit>;
    constexpr size_t kMaxVelocityOff = 0;
    constexpr size_t kMaxAccelerationOff = kMaxVelocityOff + 8;
    wpi::util::PackStruct<kMaxVelocityOff>(
        data, typename BaseConstraints::Velocity_t{value.maxVelocity.value()}
                  .value());
    wpi::util::PackStruct<kMaxAccelerationOff>(
        data,
        typename BaseConstraints::Acceleration_t{value.maxAcceleration.value()}
            .value());
  }
};

static_assert(wpi::util::StructSerializable<
              wpi::math::TrapezoidProfile<wpi::units::meters>::Constraints>);
static_assert(wpi::util::StructSerializable<
              wpi::math::TrapezoidProfile<wpi::units::feet>::Constraints>);
static_assert(wpi::util::StructSerializable<
              wpi::math::TrapezoidProfile<wpi::units::radians>::Constraints>);
static_assert(wpi::util::StructSerializable<wpi::math::TrapezoidProfile<
                  wpi::units::dimensionless::scalar>::Constraints>);
