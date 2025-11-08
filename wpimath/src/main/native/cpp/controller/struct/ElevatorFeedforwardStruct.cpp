// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/struct/ElevatorFeedforwardStruct.hpp"

namespace {
constexpr size_t kKsOff = 0;
constexpr size_t kKgOff = kKsOff + 8;
constexpr size_t kKvOff = kKgOff + 8;
constexpr size_t kKaOff = kKvOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::ElevatorFeedforward>;

wpi::math::ElevatorFeedforward StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::ElevatorFeedforward{
      wpi::units::volt_t{wpi::util::UnpackStruct<double, kKsOff>(data)},
      wpi::units::volt_t{wpi::util::UnpackStruct<double, kKgOff>(data)},
      wpi::units::unit_t<wpi::math::ElevatorFeedforward::kv_unit>{
          wpi::util::UnpackStruct<double, kKvOff>(data)},
      wpi::units::unit_t<wpi::math::ElevatorFeedforward::ka_unit>{
          wpi::util::UnpackStruct<double, kKaOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::ElevatorFeedforward& value) {
  wpi::util::PackStruct<kKsOff>(data, value.GetKs().value());
  wpi::util::PackStruct<kKgOff>(data, value.GetKg().value());
  wpi::util::PackStruct<kKvOff>(data, value.GetKv().value());
  wpi::util::PackStruct<kKaOff>(data, value.GetKa().value());
}
