// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/ArmFeedforwardStruct.h"

namespace {
constexpr size_t kKsOff = 0;
constexpr size_t kKgOff = kKsOff + 8;
constexpr size_t kKvOff = kKgOff + 8;
constexpr size_t kKaOff = kKvOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::ArmFeedforward>;

frc::ArmFeedforward StructType::Unpack(std::span<const uint8_t, kSize> data) {
  return frc::ArmFeedforward{
      wpi::UnpackStruct<double, kKsOff>(data),
      wpi::UnpackStruct<double, kKgOff>(data),
      wpi::UnpackStruct<double, kKvOff>(data),
      wpi::UnpackStruct<double, kKaOff>(data),
  };
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::ArmFeedforward& value) {
  wpi::PackStruct<kKsOff>(data, value.ks.value());
  wpi::PackStruct<kKgOff>(data, value.kg.value());
  wpi::PackStruct<kKvOff>(data, value.kv.value());
  wpi::PackStruct<kKaOff>(data, value.ka.value());
}
