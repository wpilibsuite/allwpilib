// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/QuaternionStruct.hpp"

namespace {
constexpr size_t kWOff = 0;
constexpr size_t kXOff = kWOff + 8;
constexpr size_t kYOff = kXOff + 8;
constexpr size_t kZOff = kYOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::Quaternion>;

frc::Quaternion StructType::Unpack(std::span<const uint8_t> data) {
  return frc::Quaternion{
      wpi::UnpackStruct<double, kWOff>(data),
      wpi::UnpackStruct<double, kXOff>(data),
      wpi::UnpackStruct<double, kYOff>(data),
      wpi::UnpackStruct<double, kZOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::Quaternion& value) {
  wpi::PackStruct<kWOff>(data, value.W());
  wpi::PackStruct<kXOff>(data, value.X());
  wpi::PackStruct<kYOff>(data, value.Y());
  wpi::PackStruct<kZOff>(data, value.Z());
}
