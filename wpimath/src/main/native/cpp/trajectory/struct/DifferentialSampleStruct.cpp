// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/struct/DifferentialSampleStruct.hpp"

namespace {
constexpr size_t TIMESTAMP_OFF = 0;
constexpr size_t POSE_OFF = TIMESTAMP_OFF + 8;
constexpr size_t VELOCITY_OFF = POSE_OFF + 24;
constexpr size_t ACCELERATION_OFF = VELOCITY_OFF + 24;
constexpr size_t LEFT_VELOCITY_OFF = ACCELERATION_OFF + 24;
constexpr size_t RIGHT_VELOCITY_OFF = LEFT_VELOCITY_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::DifferentialSample>;

wpi::math::DifferentialSample StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::DifferentialSample{
      wpi::units::seconds<>{
          wpi::util::UnpackStruct<double, TIMESTAMP_OFF>(data)},
      wpi::util::UnpackStruct<wpi::math::Pose2d, POSE_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::ChassisVelocities, VELOCITY_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::ChassisAccelerations,
                              ACCELERATION_OFF>(data),
      wpi::units::meters_per_second<>{
          wpi::util::UnpackStruct<double, LEFT_VELOCITY_OFF>(data)},
      wpi::units::meters_per_second<>{
          wpi::util::UnpackStruct<double, RIGHT_VELOCITY_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DifferentialSample& value) {
  wpi::util::PackStruct<TIMESTAMP_OFF>(data, value.time.value());
  wpi::util::PackStruct<POSE_OFF>(data, value.pose);
  wpi::util::PackStruct<VELOCITY_OFF>(data, value.velocity);
  wpi::util::PackStruct<ACCELERATION_OFF>(data, value.acceleration);
  wpi::util::PackStruct<LEFT_VELOCITY_OFF>(data, value.leftVelocity.value());
  wpi::util::PackStruct<RIGHT_VELOCITY_OFF>(data, value.rightVelocity.value());
}
