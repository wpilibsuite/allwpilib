// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/SwerveModuleStateStruct.h"

namespace {
constexpr size_t SPEED_OFF = 0;
constexpr size_t ANGLE_OFF = SPEED_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::SwerveModuleState>;

frc::SwerveModuleState StructType::Unpack(std::span<const uint8_t> data) {
  return frc::SwerveModuleState{
      units::meters_per_second_t{wpi::UnpackStruct<double, SPEED_OFF>(data)},
      wpi::UnpackStruct<frc::Rotation2d, ANGLE_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::SwerveModuleState& value) {
  wpi::PackStruct<SPEED_OFF>(data, value.speed.value());
  wpi::PackStruct<ANGLE_OFF>(data, value.angle);
}
