// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/SwerveModulePositionStruct.h"

namespace {
constexpr size_t DISTANCE_OFF = 0;
constexpr size_t ANGLE_OFF = DISTANCE_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::SwerveModulePosition>;

frc::SwerveModulePosition StructType::Unpack(std::span<const uint8_t> data) {
  return frc::SwerveModulePosition{
      units::meter_t{wpi::UnpackStruct<double, DISTANCE_OFF>(data)},
      wpi::UnpackStruct<frc::Rotation2d, ANGLE_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::SwerveModulePosition& value) {
  wpi::PackStruct<DISTANCE_OFF>(data, value.distance.value());
  wpi::PackStruct<ANGLE_OFF>(data, value.angle);
}
