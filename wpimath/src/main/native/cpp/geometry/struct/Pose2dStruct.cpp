// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/struct/Pose2dStruct.h"

namespace {
constexpr size_t kTranslationOff = 0;
constexpr size_t kRotationOff =
    kTranslationOff + wpi::GetStructSize<wpimath::Translation2d>();
}  // namespace

using StructType = wpi::Struct<wpimath::Pose2d>;

wpimath::Pose2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpimath::Pose2d{
      wpi::UnpackStruct<wpimath::Translation2d, kTranslationOff>(data),
      wpi::UnpackStruct<wpimath::Rotation2d, kRotationOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const wpimath::Pose2d& value) {
  wpi::PackStruct<kTranslationOff>(data, value.Translation());
  wpi::PackStruct<kRotationOff>(data, value.Rotation());
}
