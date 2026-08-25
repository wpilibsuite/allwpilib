// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/SwerveModuleVelocityStruct.hpp"

namespace {
constexpr size_t VELOCITY_OFF = 0;
constexpr size_t ANGLE_OFF = VELOCITY_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::SwerveModuleVelocity>;

wpi::math::SwerveModuleVelocity StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::SwerveModuleVelocity{
      wpi::units::meters_per_second<>{
          wpi::util::UnpackStruct<double, VELOCITY_OFF>(data)},
      wpi::util::UnpackStruct<wpi::math::Rotation2d, ANGLE_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::SwerveModuleVelocity& value) {
  wpi::util::PackStruct<VELOCITY_OFF>(data, value.velocity.value());
  wpi::util::PackStruct<ANGLE_OFF>(data, value.angle);
}
