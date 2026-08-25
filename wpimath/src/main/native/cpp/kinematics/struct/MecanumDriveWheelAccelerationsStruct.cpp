// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveWheelAccelerationsStruct.hpp"

#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"
#include "wpi/util/struct/Struct.hpp"

wpi::math::MecanumDriveWheelAccelerations
wpi::util::Struct<wpi::math::MecanumDriveWheelAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t FRONT_LEFT_OFF = 0;
  constexpr size_t FRONT_RIGHT_OFF = FRONT_LEFT_OFF + 8;
  constexpr size_t REAR_LEFT_OFF = FRONT_RIGHT_OFF + 8;
  constexpr size_t REAR_RIGHT_OFF = REAR_LEFT_OFF + 8;
  return wpi::math::MecanumDriveWheelAccelerations{
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, FRONT_LEFT_OFF>(data)},
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, FRONT_RIGHT_OFF>(data)},
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, REAR_LEFT_OFF>(data)},
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, REAR_RIGHT_OFF>(data)},
  };
}

void wpi::util::Struct<wpi::math::MecanumDriveWheelAccelerations>::Pack(
    std::span<uint8_t> data,
    const wpi::math::MecanumDriveWheelAccelerations& value) {
  constexpr size_t FRONT_LEFT_OFF = 0;
  constexpr size_t FRONT_RIGHT_OFF = FRONT_LEFT_OFF + 8;
  constexpr size_t REAR_LEFT_OFF = FRONT_RIGHT_OFF + 8;
  constexpr size_t REAR_RIGHT_OFF = REAR_LEFT_OFF + 8;
  wpi::util::PackStruct<FRONT_LEFT_OFF>(data, value.frontLeft.value());
  wpi::util::PackStruct<FRONT_RIGHT_OFF>(data, value.frontRight.value());
  wpi::util::PackStruct<REAR_LEFT_OFF>(data, value.rearLeft.value());
  wpi::util::PackStruct<REAR_RIGHT_OFF>(data, value.rearRight.value());
}
