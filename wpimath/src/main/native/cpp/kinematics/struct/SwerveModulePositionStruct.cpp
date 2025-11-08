// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/SwerveModulePositionStruct.hpp"

namespace {
constexpr size_t kDistanceOff = 0;
constexpr size_t kAngleOff = kDistanceOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::SwerveModulePosition>;

wpi::math::SwerveModulePosition StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::SwerveModulePosition{
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kDistanceOff>(data)},
      wpi::util::UnpackStruct<wpi::math::Rotation2d, kAngleOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::SwerveModulePosition& value) {
  wpi::util::PackStruct<kDistanceOff>(data, value.distance.value());
  wpi::util::PackStruct<kAngleOff>(data, value.angle);
}
