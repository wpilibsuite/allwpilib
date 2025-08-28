// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/struct/Rectangle2dStruct.hpp>

namespace {
constexpr size_t kCenterOff = 0;
constexpr size_t kXWidthOff =
    kCenterOff + wpi::GetStructSize<wpi::math::Pose2d>();
constexpr size_t kYWidthOff = kXWidthOff + 8;
}  // namespace

using StructType = wpi::Struct<wpi::math::Rectangle2d>;

wpi::math::Rectangle2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Rectangle2d{
      wpi::UnpackStruct<wpi::math::Pose2d, kCenterOff>(data),
      units::meter_t{wpi::UnpackStruct<double, kXWidthOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kYWidthOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Rectangle2d& value) {
  wpi::PackStruct<kCenterOff>(data, value.Center());
  wpi::PackStruct<kXWidthOff>(data, value.XWidth().value());
  wpi::PackStruct<kYWidthOff>(data, value.YWidth().value());
}
