// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Twist2dStruct.hpp"

namespace {
constexpr size_t DX_OFF = 0;
constexpr size_t DY_OFF = DX_OFF + 8;
constexpr size_t DTHETA_OFF = DY_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Twist2d>;

wpi::math::Twist2d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Twist2d{
      wpi::units::meters<>{wpi::util::UnpackStruct<double, DX_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, DY_OFF>(data)},
      wpi::units::radians<>{wpi::util::UnpackStruct<double, DTHETA_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Twist2d& value) {
  wpi::util::PackStruct<DX_OFF>(data, value.dx.value());
  wpi::util::PackStruct<DY_OFF>(data, value.dy.value());
  wpi::util::PackStruct<DTHETA_OFF>(data, value.dtheta.value());
}
