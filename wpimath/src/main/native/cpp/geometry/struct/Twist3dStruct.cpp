// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/Twist3dStruct.hpp"

namespace {
constexpr size_t DX_OFF = 0;
constexpr size_t DY_OFF = DX_OFF + 8;
constexpr size_t DZ_OFF = DY_OFF + 8;
constexpr size_t RX_OFF = DZ_OFF + 8;
constexpr size_t RY_OFF = RX_OFF + 8;
constexpr size_t RZ_OFF = RY_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Twist3d>;

wpi::math::Twist3d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Twist3d{
      wpi::units::meters<>{wpi::util::UnpackStruct<double, DX_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, DY_OFF>(data)},
      wpi::units::meters<>{wpi::util::UnpackStruct<double, DZ_OFF>(data)},
      wpi::units::radians<>{wpi::util::UnpackStruct<double, RX_OFF>(data)},
      wpi::units::radians<>{wpi::util::UnpackStruct<double, RY_OFF>(data)},
      wpi::units::radians<>{wpi::util::UnpackStruct<double, RZ_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Twist3d& value) {
  wpi::util::PackStruct<DX_OFF>(data, value.dx.value());
  wpi::util::PackStruct<DY_OFF>(data, value.dy.value());
  wpi::util::PackStruct<DZ_OFF>(data, value.dz.value());
  wpi::util::PackStruct<RX_OFF>(data, value.rx.value());
  wpi::util::PackStruct<RY_OFF>(data, value.ry.value());
  wpi::util::PackStruct<RZ_OFF>(data, value.rz.value());
}
