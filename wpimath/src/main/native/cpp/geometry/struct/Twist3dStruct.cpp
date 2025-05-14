// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Twist3dStruct.h"

namespace {
constexpr size_t DX_OFF = 0;
constexpr size_t DY_OFF = DX_OFF + 8;
constexpr size_t DZ_OFF = DY_OFF + 8;
constexpr size_t RX_OFF = DZ_OFF + 8;
constexpr size_t RY_OFF = RX_OFF + 8;
constexpr size_t RZ_OFF = RY_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Twist3d>;

frc::Twist3d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Twist3d{
      units::meter_t{wpi::UnpackStruct<double, DX_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, DY_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, DZ_OFF>(data)},
      units::radian_t{wpi::UnpackStruct<double, RX_OFF>(data)},
      units::radian_t{wpi::UnpackStruct<double, RY_OFF>(data)},
      units::radian_t{wpi::UnpackStruct<double, RZ_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Twist3d& value) {
  wpi::PackStruct<DX_OFF>(data, value.dx.value());
  wpi::PackStruct<DY_OFF>(data, value.dy.value());
  wpi::PackStruct<DZ_OFF>(data, value.dz.value());
  wpi::PackStruct<RX_OFF>(data, value.rx.value());
  wpi::PackStruct<RY_OFF>(data, value.ry.value());
  wpi::PackStruct<RZ_OFF>(data, value.rz.value());
}
