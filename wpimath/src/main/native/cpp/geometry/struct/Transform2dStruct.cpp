// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Transform2dStruct.hpp"

namespace {
constexpr size_t kTranslationOff = 0;
constexpr size_t kRotationOff =
    kTranslationOff + wpi::util::GetStructSize<wpi::math::Translation2d>();
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Transform2d>;

wpi::math::Transform2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Transform2d{
      wpi::util::UnpackStruct<wpi::math::Translation2d, kTranslationOff>(data),
      wpi::util::UnpackStruct<wpi::math::Rotation2d, kRotationOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Transform2d& value) {
  wpi::util::PackStruct<kTranslationOff>(data, value.Translation());
  wpi::util::PackStruct<kRotationOff>(data, value.Rotation());
}
