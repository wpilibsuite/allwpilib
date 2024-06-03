// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Rectangle2dStruct.h"

namespace {
constexpr size_t kCenterOff = 0;
constexpr size_t kXWidthOff = kCenterOff + wpi::GetStructSize<frc::Pose2d>();
constexpr size_t kYWidthOff = kXWidthOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::Rectangle2d>;

frc::Rectangle2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Rectangle2d{
      wpi::UnpackStruct<frc::Pose2d, kCenterOff>(data),
      units::meter_t{wpi::UnpackStruct<double, kXWidthOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kYWidthOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Rectangle2d& value) {
  wpi::PackStruct<kCenterOff>(data, value.Center());
  wpi::PackStruct<kXWidthOff>(data, value.XWidth().value());
  wpi::PackStruct<kYWidthOff>(data, value.YWidth().value());
}
