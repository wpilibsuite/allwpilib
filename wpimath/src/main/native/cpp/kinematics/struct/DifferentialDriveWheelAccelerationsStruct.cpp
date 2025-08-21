// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/DifferentialDriveWheelAccelerationsStruct.h"

#include <wpi/struct/Struct.h>

#include "frc/kinematics/DifferentialDriveWheelAccelerations.h"

frc::DifferentialDriveWheelAccelerations wpi::Struct<frc::DifferentialDriveWheelAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kLeftOff = 0;
  constexpr size_t kRightOff = kLeftOff + 8;
  return frc::DifferentialDriveWheelAccelerations{
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kLeftOff>(data)},
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kRightOff>(data)},
  };
}

void wpi::Struct<frc::DifferentialDriveWheelAccelerations>::Pack(
    std::span<uint8_t> data, const frc::DifferentialDriveWheelAccelerations& value) {
  constexpr size_t kLeftOff = 0;
  constexpr size_t kRightOff = kLeftOff + 8;
  wpi::PackStruct<kLeftOff>(data, value.left.value());
  wpi::PackStruct<kRightOff>(data, value.right.value());
}
