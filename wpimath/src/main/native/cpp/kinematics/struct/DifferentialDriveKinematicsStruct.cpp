// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/kinematics/struct/DifferentialDriveKinematicsStruct.h>

namespace {
constexpr size_t kTrackwidthOff = 0;
}  // namespace

using StructType = wpi::Struct<wpi::math::DifferentialDriveKinematics>;

wpi::math::DifferentialDriveKinematics StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::DifferentialDriveKinematics{
      units::meter_t{wpi::UnpackStruct<double, kTrackwidthOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DifferentialDriveKinematics& value) {
  wpi::PackStruct<kTrackwidthOff>(data, value.trackwidth.value());
}
