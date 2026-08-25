// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/struct/HolonomicSampleStruct.hpp"

namespace {
constexpr size_t TIMESTAMP_OFF = 0;
constexpr size_t POSE_OFF = TIMESTAMP_OFF + 8;
constexpr size_t VELOCITY_OFF = POSE_OFF + 24;
constexpr size_t ACCELERATION_OFF = VELOCITY_OFF + 24;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::HolonomicSample>;

wpi::math::HolonomicSample StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::HolonomicSample{
      wpi::units::seconds<>{
          wpi::util::UnpackStruct<double, TIMESTAMP_OFF>(data)},
      wpi::util::UnpackStruct<wpi::math::Pose2d, POSE_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::ChassisVelocities, VELOCITY_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::ChassisAccelerations,
                              ACCELERATION_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::HolonomicSample& value) {
  wpi::util::PackStruct<TIMESTAMP_OFF>(data, value.time.value());
  wpi::util::PackStruct<POSE_OFF>(data, value.pose);
  wpi::util::PackStruct<VELOCITY_OFF>(data, value.velocity);
  wpi::util::PackStruct<ACCELERATION_OFF>(data, value.acceleration);
}
