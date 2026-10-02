// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Pose2dStruct.hpp"

namespace {
constexpr size_t TRANSLATION_OFF = 0;
constexpr size_t ROTATION_OFF =
    TRANSLATION_OFF + wpi::util::GetStructSize<wpi::math::Translation2d>();
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Pose2d>;

wpi::math::Pose2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Pose2d{
      wpi::util::UnpackStruct<wpi::math::Translation2d, TRANSLATION_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::Rotation2d, ROTATION_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const wpi::math::Pose2d& value) {
  wpi::util::PackStruct<TRANSLATION_OFF>(data, value.Translation());
  wpi::util::PackStruct<ROTATION_OFF>(data, value.Rotation());
}
