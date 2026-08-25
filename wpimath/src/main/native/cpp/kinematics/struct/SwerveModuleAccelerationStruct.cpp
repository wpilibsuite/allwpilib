// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/SwerveModuleAccelerationStruct.hpp"

#include "wpi/math/kinematics/SwerveModuleAcceleration.hpp"
#include "wpi/util/struct/Struct.hpp"

wpi::math::SwerveModuleAcceleration
wpi::util::Struct<wpi::math::SwerveModuleAcceleration>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t ACCELERATION_OFF = 0;
  constexpr size_t ANGLE_OFF = ACCELERATION_OFF + 8;
  return wpi::math::SwerveModuleAcceleration{
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, ACCELERATION_OFF>(data)},
      wpi::util::UnpackStruct<wpi::math::Rotation2d, ANGLE_OFF>(data)};
}

void wpi::util::Struct<wpi::math::SwerveModuleAcceleration>::Pack(
    std::span<uint8_t> data, const wpi::math::SwerveModuleAcceleration& value) {
  constexpr size_t ACCELERATION_OFF = 0;
  constexpr size_t ANGLE_OFF = ACCELERATION_OFF + 8;
  wpi::util::PackStruct<ACCELERATION_OFF>(data, value.acceleration.value());
  wpi::util::PackStruct<ANGLE_OFF>(data, value.angle);
}
