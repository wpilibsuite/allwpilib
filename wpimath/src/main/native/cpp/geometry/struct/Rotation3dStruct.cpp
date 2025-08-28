// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpimath/geometry/struct/Rotation3dStruct.h"

namespace {
constexpr size_t kQOff = 0;
}  // namespace

using StructType = wpi::Struct<wpimath::Rotation3d>;

wpimath::Rotation3d StructType::Unpack(std::span<const uint8_t> data) {
  return wpimath::Rotation3d{
      wpi::UnpackStruct<wpimath::Quaternion, kQOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpimath::Rotation3d& value) {
  wpi::PackStruct<kQOff>(data, value.GetQuaternion());
}
