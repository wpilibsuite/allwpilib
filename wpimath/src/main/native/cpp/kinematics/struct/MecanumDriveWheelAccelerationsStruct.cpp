// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveWheelAccelerationsStruct.hpp"

#include <wpi/util/struct/Struct.hpp>

#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"

wpi::math::MecanumDriveWheelAccelerations
wpi::util::Struct<wpi::math::MecanumDriveWheelAccelerations>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kFrontLeftOff = 0;
  constexpr size_t kFrontRightOff = kFrontLeftOff + 8;
  constexpr size_t kRearLeftOff = kFrontRightOff + 8;
  constexpr size_t kRearRightOff = kRearLeftOff + 8;
  return wpi::math::MecanumDriveWheelAccelerations{
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kFrontLeftOff>(data)},
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kFrontRightOff>(data)},
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kRearLeftOff>(data)},
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kRearRightOff>(data)},
  };
}

void wpi::util::Struct<wpi::math::MecanumDriveWheelAccelerations>::Pack(
    std::span<uint8_t> data, const wpi::math::MecanumDriveWheelAccelerations& value) {
  constexpr size_t kFrontLeftOff = 0;
  constexpr size_t kFrontRightOff = kFrontLeftOff + 8;
  constexpr size_t kRearLeftOff = kFrontRightOff + 8;
  constexpr size_t kRearRightOff = kRearLeftOff + 8;
  wpi::util::PackStruct<kFrontLeftOff>(data, value.frontLeft.value());
  wpi::util::PackStruct<kFrontRightOff>(data, value.frontRight.value());
  wpi::util::PackStruct<kRearLeftOff>(data, value.rearLeft.value());
  wpi::util::PackStruct<kRearRightOff>(data, value.rearRight.value());
}
