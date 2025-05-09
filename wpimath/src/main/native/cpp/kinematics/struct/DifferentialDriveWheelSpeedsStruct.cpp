// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/DifferentialDriveWheelSpeedsStruct.h"

namespace {
constexpr size_t LEFT_OFF = 0;
constexpr size_t RIGHT_OFF = LEFT_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::DifferentialDriveWheelSpeeds>;

frc::DifferentialDriveWheelSpeeds StructType::Unpack(
    std::span<const uint8_t> data) {
  return frc::DifferentialDriveWheelSpeeds{
      units::meters_per_second_t{wpi::UnpackStruct<double, LEFT_OFF>(data)},
      units::meters_per_second_t{wpi::UnpackStruct<double, RIGHT_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::DifferentialDriveWheelSpeeds& value) {
  wpi::PackStruct<LEFT_OFF>(data, value.left.value());
  wpi::PackStruct<RIGHT_OFF>(data, value.right.value());
}
