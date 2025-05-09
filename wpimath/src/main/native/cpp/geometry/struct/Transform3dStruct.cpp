// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Transform3dStruct.h"

namespace {
constexpr size_t TRANSLATION_OFF = 0;
constexpr size_t ROTATION_OFF =
    TRANSLATION_OFF + wpi::GetStructSize<frc::Translation3d>();
}  // namespace

using StructType = wpi::Struct<frc::Transform3d>;

frc::Transform3d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Transform3d{
      wpi::UnpackStruct<frc::Translation3d, TRANSLATION_OFF>(data),
      wpi::UnpackStruct<frc::Rotation3d, ROTATION_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Transform3d& value) {
  wpi::PackStruct<TRANSLATION_OFF>(data, value.Translation());
  wpi::PackStruct<ROTATION_OFF>(data, value.Rotation());
}
