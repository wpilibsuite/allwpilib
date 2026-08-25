// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/DifferentialDriveWheelAccelerationsStruct.hpp"

#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"
#include "wpi/util/struct/Struct.hpp"

wpi::math::DifferentialDriveWheelAccelerations
wpi::util::Struct<wpi::math::DifferentialDriveWheelAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t LEFT_OFF = 0;
  constexpr size_t RIGHT_OFF = LEFT_OFF + 8;
  return wpi::math::DifferentialDriveWheelAccelerations{
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, LEFT_OFF>(data)},
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, RIGHT_OFF>(data)},
  };
}

void wpi::util::Struct<wpi::math::DifferentialDriveWheelAccelerations>::Pack(
    std::span<uint8_t> data,
    const wpi::math::DifferentialDriveWheelAccelerations& value) {
  constexpr size_t LEFT_OFF = 0;
  constexpr size_t RIGHT_OFF = LEFT_OFF + 8;
  wpi::util::PackStruct<LEFT_OFF>(data, value.left.value());
  wpi::util::PackStruct<RIGHT_OFF>(data, value.right.value());
}
