// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/ChassisAccelerationsStruct.hpp"

#include "wpi/util/struct/Struct.hpp"

#include "wpi/math/kinematics/ChassisAccelerations.hpp"

wpi::math::ChassisAccelerations wpi::util::Struct<
    wpi::math::ChassisAccelerations>::Unpack(std::span<const uint8_t> data) {
  constexpr size_t kAxOff = 0;
  constexpr size_t kAyOff = kAxOff + 8;
  constexpr size_t kAlphaOff = kAyOff + 8;
  return wpi::math::ChassisAccelerations{
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kAxOff>(data)},
      units::meters_per_second_squared_t{
          wpi::util::UnpackStruct<double, kAyOff>(data)},
      units::radians_per_second_squared_t{
          wpi::util::UnpackStruct<double, kAlphaOff>(data)},
  };
}

void wpi::util::Struct<wpi::math::ChassisAccelerations>::Pack(
    std::span<uint8_t> data, const wpi::math::ChassisAccelerations& value) {
  constexpr size_t kAxOff = 0;
  constexpr size_t kAyOff = kAxOff + 8;
  constexpr size_t kAlphaOff = kAyOff + 8;
  wpi::util::PackStruct<kAxOff>(data, value.ax.value());
  wpi::util::PackStruct<kAyOff>(data, value.ay.value());
  wpi::util::PackStruct<kAlphaOff>(data, value.alpha.value());
}
