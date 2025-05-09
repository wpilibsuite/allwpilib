// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Rotation2dStruct.h"

namespace {
constexpr size_t VALUE_OFF = 0;
}  // namespace

using StructType = wpi::Struct<frc::Rotation2d>;

frc::Rotation2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Rotation2d{
      units::radian_t{wpi::UnpackStruct<double, VALUE_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Rotation2d& value) {
  wpi::PackStruct<VALUE_OFF>(data, value.Radians().value());
}
