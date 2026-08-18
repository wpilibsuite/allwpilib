// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/struct/TrajectorySampleStruct.hpp"

namespace {
constexpr size_t kTimestampOff = 0;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::TrajectorySample>;

wpi::math::TrajectorySample StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::TrajectorySample{wpi::units::second_t{
      wpi::util::UnpackStruct<double, kTimestampOff>(data)}};
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::TrajectorySample& value) {
  wpi::util::PackStruct<kTimestampOff>(data, value.time.value());
}
