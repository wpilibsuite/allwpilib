// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/MecanumDriveKinematicsStruct.h"

namespace {
constexpr size_t FRONT_LEFT_OFF = 0;
constexpr size_t FRONT_RIGHT_OFF =
    FRONT_LEFT_OFF + wpi::GetStructSize<frc::Translation2d>();
constexpr size_t REAR_LEFT_OFF =
    FRONT_RIGHT_OFF + wpi::GetStructSize<frc::Translation2d>();
constexpr size_t REAR_RIGHT_OFF =
    REAR_LEFT_OFF + wpi::GetStructSize<frc::Translation2d>();
}  // namespace

using StructType = wpi::Struct<frc::MecanumDriveKinematics>;

frc::MecanumDriveKinematics StructType::Unpack(std::span<const uint8_t> data) {
  return frc::MecanumDriveKinematics{
      wpi::UnpackStruct<frc::Translation2d, FRONT_LEFT_OFF>(data),
      wpi::UnpackStruct<frc::Translation2d, FRONT_RIGHT_OFF>(data),
      wpi::UnpackStruct<frc::Translation2d, REAR_LEFT_OFF>(data),
      wpi::UnpackStruct<frc::Translation2d, REAR_RIGHT_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::MecanumDriveKinematics& value) {
  wpi::PackStruct<FRONT_LEFT_OFF>(data, value.GetFrontLeft());
  wpi::PackStruct<FRONT_RIGHT_OFF>(data, value.GetFrontRight());
  wpi::PackStruct<REAR_LEFT_OFF>(data, value.GetRearLeft());
  wpi::PackStruct<REAR_RIGHT_OFF>(data, value.GetRearRight());
}
