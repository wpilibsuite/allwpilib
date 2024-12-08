// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/DifferentialDriveKinematicsStruct.h"

namespace {
constexpr size_t kTrackwidthOff = 0;
}  // namespace

using StructType = wpi::Struct<frc::DifferentialDriveKinematics>;

frc::DifferentialDriveKinematics StructType::Unpack(
    std::span<const uint8_t> data) {
  return frc::DifferentialDriveKinematics{
      units::meter_t{wpi::UnpackStruct<double, kTrackwidthOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::DifferentialDriveKinematics& value) {
  wpi::PackStruct<kTrackwidthOff>(data, value.trackwidth.value());
}
