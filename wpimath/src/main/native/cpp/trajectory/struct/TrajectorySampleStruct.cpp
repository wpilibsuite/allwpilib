// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/struct/TrajectorySampleStruct.hpp"

namespace {
constexpr size_t kTimestampOff = 0;
constexpr size_t kPoseOff = kTimestampOff + 8;
constexpr size_t kVelocityOff = kPoseOff + 24;
constexpr size_t kAccelerationOff = kVelocityOff + 24;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::TrajectorySample>;

wpi::math::TrajectorySample StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::TrajectorySample{
      wpi::units::second_t{
          wpi::util::UnpackStruct<double, kTimestampOff>(data)},
      wpi::util::Struct<wpi::math::Pose2d>::Unpack(data.subspan(kPoseOff, 24)),
      wpi::util::Struct<wpi::math::ChassisSpeeds>::Unpack(
          data.subspan(kVelocityOff, 24)),
      wpi::util::Struct<wpi::math::ChassisAccelerations>::Unpack(
          data.subspan(kAccelerationOff, 24)),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::TrajectorySample& value) {
  wpi::util::PackStruct<kTimestampOff>(data, value.timestamp.value());
  wpi::util::Struct<wpi::math::Pose2d>::Pack(data.subspan(kPoseOff, 24),
                                             value.pose);
  wpi::util::Struct<wpi::math::ChassisSpeeds>::Pack(
      data.subspan(kVelocityOff, 24), value.velocity);
  wpi::util::Struct<wpi::math::ChassisAccelerations>::Pack(
      data.subspan(kAccelerationOff, 24), value.acceleration);
}
