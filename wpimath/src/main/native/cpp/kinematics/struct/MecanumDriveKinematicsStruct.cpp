// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/kinematics/struct/MecanumDriveKinematicsStruct.h"

namespace {
constexpr size_t kFrontLeftOff = 0;
constexpr size_t kFrontRightOff =
    kFrontLeftOff + wpi::GetStructSize<wpimath::Translation2d>();
constexpr size_t kRearLeftOff =
    kFrontRightOff + wpi::GetStructSize<wpimath::Translation2d>();
constexpr size_t kRearRightOff =
    kRearLeftOff + wpi::GetStructSize<wpimath::Translation2d>();
}  // namespace

using StructType = wpi::Struct<wpimath::MecanumDriveKinematics>;

wpimath::MecanumDriveKinematics StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpimath::MecanumDriveKinematics{
      wpi::UnpackStruct<wpimath::Translation2d, kFrontLeftOff>(data),
      wpi::UnpackStruct<wpimath::Translation2d, kFrontRightOff>(data),
      wpi::UnpackStruct<wpimath::Translation2d, kRearLeftOff>(data),
      wpi::UnpackStruct<wpimath::Translation2d, kRearRightOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpimath::MecanumDriveKinematics& value) {
  wpi::PackStruct<kFrontLeftOff>(data, value.GetFrontLeft());
  wpi::PackStruct<kFrontRightOff>(data, value.GetFrontRight());
  wpi::PackStruct<kRearLeftOff>(data, value.GetRearLeft());
  wpi::PackStruct<kRearRightOff>(data, value.GetRearRight());
}
