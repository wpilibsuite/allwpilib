// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/struct/QuaternionStruct.hpp"

namespace {
constexpr size_t W_OFF = 0;
constexpr size_t X_OFF = W_OFF + 8;
constexpr size_t Y_OFF = X_OFF + 8;
constexpr size_t Z_OFF = Y_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::Quaternion>;

wpi::math::Quaternion StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::Quaternion{
      wpi::util::UnpackStruct<double, W_OFF>(data),
      wpi::util::UnpackStruct<double, X_OFF>(data),
      wpi::util::UnpackStruct<double, Y_OFF>(data),
      wpi::util::UnpackStruct<double, Z_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::Quaternion& value) {
  wpi::util::PackStruct<W_OFF>(data, value.W());
  wpi::util::PackStruct<X_OFF>(data, value.X());
  wpi::util::PackStruct<Y_OFF>(data, value.Y());
  wpi::util::PackStruct<Z_OFF>(data, value.Z());
}
