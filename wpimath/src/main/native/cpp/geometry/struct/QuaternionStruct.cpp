// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/struct/QuaternionStruct.h"

namespace {
constexpr size_t W_OFF = 0;
constexpr size_t X_OFF = W_OFF + 8;
constexpr size_t Y_OFF = X_OFF + 8;
constexpr size_t Z_OFF = Y_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::Quaternion>;

frc::Quaternion StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Quaternion{
      wpi::UnpackStruct<double, W_OFF>(data),
      wpi::UnpackStruct<double, X_OFF>(data),
      wpi::UnpackStruct<double, Y_OFF>(data),
      wpi::UnpackStruct<double, Z_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Quaternion& value) {
  wpi::PackStruct<W_OFF>(data, value.W());
  wpi::PackStruct<X_OFF>(data, value.X());
  wpi::PackStruct<Y_OFF>(data, value.Y());
  wpi::PackStruct<Z_OFF>(data, value.Z());
}
