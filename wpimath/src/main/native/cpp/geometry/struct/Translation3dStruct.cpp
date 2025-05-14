// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Translation3dStruct.h"

namespace {
constexpr size_t X_OFF = 0;
constexpr size_t Y_OFF = X_OFF + 8;
constexpr size_t Z_OFF = Y_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Translation3d>;

frc::Translation3d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Translation3d{
      units::meter_t{wpi::UnpackStruct<double, X_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, Y_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, Z_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::Translation3d& value) {
  wpi::PackStruct<X_OFF>(data, value.X().value());
  wpi::PackStruct<Y_OFF>(data, value.Y().value());
  wpi::PackStruct<Z_OFF>(data, value.Z().value());
}
