// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/ChassisSpeedsStruct.h"

namespace {
constexpr size_t VX_OFF = 0;
constexpr size_t VY_OFF = VX_OFF + 8;
constexpr size_t OMEGA_OFF = VY_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::ChassisSpeeds>;

frc::ChassisSpeeds StructType::Unpack(std::span<const uint8_t> data) {
  return frc::ChassisSpeeds{
      units::meters_per_second_t{wpi::UnpackStruct<double, VX_OFF>(data)},
      units::meters_per_second_t{wpi::UnpackStruct<double, VY_OFF>(data)},
      units::radians_per_second_t{wpi::UnpackStruct<double, OMEGA_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::ChassisSpeeds& value) {
  wpi::PackStruct<VX_OFF>(data, value.vx.value());
  wpi::PackStruct<VY_OFF>(data, value.vy.value());
  wpi::PackStruct<OMEGA_OFF>(data, value.omega.value());
}
