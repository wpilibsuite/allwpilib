// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/MecanumDriveWheelAccelerationsStruct.h"

#include <wpi/struct/Struct.h>

#include "frc/kinematics/MecanumDriveWheelAccelerations.h"

frc::MecanumDriveWheelAccelerations wpi::Struct<frc::MecanumDriveWheelAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kFrontLeftOff = 0;
  constexpr size_t kFrontRightOff = kFrontLeftOff + 8;
  constexpr size_t kRearLeftOff = kFrontRightOff + 8;
  constexpr size_t kRearRightOff = kRearLeftOff + 8;
  return frc::MecanumDriveWheelAccelerations{
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kFrontLeftOff>(data)},
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kFrontRightOff>(data)},
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kRearLeftOff>(data)},
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kRearRightOff>(data)},
  };
}

void wpi::Struct<frc::MecanumDriveWheelAccelerations>::Pack(
    std::span<uint8_t> data, const frc::MecanumDriveWheelAccelerations& value) {
  constexpr size_t kFrontLeftOff = 0;
  constexpr size_t kFrontRightOff = kFrontLeftOff + 8;
  constexpr size_t kRearLeftOff = kFrontRightOff + 8;
  constexpr size_t kRearRightOff = kRearLeftOff + 8;
  wpi::PackStruct<kFrontLeftOff>(data, value.frontLeft.value());
  wpi::PackStruct<kFrontRightOff>(data, value.frontRight.value());
  wpi::PackStruct<kRearLeftOff>(data, value.rearLeft.value());
  wpi::PackStruct<kRearRightOff>(data, value.rearRight.value());
}
