// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/struct/Transform3dStruct.h"

namespace {
constexpr size_t kTranslationOff = 0;
constexpr size_t kRotationOff =
    kTranslationOff + wpi::GetStructSize<wpimath::Translation3d>();
}  // namespace

using StructType = wpi::Struct<wpimath::Transform3d>;

wpimath::Transform3d StructType::Unpack(std::span<const uint8_t> data) {
  return wpimath::Transform3d{
      wpi::UnpackStruct<wpimath::Translation3d, kTranslationOff>(data),
      wpi::UnpackStruct<wpimath::Rotation3d, kRotationOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpimath::Transform3d& value) {
  wpi::PackStruct<kTranslationOff>(data, value.Translation());
  wpi::PackStruct<kRotationOff>(data, value.Rotation());
}
