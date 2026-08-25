// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/ChassisAccelerationsStruct.hpp"

#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/util/struct/Struct.hpp"

wpi::math::ChassisAccelerations wpi::util::Struct<
    wpi::math::ChassisAccelerations>::Unpack(std::span<const uint8_t> data) {
  constexpr size_t AX_OFF = 0;
  constexpr size_t AY_OFF = AX_OFF + 8;
  constexpr size_t ALPHA_OFF = AY_OFF + 8;
  return wpi::math::ChassisAccelerations{
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, AX_OFF>(data)},
      units::meters_per_second_squared<>{
          wpi::util::UnpackStruct<double, AY_OFF>(data)},
      units::radians_per_second_squared<>{
          wpi::util::UnpackStruct<double, ALPHA_OFF>(data)},
  };
}

void wpi::util::Struct<wpi::math::ChassisAccelerations>::Pack(
    std::span<uint8_t> data, const wpi::math::ChassisAccelerations& value) {
  constexpr size_t AX_OFF = 0;
  constexpr size_t AY_OFF = AX_OFF + 8;
  constexpr size_t ALPHA_OFF = AY_OFF + 8;
  wpi::util::PackStruct<AX_OFF>(data, value.ax.value());
  wpi::util::PackStruct<AY_OFF>(data, value.ay.value());
  wpi::util::PackStruct<ALPHA_OFF>(data, value.alpha.value());
}
