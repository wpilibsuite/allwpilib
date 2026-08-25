// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Translation3dStruct.hpp"

namespace {
constexpr size_t X_OFF = 0;
constexpr size_t Y_OFF = X_OFF + 8;
constexpr size_t Z_OFF = Y_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Translation3d>;

wpi::math::Translation3d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Translation3d{
      wpi::units::meters<>{wpi::util::UnpackStruct<double, X_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, Y_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, Z_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Translation3d& value) {
  wpi::util::PackStruct<X_OFF>(data, value.X().value());
  wpi::util::PackStruct<Y_OFF>(data, value.Y().value());
  wpi::util::PackStruct<Z_OFF>(data, value.Z().value());
}
