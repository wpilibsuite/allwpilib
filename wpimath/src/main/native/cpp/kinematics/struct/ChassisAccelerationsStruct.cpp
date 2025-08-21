// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/struct/ChassisAccelerationsStruct.h"

#include <wpi/struct/Struct.h>

#include "frc/kinematics/ChassisAccelerations.h"

frc::ChassisAccelerations wpi::Struct<frc::ChassisAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kAxOff = 0;
  constexpr size_t kAyOff = kAxOff + 8;
  constexpr size_t kAlphaOff = kAyOff + 8;
  return frc::ChassisAccelerations{
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kAxOff>(data)},
      units::meters_per_second_squared_t{
          wpi::UnpackStruct<double, kAyOff>(data)},
      units::radians_per_second_squared_t{
          wpi::UnpackStruct<double, kAlphaOff>(data)},
  };
}

void wpi::Struct<frc::ChassisAccelerations>::Pack(
    std::span<uint8_t> data, const frc::ChassisAccelerations& value) {
  constexpr size_t kAxOff = 0;
  constexpr size_t kAyOff = kAxOff + 8;
  constexpr size_t kAlphaOff = kAyOff + 8;
  wpi::PackStruct<kAxOff>(data, value.ax.value());
  wpi::PackStruct<kAyOff>(data, value.ay.value());
  wpi::PackStruct<kAlphaOff>(data, value.alpha.value());
}
