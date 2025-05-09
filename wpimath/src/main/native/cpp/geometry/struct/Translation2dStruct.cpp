// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Translation2dStruct.h"

namespace {
constexpr size_t X_OFF = 0;
constexpr size_t Y_OFF = X_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Translation2d>;

frc::Translation2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Translation2d{
      units::meter_t{wpi::UnpackStruct<double, X_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, Y_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::Translation2d& value) {
  wpi::PackStruct<X_OFF>(data, value.X().value());
  wpi::PackStruct<Y_OFF>(data, value.Y().value());
}
