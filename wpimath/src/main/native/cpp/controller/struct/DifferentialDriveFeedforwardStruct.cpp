// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/DifferentialDriveFeedforwardStruct.h"

frc::DifferentialDriveFeedforward wpi::Struct<
    frc::DifferentialDriveFeedforward>::Unpack(std::span<const uint8_t> data) {
  return {decltype(1_V / 1_mps){wpi::UnpackStruct<double, 0>(data)},
          decltype(1_V / 1_mps_sq){wpi::UnpackStruct<double, 8>(data)},
          decltype(1_V / 1_mps){wpi::UnpackStruct<double, 16>(data)},
          decltype(1_V / 1_mps_sq){wpi::UnpackStruct<double, 24>(data)}};
}

void wpi::Struct<frc::DifferentialDriveFeedforward>::Pack(
    std::span<uint8_t> data, const frc::DifferentialDriveFeedforward& value) {
  wpi::PackStruct<0>(data, value.m_kVLinear.value());
  wpi::PackStruct<8>(data, value.m_kALinear.value());
  wpi::PackStruct<16>(data, value.m_kVAngular.value());
  wpi::PackStruct<24>(data, value.m_kAAngular.value());
}
