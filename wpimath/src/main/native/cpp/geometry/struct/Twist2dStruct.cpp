// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/Twist2dStruct.h"

namespace {
constexpr size_t DX_OFF = 0;
constexpr size_t DY_OFF = DX_OFF + 8;
constexpr size_t DTHETA_OFF = DY_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Twist2d>;

frc::Twist2d StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Twist2d{
      units::meter_t{wpi::UnpackStruct<double, DX_OFF>(data)},
      units::meter_t{wpi::UnpackStruct<double, DY_OFF>(data)},
      units::radian_t{wpi::UnpackStruct<double, DTHETA_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Twist2d& value) {
  wpi::PackStruct<DX_OFF>(data, value.dx.value());
  wpi::PackStruct<DY_OFF>(data, value.dy.value());
  wpi::PackStruct<DTHETA_OFF>(data, value.dtheta.value());
}
