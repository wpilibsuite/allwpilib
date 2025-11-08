// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Translation3dStruct.hpp"

namespace {
constexpr size_t kXOff = 0;
constexpr size_t kYOff = kXOff + 8;
constexpr size_t kZOff = kYOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::Translation3d>;

frc::Translation3d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Translation3d{
      units::meter_t{wpi::UnpackStruct<double, kXOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kYOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kZOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::Translation3d& value) {
  wpi::PackStruct<kXOff>(data, value.X().value());
  wpi::PackStruct<kYOff>(data, value.Y().value());
  wpi::PackStruct<kZOff>(data, value.Z().value());
}
