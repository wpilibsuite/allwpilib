// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/DifferentialDriveKinematicsStruct.hpp"

namespace {
constexpr size_t TRACKWIDTH_OFF = 0;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::DifferentialDriveKinematics>;

wpi::math::DifferentialDriveKinematics StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::DifferentialDriveKinematics{
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, TRACKWIDTH_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DifferentialDriveKinematics& value) {
  wpi::util::PackStruct<TRACKWIDTH_OFF>(data, value.trackwidth.value());
}
