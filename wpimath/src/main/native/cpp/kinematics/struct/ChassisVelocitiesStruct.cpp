// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/struct/ChassisVelocitiesStruct.hpp"

namespace {
constexpr size_t VX_OFF = 0;
constexpr size_t VY_OFF = VX_OFF + 8;
constexpr size_t OMEGA_OFF = VY_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::ChassisVelocities>;

wpi::math::ChassisVelocities StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::ChassisVelocities{
      wpi::units::meters_per_second<>{
          wpi::util::UnpackStruct<double, VX_OFF>(data)},
      wpi::units::meters_per_second<>{
          wpi::util::UnpackStruct<double, VY_OFF>(data)},
      wpi::units::radians_per_second<>{
          wpi::util::UnpackStruct<double, OMEGA_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::ChassisVelocities& value) {
  wpi::util::PackStruct<VX_OFF>(data, value.vx.value());
  wpi::util::PackStruct<VY_OFF>(data, value.vy.value());
  wpi::util::PackStruct<OMEGA_OFF>(data, value.omega.value());
}
