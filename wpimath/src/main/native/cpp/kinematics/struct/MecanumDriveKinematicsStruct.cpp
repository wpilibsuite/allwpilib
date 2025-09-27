// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/MecanumDriveKinematicsStruct.hpp"

namespace {
constexpr size_t kFrontLeftOff = 0;
constexpr size_t kFrontRightOff =
    kFrontLeftOff + wpi::util::GetStructSize<wpi::math::Translation2d>();
constexpr size_t kRearLeftOff =
    kFrontRightOff + wpi::util::GetStructSize<wpi::math::Translation2d>();
constexpr size_t kRearRightOff =
    kRearLeftOff + wpi::util::GetStructSize<wpi::math::Translation2d>();
}  // namespace

using StructType = wpi::util::Struct<wpi::math::MecanumDriveKinematics>;

wpi::math::MecanumDriveKinematics StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::MecanumDriveKinematics{
      wpi::util::UnpackStruct<wpi::math::Translation2d, kFrontLeftOff>(data),
      wpi::util::UnpackStruct<wpi::math::Translation2d, kFrontRightOff>(data),
      wpi::util::UnpackStruct<wpi::math::Translation2d, kRearLeftOff>(data),
      wpi::util::UnpackStruct<wpi::math::Translation2d, kRearRightOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::MecanumDriveKinematics& value) {
  wpi::util::PackStruct<kFrontLeftOff>(data, value.GetFrontLeft());
  wpi::util::PackStruct<kFrontRightOff>(data, value.GetFrontRight());
  wpi::util::PackStruct<kRearLeftOff>(data, value.GetRearLeft());
  wpi::util::PackStruct<kRearRightOff>(data, value.GetRearRight());
}
