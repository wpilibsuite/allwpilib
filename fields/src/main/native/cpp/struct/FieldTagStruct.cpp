// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/fields/struct/FieldTagStruct.hpp"

namespace {
constexpr size_t ID_OFF = 0;
constexpr size_t POSE_OFF = ID_OFF + 4;
}  // namespace

using StructType = wpi::util::Struct<wpi::fields::FieldTag>;

wpi::fields::FieldTag StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::fields::FieldTag{
      wpi::util::UnpackStruct<int, ID_OFF>(data),
      wpi::util::UnpackStruct<wpi::math::Pose3d, POSE_OFF>(data),
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::fields::FieldTag& value) {
  wpi::util::PackStruct<ID_OFF>(data, value.ID);
  wpi::util::PackStruct<POSE_OFF>(data, value.pose);
}
