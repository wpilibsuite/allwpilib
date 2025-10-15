// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/struct/DifferentialDriveFeedforwardStruct.hpp"

namespace {
constexpr size_t kKvLinearOff = 0;
constexpr size_t kKaLinearOff = kKvLinearOff + 8;
constexpr size_t kKvAngularOff = kKaLinearOff + 8;
constexpr size_t kKaAngularOff = kKvAngularOff + 8;
}  // namespace

wpi::math::DifferentialDriveFeedforward wpi::util::Struct<
    wpi::math::DifferentialDriveFeedforward>::Unpack(std::span<const uint8_t> data) {
  return {
      decltype(1_V / 1_mps){wpi::util::UnpackStruct<double, kKvLinearOff>(data)},
      decltype(1_V / 1_mps_sq){wpi::util::UnpackStruct<double, kKaLinearOff>(data)},
      decltype(1_V / 1_mps){wpi::util::UnpackStruct<double, kKvAngularOff>(data)},
      decltype(1_V / 1_mps_sq){wpi::util::UnpackStruct<double, kKaAngularOff>(data)}};
}

void wpi::util::Struct<wpi::math::DifferentialDriveFeedforward>::Pack(
    std::span<uint8_t> data, const wpi::math::DifferentialDriveFeedforward& value) {
  wpi::util::PackStruct<kKvLinearOff>(data, value.m_kVLinear.value());
  wpi::util::PackStruct<kKaLinearOff>(data, value.m_kALinear.value());
  wpi::util::PackStruct<kKvAngularOff>(data, value.m_kVAngular.value());
  wpi::util::PackStruct<kKaAngularOff>(data, value.m_kAAngular.value());
}
