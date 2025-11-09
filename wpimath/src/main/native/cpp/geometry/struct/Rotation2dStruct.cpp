// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Rotation2dStruct.hpp"

namespace {
constexpr size_t kValueOff = 0;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Rotation2d>;

wpi::math::Rotation2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Rotation2d{
      wpi::units::radian_t{wpi::util::UnpackStruct<double, kValueOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Rotation2d& value) {
  wpi::util::PackStruct<kValueOff>(data, value.Radians().value());
}
