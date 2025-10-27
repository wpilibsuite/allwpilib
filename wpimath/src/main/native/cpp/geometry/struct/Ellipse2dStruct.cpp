// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Ellipse2dStruct.hpp"

namespace {
constexpr size_t kCenterOff = 0;
constexpr size_t kXSemiAxisOff =
    kCenterOff + wpi::util::GetStructSize<wpi::math::Pose2d>();
constexpr size_t kYSemiAxisOff = kXSemiAxisOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Ellipse2d>;

wpi::math::Ellipse2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Ellipse2d{
      wpi::util::UnpackStruct<wpi::math::Pose2d, kCenterOff>(data),
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kXSemiAxisOff>(data)},
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kYSemiAxisOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Ellipse2d& value) {
  wpi::util::PackStruct<kCenterOff>(data, value.Center());
  wpi::util::PackStruct<kXSemiAxisOff>(data, value.XSemiAxis().value());
  wpi::util::PackStruct<kYSemiAxisOff>(data, value.YSemiAxis().value());
}
