// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Rectangle2dStruct.h"

namespace {
constexpr size_t CENTER_OFF = 0;
constexpr size_t X_WIDTH_OFF = CENTER_OFF + wpi::GetStructSize<frc::Pose2d>();
constexpr size_t Y_WIDTH_OFF = X_WIDTH_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Rectangle2d>;

frc::Rectangle2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Rectangle2d{
      wpi::UnpackStruct<frc::Pose2d, CENTER_OFF>(data),
      units::meter_t{wpi::UnpackStruct<double, X_WIDTH_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, Y_WIDTH_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Rectangle2d& value) {
  wpi::PackStruct<CENTER_OFF>(data, value.Center());
  wpi::PackStruct<X_WIDTH_OFF>(data, value.XWidth().value());
  wpi::PackStruct<Y_WIDTH_OFF>(data, value.YWidth().value());
}
