// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/struct/DCMotorStruct.hpp"

namespace {
constexpr size_t NOMINAL_VOLTAGE_OFF = 0;
constexpr size_t STALL_TORQUE_OFF = NOMINAL_VOLTAGE_OFF + 8;
constexpr size_t STALL_CURRENT_OFF = STALL_TORQUE_OFF + 8;
constexpr size_t FREE_CURRENT_OFF = STALL_CURRENT_OFF + 8;
constexpr size_t FREE_SPEED_OFF = FREE_CURRENT_OFF + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::DCMotor>;

wpi::math::DCMotor StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::DCMotor{
      wpi::units::volts<>{
          wpi::util::UnpackStruct<double, NOMINAL_VOLTAGE_OFF>(data)},
      wpi::units::newton_meters<>{
          wpi::util::UnpackStruct<double, STALL_TORQUE_OFF>(data)},
      wpi::units::amperes<>{
          wpi::util::UnpackStruct<double, STALL_CURRENT_OFF>(data)},
      wpi::units::amperes<>{
          wpi::util::UnpackStruct<double, FREE_CURRENT_OFF>(data)},
      wpi::units::radians_per_second<>{
          wpi::util::UnpackStruct<double, FREE_SPEED_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data,
                      const wpi::math::DCMotor& value) {
  wpi::util::PackStruct<NOMINAL_VOLTAGE_OFF>(data,
                                             value.nominalVoltage.value());
  wpi::util::PackStruct<STALL_TORQUE_OFF>(data, value.stallTorque.value());
  wpi::util::PackStruct<STALL_CURRENT_OFF>(data, value.stallCurrent.value());
  wpi::util::PackStruct<FREE_CURRENT_OFF>(data, value.freeCurrent.value());
  wpi::util::PackStruct<FREE_SPEED_OFF>(data, value.freeSpeed.value());
}
