// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/ChassisSpeedsStruct.hpp"

namespace {
constexpr size_t kVxOff = 0;
constexpr size_t kVyOff = kVxOff + 8;
constexpr size_t kOmegaOff = kVyOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::ChassisSpeeds>;

wpi::math::ChassisSpeeds StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::ChassisSpeeds{
      wpi::units::meters_per_second_t{wpi::util::UnpackStruct<double, kVxOff>(data)},
      wpi::units::meters_per_second_t{wpi::util::UnpackStruct<double, kVyOff>(data)},
      wpi::units::radians_per_second_t{wpi::util::UnpackStruct<double, kOmegaOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::ChassisSpeeds& value) {
  wpi::util::PackStruct<kVxOff>(data, value.vx.value());
  wpi::util::PackStruct<kVyOff>(data, value.vy.value());
  wpi::util::PackStruct<kOmegaOff>(data, value.omega.value());
}
