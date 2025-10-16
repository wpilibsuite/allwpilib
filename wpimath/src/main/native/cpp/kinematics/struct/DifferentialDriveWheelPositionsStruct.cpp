// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/DifferentialDriveWheelPositionsStruct.hpp"

namespace {
constexpr size_t kLeftOff = 0;
constexpr size_t kRightOff = kLeftOff + 8;
}  // namespace

using StructType =
    wpi::util::Struct<wpi::math::DifferentialDriveWheelPositions>;

wpi::math::DifferentialDriveWheelPositions StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::DifferentialDriveWheelPositions{
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kLeftOff>(data)},
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kRightOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DifferentialDriveWheelPositions& value) {
  wpi::util::PackStruct<kLeftOff>(data, value.left.value());
  wpi::util::PackStruct<kRightOff>(data, value.right.value());
}
