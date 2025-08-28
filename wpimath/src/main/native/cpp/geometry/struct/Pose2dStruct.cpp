// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/struct/Pose2dStruct.h>

namespace {
constexpr size_t kTranslationOff = 0;
constexpr size_t kRotationOff =
    kTranslationOff + wpi::GetStructSize<wpi::math::Translation2d>();
}  // namespace

using StructType = wpi::Struct<wpi::math::Pose2d>;

wpi::math::Pose2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Pose2d{
      wpi::UnpackStruct<wpi::math::Translation2d, kTranslationOff>(data),
      wpi::UnpackStruct<wpi::math::Rotation2d, kRotationOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const wpi::math::Pose2d& value) {
  wpi::PackStruct<kTranslationOff>(data, value.Translation());
  wpi::PackStruct<kRotationOff>(data, value.Rotation());
}
