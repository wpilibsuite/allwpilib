// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/math/geometry/struct/Rotation3dStruct.hpp>

namespace {
constexpr size_t kQOff = 0;
}  // namespace

using StructType = wpi::Struct<wpi::math::Rotation3d>;

wpi::math::Rotation3d StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Rotation3d{
      wpi::UnpackStruct<wpi::math::Quaternion, kQOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Rotation3d& value) {
  wpi::PackStruct<kQOff>(data, value.GetQuaternion());
}
