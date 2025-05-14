// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/DifferentialDriveFeedforwardStruct.h"

namespace {
constexpr size_t KV_LINEAR_OFF = 0;
constexpr size_t KA_LINEAR_OFF = KV_LINEAR_OFF + 8;
constexpr size_t KV_ANGULAR_OFF = KA_LINEAR_OFF + 8;
constexpr size_t KA_ANGULAR_OFF = KV_ANGULAR_OFF + 8;
}  // namespace

frc::DifferentialDriveFeedforward wpi::Struct<
    frc::DifferentialDriveFeedforward>::Unpack(std::span<const uint8_t> data) {
  return {
      decltype(1_V / 1_mps){wpi::UnpackStruct<double, KV_LINEAR_OFF>(data)},
      decltype(1_V / 1_mps_sq){wpi::UnpackStruct<double, KA_LINEAR_OFF>(data)},
      decltype(1_V / 1_mps){wpi::UnpackStruct<double, KV_ANGULAR_OFF>(data)},
      decltype(1_V /
               1_mps_sq){wpi::UnpackStruct<double, KA_ANGULAR_OFF>(data)}};
}

void wpi::Struct<frc::DifferentialDriveFeedforward>::Pack(
    std::span<uint8_t> data, const frc::DifferentialDriveFeedforward& value) {
  wpi::PackStruct<KV_LINEAR_OFF>(data, value.m_kVLinear.value());
  wpi::PackStruct<KA_LINEAR_OFF>(data, value.m_kALinear.value());
  wpi::PackStruct<KV_ANGULAR_OFF>(data, value.m_kVAngular.value());
  wpi::PackStruct<KA_ANGULAR_OFF>(data, value.m_kAAngular.value());
}
