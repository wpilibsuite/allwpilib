// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/system/plant/struct/DCMotorStruct.h"

namespace {
constexpr size_t NOMINAL_VOLTAGE_OFF = 0;
constexpr size_t STALL_TORQUE_OFF = NOMINAL_VOLTAGE_OFF + 8;
constexpr size_t STALL_CURRENT_OFF = STALL_TORQUE_OFF + 8;
constexpr size_t FREE_CURRENT_OFF = STALL_CURRENT_OFF + 8;
constexpr size_t FREE_SPEED_OFF = FREE_CURRENT_OFF + 8;
}  // namespace

using StructType = wpi::Struct<frc::DCMotor>;

frc::DCMotor StructType::Unpack(std::span<const uint8_t> data) {
  return frc::DCMotor{
      units::volt_t{wpi::UnpackStruct<double, NOMINAL_VOLTAGE_OFF>(data)},
      units::newton_meter_t{wpi::UnpackStruct<double, STALL_TORQUE_OFF>(data)},
      units::ampere_t{wpi::UnpackStruct<double, STALL_CURRENT_OFF>(data)},
      units::ampere_t{wpi::UnpackStruct<double, FREE_CURRENT_OFF>(data)},
      units::radians_per_second_t{
          wpi::UnpackStruct<double, FREE_SPEED_OFF>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const frc::DCMotor& value) {
  wpi::PackStruct<NOMINAL_VOLTAGE_OFF>(data, value.nominalVoltage.value());
  wpi::PackStruct<STALL_TORQUE_OFF>(data, value.stallTorque.value());
  wpi::PackStruct<STALL_CURRENT_OFF>(data, value.stallCurrent.value());
  wpi::PackStruct<FREE_CURRENT_OFF>(data, value.freeCurrent.value());
  wpi::PackStruct<FREE_SPEED_OFF>(data, value.freeSpeed.value());
}
