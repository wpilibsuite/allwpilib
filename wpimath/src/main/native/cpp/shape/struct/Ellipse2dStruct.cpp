// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/shape/struct/Ellipse2dStruct.hpp"

namespace {
constexpr size_t CENTER_OFF = 0;
constexpr size_t X_SEMI_AXIS_OFF =
    CENTER_OFF + wpi::util::GetStructSize<wpi::math::Pose2d>();
constexpr size_t Y_SEMI_AXIS_OFF = X_SEMI_AXIS_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Ellipse2d>;

wpi::math::Ellipse2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Ellipse2d{
      wpi::util::UnpackStruct<wpi::math::Pose2d, CENTER_OFF>(data),
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, X_SEMI_AXIS_OFF>(data)},
      wpi::units::meters<>{
          wpi::util::UnpackStruct<double, Y_SEMI_AXIS_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Ellipse2d& value) {
  wpi::util::PackStruct<CENTER_OFF>(data, value.Center());
  wpi::util::PackStruct<X_SEMI_AXIS_OFF>(data, value.XSemiAxis().value());
  wpi::util::PackStruct<Y_SEMI_AXIS_OFF>(data, value.YSemiAxis().value());
}
