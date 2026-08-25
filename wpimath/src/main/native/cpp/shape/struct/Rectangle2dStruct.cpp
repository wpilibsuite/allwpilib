// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/shape/struct/Rectangle2dStruct.hpp"

namespace {
constexpr size_t CENTER_OFF = 0;
constexpr size_t X_WIDTH_OFF =
    CENTER_OFF + wpi::util::GetStructSize<wpi::math::Pose2d>();
constexpr size_t Y_WIDTH_OFF = X_WIDTH_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Rectangle2d>;

wpi::math::Rectangle2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Rectangle2d{
      wpi::util::UnpackStruct<wpi::math::Pose2d, CENTER_OFF>(data),
      wpi::units::meters<>{wpi::util::UnpackStruct<double, X_WIDTH_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, Y_WIDTH_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Rectangle2d& value) {
  wpi::util::PackStruct<CENTER_OFF>(data, value.Center());
  wpi::util::PackStruct<X_WIDTH_OFF>(data, value.XWidth().value());
  wpi::util::PackStruct<Y_WIDTH_OFF>(data, value.YWidth().value());
}
