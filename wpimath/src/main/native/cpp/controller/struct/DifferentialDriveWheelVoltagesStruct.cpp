// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/DifferentialDriveWheelVoltagesStruct.h"

namespace {
constexpr size_t LEFT_OFF = 0;
constexpr size_t RIGHT_OFF = LEFT_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::DifferentialDriveWheelVoltages>;

frc::DifferentialDriveWheelVoltages StructType::Unpack(
    std::span<const uint8_t> data) {
  return frc::DifferentialDriveWheelVoltages{
      units::volt_t{wpi::UnpackStruct<double, LEFT_OFF>(data)},
      units::volt_t{wpi::UnpackStruct<double, RIGHT_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::DifferentialDriveWheelVoltages& value) {
  wpi::PackStruct<LEFT_OFF>(data, value.left.value());
  wpi::PackStruct<RIGHT_OFF>(data, value.right.value());
}
