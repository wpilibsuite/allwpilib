// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/ElevatorFeedforwardStruct.h"

namespace {
constexpr size_t kKsOff = 0;
constexpr size_t kKgOff = kKsOff + 8;
constexpr size_t kKvOff = kKgOff + 8;
constexpr size_t kKaOff = kKvOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::ElevatorFeedforward>;

frc::ElevatorFeedforward StructType::Unpack(std::span<const uint8_t> data) {
  return frc::ElevatorFeedforward{
      units::volt_t{wpi::UnpackStruct<double, kKsOff>(data)},
      units::volt_t{wpi::UnpackStruct<double, kKgOff>(data)},
      units::unit_t<frc::ElevatorFeedforward::kv_unit>{
          wpi::UnpackStruct<double, kKvOff>(data)},
      units::unit_t<frc::ElevatorFeedforward::ka_unit>{
          wpi::UnpackStruct<double, kKaOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::ElevatorFeedforward& value) {
  wpi::PackStruct<kKsOff>(data, value.GetKs().value());
  wpi::PackStruct<kKgOff>(data, value.GetKg().value());
  wpi::PackStruct<kKvOff>(data, value.GetKv().value());
  wpi::PackStruct<kKaOff>(data, value.GetKa().value());
}
