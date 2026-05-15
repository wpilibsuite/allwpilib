// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/SwerveModuleVelocityStruct.hpp"

namespace {
constexpr size_t kVelocityOff = 0;
constexpr size_t kAngleOff = kVelocityOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::SwerveModuleVelocity>;

wpi::math::SwerveModuleVelocity StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::SwerveModuleVelocity{
      wpi::units::meters_per_second_t{
          wpi::util::UnpackStruct<double, kVelocityOff>(data)},
      wpi::util::UnpackStruct<wpi::math::Rotation2d, kAngleOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::SwerveModuleVelocity& value) {
  wpi::util::PackStruct<kVelocityOff>(data, value.velocity.value());
  wpi::util::PackStruct<kAngleOff>(data, value.angle);
}
