// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/controller/struct/SimpleMotorFeedforwardStruct.h"

template <class Distance>
frc::SimpleMotorFeedforward<Distance>
wpi::Struct<frc::SimpleMotorFeedforward<Distance>>::Unpack(
    std::span<const uint8_t> data) {
  constexpr size_t kKsOff = 0;
  constexpr size_t kKvOff = kKsOff + 8;
  constexpr size_t kKaOff = kKvOff + 8;
  constexpr size_t kDtOff = kKaOff + 8;
  return {units::volt_t{wpi::UnpackStruct<double, kKsOff>(data)},
          units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
              wpi::UnpackStruct<double, kKvOff>(data)},
          units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
              wpi::UnpackStruct<double, kKaOff>(data)},
          units::second_t{wpi::UnpackStruct<double, kDtOff>(data)}};
}

template <class Distance>
void wpi::Struct<frc::SimpleMotorFeedforward<Distance>>::Pack(
    std::span<uint8_t> data,
    const frc::SimpleMotorFeedforward<Distance>& value) {
  constexpr size_t kKsOff = 0;
  constexpr size_t kKvOff = kKsOff + 8;
  constexpr size_t kKaOff = kKvOff + 8;
  constexpr size_t kDtOff = kKaOff + 8;
  wpi::PackStruct<kKsOff>(data, value.GetKs().value());
  wpi::PackStruct<kKvOff>(
      data,
      units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
          value.GetKv()}
          .value());
  wpi::PackStruct<kKaOff>(
      data,
      units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
          value.GetKa()}
          .value());
  wpi::PackStruct<kDtOff>(data, units::second_t{value.GetDt()}.value());
}
