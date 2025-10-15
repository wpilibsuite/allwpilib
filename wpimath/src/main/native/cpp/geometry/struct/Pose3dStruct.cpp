// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Pose3dStruct.hpp"

namespace {
constexpr size_t kTranslationOff = 0;
constexpr size_t kRotationOff =
    kTranslationOff + wpi::GetStructSize<frc::Translation3d>();
}  // namespace

using StructType = wpi::Struct<frc::Pose3d>;

frc::Pose3d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Pose3d{
      wpi::UnpackStruct<frc::Translation3d, kTranslationOff>(data),
      wpi::UnpackStruct<frc::Rotation3d, kRotationOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Pose3d& value) {
  wpi::PackStruct<kTranslationOff>(data, value.Translation());
  wpi::PackStruct<kRotationOff>(data, value.Rotation());
}
