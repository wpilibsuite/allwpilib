// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Translation2dStruct.hpp"

namespace {
constexpr size_t kXOff = 0;
constexpr size_t kYOff = kXOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Translation2d>;

wpi::math::Translation2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Translation2d{
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kXOff>(data)},
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kYOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Translation2d& value) {
  wpi::util::PackStruct<kXOff>(data, value.X().value());
  wpi::util::PackStruct<kYOff>(data, value.Y().value());
}
