// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveWheelPositionsStruct.hpp"

namespace {
constexpr size_t FRONT_LEFT_OFF = 0;
constexpr size_t FRONT_RIGHT_OFF = FRONT_LEFT_OFF + 8;
constexpr size_t REAR_LEFT_OFF = FRONT_RIGHT_OFF + 8;
constexpr size_t REAR_RIGHT_OFF = REAR_LEFT_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::MecanumDriveWheelPositions>;

wpi::math::MecanumDriveWheelPositions StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::MecanumDriveWheelPositions{
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, FRONT_LEFT_OFF>(data)},
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, FRONT_RIGHT_OFF>(data)},
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, REAR_LEFT_OFF>(data)},
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, REAR_RIGHT_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::MecanumDriveWheelPositions& value) {
  wpi::util::PackStruct<FRONT_LEFT_OFF>(data, value.frontLeft.value());
  wpi::util::PackStruct<FRONT_RIGHT_OFF>(data, value.frontRight.value());
  wpi::util::PackStruct<REAR_LEFT_OFF>(data, value.rearLeft.value());
  wpi::util::PackStruct<REAR_RIGHT_OFF>(data, value.rearRight.value());
}
