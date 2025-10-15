// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveKinematicsStruct.hpp"

namespace {
constexpr size_t kFrontLeftOff = 0;
constexpr size_t kFrontRightOff =
    kFrontLeftOff + wpi::GetStructSize<frc::Translation2d>();
constexpr size_t kRearLeftOff =
    kFrontRightOff + wpi::GetStructSize<frc::Translation2d>();
constexpr size_t kRearRightOff =
    kRearLeftOff + wpi::GetStructSize<frc::Translation2d>();
}  // namespace

using StructType = wpi::Struct<frc::MecanumDriveKinematics>;

frc::MecanumDriveKinematics StructType::Unpack(std::span<const uint8_t> data) {
  return frc::MecanumDriveKinematics{
      wpi::UnpackStruct<frc::Translation2d, kFrontLeftOff>(data),
      wpi::UnpackStruct<frc::Translation2d, kFrontRightOff>(data),
      wpi::UnpackStruct<frc::Translation2d, kRearLeftOff>(data),
      wpi::UnpackStruct<frc::Translation2d, kRearRightOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::MecanumDriveKinematics& value) {
  wpi::PackStruct<kFrontLeftOff>(data, value.GetFrontLeft());
  wpi::PackStruct<kFrontRightOff>(data, value.GetFrontRight());
  wpi::PackStruct<kRearLeftOff>(data, value.GetRearLeft());
  wpi::PackStruct<kRearRightOff>(data, value.GetRearRight());
}
