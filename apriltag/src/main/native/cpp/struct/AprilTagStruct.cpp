// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/apriltag/struct/AprilTagStruct.hpp"

namespace {
constexpr size_t kIdOff = 0;
constexpr size_t kPoseOff = kIdOff + 4;
}  // namespace

using StructType = wpi::util::Struct<wpi::apriltag::AprilTag>;

wpi::apriltag::AprilTag StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::apriltag::AprilTag{
      wpi::util::UnpackStruct<int, kIdOff>(data),
      wpi::util::UnpackStruct<wpi::math::Pose3d, kPoseOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::apriltag::AprilTag& value) {
  wpi::util::PackStruct<kIdOff>(data, value.ID);
  wpi::util::PackStruct<kPoseOff>(data, value.pose);
}
