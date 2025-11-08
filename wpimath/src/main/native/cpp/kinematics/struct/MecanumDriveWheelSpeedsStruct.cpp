// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveWheelSpeedsStruct.hpp"

namespace {
constexpr size_t kFrontLeftOff = 0;
constexpr size_t kFrontRightOff = kFrontLeftOff + 8;
constexpr size_t kRearLeftOff = kFrontRightOff + 8;
constexpr size_t kRearRightOff = kRearLeftOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::MecanumDriveWheelSpeeds>;

wpi::math::MecanumDriveWheelSpeeds StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::MecanumDriveWheelSpeeds{
      wpi::units::meters_per_second_t{
          wpi::util::UnpackStruct<double, kFrontLeftOff>(data)},
      wpi::units::meters_per_second_t{
          wpi::util::UnpackStruct<double, kFrontRightOff>(data)},
      wpi::units::meters_per_second_t{
          wpi::util::UnpackStruct<double, kRearLeftOff>(data)},
      wpi::units::meters_per_second_t{
          wpi::util::UnpackStruct<double, kRearRightOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::MecanumDriveWheelSpeeds& value) {
  wpi::util::PackStruct<kFrontLeftOff>(data, value.frontLeft.value());
  wpi::util::PackStruct<kFrontRightOff>(data, value.frontRight.value());
  wpi::util::PackStruct<kRearLeftOff>(data, value.rearLeft.value());
  wpi::util::PackStruct<kRearRightOff>(data, value.rearRight.value());
}
