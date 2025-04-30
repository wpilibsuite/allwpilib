// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Transform2dStruct.h"

namespace {
constexpr size_t TRANSLATION_OFF = 0;
constexpr size_t ROTATION_OFF =
    TRANSLATION_OFF + wpi::GetStructSize<frc::Translation2d>();
}  // namespace

using StructType = wpi::Struct<frc::Transform2d>;

frc::Transform2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Transform2d{
      wpi::UnpackStruct<frc::Translation2d, TRANSLATION_OFF>(data),
      wpi::UnpackStruct<frc::Rotation2d, ROTATION_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Transform2d& value) {
  wpi::PackStruct<TRANSLATION_OFF>(data, value.Translation());
  wpi::PackStruct<ROTATION_OFF>(data, value.Rotation());
}
