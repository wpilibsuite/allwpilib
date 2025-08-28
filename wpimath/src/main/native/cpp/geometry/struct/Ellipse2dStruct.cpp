// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/struct/Ellipse2dStruct.h"

namespace {
constexpr size_t kCenterOff = 0;
constexpr size_t kXSemiAxisOff =
    kCenterOff + wpi::GetStructSize<wpimath::Pose2d>();
constexpr size_t kYSemiAxisOff = kXSemiAxisOff + 8;
}  // namespace

using StructType = wpi::Struct<wpimath::Ellipse2d>;

wpimath::Ellipse2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpimath::Ellipse2d{
      wpi::UnpackStruct<wpimath::Pose2d, kCenterOff>(data),
      units::meter_t{wpi::UnpackStruct<double, kXSemiAxisOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kYSemiAxisOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpimath::Ellipse2d& value) {
  wpi::PackStruct<kCenterOff>(data, value.Center());
  wpi::PackStruct<kXSemiAxisOff>(data, value.XSemiAxis().value());
  wpi::PackStruct<kYSemiAxisOff>(data, value.YSemiAxis().value());
}
