// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Translation2dStruct.hpp"

namespace {
constexpr size_t X_OFF = 0;
constexpr size_t Y_OFF = X_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Translation2d>;

wpi::math::Translation2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Translation2d{
      wpi::units::meters<>{wpi::util::UnpackStruct<double, X_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, Y_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Translation2d& value) {
  wpi::util::PackStruct<X_OFF>(data, value.X().value());
  wpi::util::PackStruct<Y_OFF>(data, value.Y().value());
}
