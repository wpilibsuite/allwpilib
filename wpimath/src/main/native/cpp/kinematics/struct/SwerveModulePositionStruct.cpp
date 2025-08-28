// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/kinematics/struct/SwerveModulePositionStruct.h"

namespace {
constexpr size_t kDistanceOff = 0;
constexpr size_t kAngleOff = kDistanceOff + 8;
}  // namespace

using StructType = wpi::Struct<wpimath::SwerveModulePosition>;

wpimath::SwerveModulePosition StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpimath::SwerveModulePosition{
      units::meter_t{wpi::UnpackStruct<double, kDistanceOff>(data)},
      wpi::UnpackStruct<wpimath::Rotation2d, kAngleOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpimath::SwerveModulePosition& value) {
  wpi::PackStruct<kDistanceOff>(data, value.distance.value());
  wpi::PackStruct<kAngleOff>(data, value.angle);
}
