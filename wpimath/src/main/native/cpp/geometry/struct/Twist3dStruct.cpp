// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Twist3dStruct.hpp"

namespace {
constexpr size_t kDxOff = 0;
constexpr size_t kDyOff = kDxOff + 8;
constexpr size_t kDzOff = kDyOff + 8;
constexpr size_t kRxOff = kDzOff + 8;
constexpr size_t kRyOff = kRxOff + 8;
constexpr size_t kRzOff = kRyOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Twist3d>;

wpi::math::Twist3d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Twist3d{
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kDxOff>(data)},
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kDyOff>(data)},
      wpi::units::meter_t{wpi::util::UnpackStruct<double, kDzOff>(data)},
      wpi::units::radian_t{wpi::util::UnpackStruct<double, kRxOff>(data)},
      wpi::units::radian_t{wpi::util::UnpackStruct<double, kRyOff>(data)},
      wpi::units::radian_t{wpi::util::UnpackStruct<double, kRzOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Twist3d& value) {
  wpi::util::PackStruct<kDxOff>(data, value.dx.value());
  wpi::util::PackStruct<kDyOff>(data, value.dy.value());
  wpi::util::PackStruct<kDzOff>(data, value.dz.value());
  wpi::util::PackStruct<kRxOff>(data, value.rx.value());
  wpi::util::PackStruct<kRyOff>(data, value.ry.value());
  wpi::util::PackStruct<kRzOff>(data, value.rz.value());
}
