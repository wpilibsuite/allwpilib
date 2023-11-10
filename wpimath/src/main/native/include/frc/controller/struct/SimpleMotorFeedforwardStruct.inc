// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/controller/struct/SimpleMotorFeedforwardStruct.h"

template <class Distance>
frc::SimpleMotorFeedforward<Distance>
wpi::Struct<frc::SimpleMotorFeedforward<Distance>>::Unpack(
    std::span<const uint8_t, kSize> data) {
  return {units::volt_t{wpi::UnpackStruct<double, 0>(data)},
          units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
              wpi::UnpackStruct<double, 8>(data)},
          units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
              wpi::UnpackStruct<double, 16>(data)}};
}

template <class Distance>
void wpi::Struct<frc::SimpleMotorFeedforward<Distance>>::Pack(
    std::span<uint8_t, kSize> data,
    const frc::SimpleMotorFeedforward<Distance>& value) {
  wpi::PackStruct<0>(data, value.kS.value());
  wpi::PackStruct<8>(
      data,
      units::unit_t<frc::SimpleMotorFeedforward<units::meters>::kv_unit>{
          value.kV}
          .value());
  wpi::PackStruct<16>(
      data,
      units::unit_t<frc::SimpleMotorFeedforward<units::meters>::ka_unit>{
          value.kA}
          .value());
}
