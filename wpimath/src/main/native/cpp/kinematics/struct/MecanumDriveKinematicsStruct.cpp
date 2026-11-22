// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveKinematicsStruct.hpp"

namespace {
constexpr size_t FRONT_LEFT_OFF = 0;
constexpr size_t FRONT_RIGHT_OFF =
    FRONT_LEFT_OFF + wpi::util::GetStructSize<wpi::math::Translation2d>();
constexpr size_t REAR_LEFT_OFF =
    FRONT_RIGHT_OFF + wpi::util::GetStructSize<wpi::math::Translation2d>();
constexpr size_t REAR_RIGHT_OFF =
    REAR_LEFT_OFF + wpi::util::GetStructSize<wpi::math::Translation2d>();
}  // namespace

using StructType = wpi::util::Struct<wpi::math::MecanumDriveKinematics>;

wpi::math::MecanumDriveKinematics StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::MecanumDriveKinematics{
      wpi::util::UnpackStruct<wpi::math::Translation2d, FRONT_LEFT_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::Translation2d, FRONT_RIGHT_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::Translation2d, REAR_LEFT_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::Translation2d, REAR_RIGHT_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::MecanumDriveKinematics& value) {
  wpi::util::PackStruct<FRONT_LEFT_OFF>(data, value.GetFrontLeft());
  wpi::util::PackStruct<FRONT_RIGHT_OFF>(data, value.GetFrontRight());
  wpi::util::PackStruct<REAR_LEFT_OFF>(data, value.GetRearLeft());
  wpi::util::PackStruct<REAR_RIGHT_OFF>(data, value.GetRearRight());
}
