// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/SwerveModuleAccelerationsStruct.h"

#include <wpi/struct/Struct.h>

#include "frc/kinematics/SwerveModuleAccelerations.h"

frc::SwerveModuleAccelerations wpi::Struct<frc::SwerveModuleAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kAccelerationOff = 0;
  constexpr size_t kAngleOff = kAccelerationOff + 8;
  return frc::SwerveModuleAccelerations{
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kAccelerationOff>(data)},
      frc::Rotation2d{units::radian_t{
          wpi::UnpackStruct<double, kAngleOff>(data)}},
  };
}

void wpi::Struct<frc::SwerveModuleAccelerations>::Pack(
    std::span<uint8_t> data, const frc::SwerveModuleAccelerations& value) {
  constexpr size_t kAccelerationOff = 0;
  constexpr size_t kAngleOff = kAccelerationOff + 8;
  wpi::PackStruct<kAccelerationOff>(data, value.acceleration.value());
  wpi::PackStruct<kAngleOff>(data, value.angle.Radians().value());
}
