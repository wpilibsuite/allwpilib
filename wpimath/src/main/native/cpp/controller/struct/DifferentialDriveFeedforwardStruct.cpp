// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/DifferentialDriveFeedforwardStruct.h"

namespace {
constexpr size_t kKvLinearOff = 0;
constexpr size_t kKaLinearOff = kKvLinearOff + 8;
constexpr size_t kKvAngularOff = kKaLinearOff + 8;
constexpr size_t kKaAngularOff = kKvAngularOff + 8;
}  // namespace

frc::DifferentialDriveFeedforward wpi::Struct<
    frc::DifferentialDriveFeedforward>::Unpack(std::span<const uint8_t> data) {
  return {
      decltype(1_V / 1_mps){wpi::UnpackStruct<double, kKvLinearOff>(data)},
      decltype(1_V / 1_mps_sq){wpi::UnpackStruct<double, kKaLinearOff>(data)},
      decltype(1_V / 1_mps){wpi::UnpackStruct<double, kKvAngularOff>(data)},
      decltype(1_V / 1_mps_sq){wpi::UnpackStruct<double, kKaAngularOff>(data)}};
}

void wpi::Struct<frc::DifferentialDriveFeedforward>::Pack(
    std::span<uint8_t> data, const frc::DifferentialDriveFeedforward& value) {
  wpi::PackStruct<kKvLinearOff>(data, value.m_kVLinear.value());
  wpi::PackStruct<kKaLinearOff>(data, value.m_kALinear.value());
  wpi::PackStruct<kKvAngularOff>(data, value.m_kVAngular.value());
  wpi::PackStruct<kKaAngularOff>(data, value.m_kAAngular.value());
}
