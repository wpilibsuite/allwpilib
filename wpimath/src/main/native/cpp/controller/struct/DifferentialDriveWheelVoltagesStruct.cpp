// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/struct/DifferentialDriveWheelVoltagesStruct.hpp"

namespace {
constexpr size_t LEFT_OFF = 0;
constexpr size_t RIGHT_OFF = LEFT_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::DifferentialDriveWheelVoltages>;

wpi::math::DifferentialDriveWheelVoltages StructType::Unpack(
    std::span<const uint8_t> data) {
  return wpi::math::DifferentialDriveWheelVoltages{
      wpi::units::volts<>{wpi::util::UnpackStruct<double, LEFT_OFF>(data)},
      wpi::units::volts<>{wpi::util::UnpackStruct<double, RIGHT_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DifferentialDriveWheelVoltages& value) {
  wpi::util::PackStruct<LEFT_OFF>(data, value.left.value());
  wpi::util::PackStruct<RIGHT_OFF>(data, value.right.value());
}
