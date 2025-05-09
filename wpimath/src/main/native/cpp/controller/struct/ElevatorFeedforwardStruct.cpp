// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/ElevatorFeedforwardStruct.h"

namespace {
constexpr size_t KS_OFF = 0;
constexpr size_t KG_OFF = KS_OFF + 8;
constexpr size_t KV_OFF = KG_OFF + 8;
constexpr size_t KA_OFF = KV_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::ElevatorFeedforward>;

frc::ElevatorFeedforward StructType::Unpack(std::span<const uint8_t> data) {
  return frc::ElevatorFeedforward{
      units::volt_t{wpi::UnpackStruct<double, KS_OFF>(data)},
      units::volt_t{wpi::UnpackStruct<double, KG_OFF>(data)},
      units::unit_t<frc::ElevatorFeedforward::kv_unit>{
          wpi::UnpackStruct<double, KV_OFF>(data)},
      units::unit_t<frc::ElevatorFeedforward::ka_unit>{
          wpi::UnpackStruct<double, KA_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const frc::ElevatorFeedforward& value) {
  wpi::PackStruct<KS_OFF>(data, value.GetKs().value());
  wpi::PackStruct<KG_OFF>(data, value.GetKg().value());
  wpi::PackStruct<KV_OFF>(data, value.GetKv().value());
  wpi::PackStruct<KA_OFF>(data, value.GetKa().value());
}
