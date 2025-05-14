// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Ellipse2dStruct.h"

namespace {
constexpr size_t CENTER_OFF = 0;
constexpr size_t X_SEMI_AXIS_OFF =
    CENTER_OFF + wpi::GetStructSize<frc::Pose2d>();
constexpr size_t Y_SEMI_AXIS_OFF = X_SEMI_AXIS_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Ellipse2d>;

frc::Ellipse2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Ellipse2d{
      wpi::UnpackStruct<frc::Pose2d, CENTER_OFF>(data),
      units::meter_t{wpi::UnpackStruct<double, X_SEMI_AXIS_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, Y_SEMI_AXIS_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Ellipse2d& value) {
  wpi::PackStruct<CENTER_OFF>(data, value.Center());
  wpi::PackStruct<X_SEMI_AXIS_OFF>(data, value.XSemiAxis().value());
  wpi::PackStruct<Y_SEMI_AXIS_OFF>(data, value.YSemiAxis().value());
}
