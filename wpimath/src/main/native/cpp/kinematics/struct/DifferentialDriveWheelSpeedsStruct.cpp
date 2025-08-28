// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/DifferentialDriveWheelSpeedsStruct.h"

namespace {
constexpr size_t kLeftOff = 0;
constexpr size_t kRightOff = kLeftOff + 8;
}  // namespace

using StructType = wpi::Struct<wpi::math::DifferentialDriveWheelSpeeds>;

wpi::math::DifferentialDriveWheelSpeeds StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::DifferentialDriveWheelSpeeds{
      units::meters_per_second_t{wpi::UnpackStruct<double, kLeftOff>(data)},
      units::meters_per_second_t{wpi::UnpackStruct<double, kRightOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DifferentialDriveWheelSpeeds& value) {
  wpi::PackStruct<kLeftOff>(data, value.left.value());
  wpi::PackStruct<kRightOff>(data, value.right.value());
}
