// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Rotation2dStruct.h"

using StructType = wpi::Struct<frc::Rotation2d>;

frc::Rotation2d StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return units::radian_t{wpi::UnpackStruct<double>(data)};
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::Rotation2d& value) {
  wpi::PackStruct(data, value.Radians().value());
}
