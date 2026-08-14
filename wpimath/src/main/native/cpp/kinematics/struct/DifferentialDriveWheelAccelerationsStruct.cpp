// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/DifferentialDriveWheelAccelerationsStruct.hpp"

#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"
#include "wpi/util/struct/Struct.hpp"

wpi::math::DifferentialDriveWheelAccelerations
wpi::util::Struct<wpi::math::DifferentialDriveWheelAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kLeftOff = 0;
  constexpr size_t kRightOff = kLeftOff + 8;
  return wpi::math::DifferentialDriveWheelAccelerations{
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kLeftOff>(data)},
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kRightOff>(data)},
  };
}

void wpi::util::Struct<wpi::math::DifferentialDriveWheelAccelerations>::Pack(
    std::span<uint8_t> data,
    const wpi::math::DifferentialDriveWheelAccelerations& value) {
  constexpr size_t kLeftOff = 0;
  constexpr size_t kRightOff = kLeftOff + 8;
  wpi::util::PackStruct<kLeftOff>(data, value.left.value());
  wpi::util::PackStruct<kRightOff>(data, value.right.value());
}
