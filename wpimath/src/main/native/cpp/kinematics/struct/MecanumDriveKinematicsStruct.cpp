// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveKinematicsStruct.h"

namespace {
constexpr size_t kFrontLeftOff = 0;
constexpr size_t kFrontRightOff =
    kFrontLeftOff + wpi::GetStructSize<wpi::math::Translation2d>();
constexpr size_t kRearLeftOff =
    kFrontRightOff + wpi::GetStructSize<wpi::math::Translation2d>();
constexpr size_t kRearRightOff =
    kRearLeftOff + wpi::GetStructSize<wpi::math::Translation2d>();
}  // namespace

using StructType = wpi::Struct<wpi::math::MecanumDriveKinematics>;

wpi::math::MecanumDriveKinematics StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::MecanumDriveKinematics{
      wpi::UnpackStruct<wpi::math::Translation2d, kFrontLeftOff>(data),
      wpi::UnpackStruct<wpi::math::Translation2d, kFrontRightOff>(data),
      wpi::UnpackStruct<wpi::math::Translation2d, kRearLeftOff>(data),
      wpi::UnpackStruct<wpi::math::Translation2d, kRearRightOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::MecanumDriveKinematics& value) {
  wpi::PackStruct<kFrontLeftOff>(data, value.GetFrontLeft());
  wpi::PackStruct<kFrontRightOff>(data, value.GetFrontRight());
  wpi::PackStruct<kRearLeftOff>(data, value.GetRearLeft());
  wpi::PackStruct<kRearRightOff>(data, value.GetRearRight());
}
