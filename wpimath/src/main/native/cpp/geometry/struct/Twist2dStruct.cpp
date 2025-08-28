// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/struct/Twist2dStruct.hpp>

namespace {
constexpr size_t kDxOff = 0;
constexpr size_t kDyOff = kDxOff + 8;
constexpr size_t kDthetaOff = kDyOff + 8;
}  // namespace

using StructType = wpi::Struct<wpi::math::Twist2d>;

wpi::math::Twist2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Twist2d{
      units::meter_t{wpi::UnpackStruct<double, kDxOff>(data)},
      units::meter_t{wpi::UnpackStruct<double, kDyOff>(data)},
      units::radian_t{wpi::UnpackStruct<double, kDthetaOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Twist2d& value) {
  wpi::PackStruct<kDxOff>(data, value.dx.value());
  wpi::PackStruct<kDyOff>(data, value.dy.value());
  wpi::PackStruct<kDthetaOff>(data, value.dtheta.value());
}
