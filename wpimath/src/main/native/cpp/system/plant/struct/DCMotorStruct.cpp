// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/plant/struct/DCMotorStruct.hpp"

namespace {
constexpr size_t kNominalVoltageOff = 0;
constexpr size_t kStallTorqueOff = kNominalVoltageOff + 8;
constexpr size_t kStallCurrentOff = kStallTorqueOff + 8;
constexpr size_t kFreeCurrentOff = kStallCurrentOff + 8;
constexpr size_t kFreeSpeedOff = kFreeCurrentOff + 8;
}  // namespace

using StructType = wpi::util::Struct<wpi::math::DCMotor>;

wpi::math::DCMotor StructType::Unpack(std::span<const uint8_t> data) {
  return wpi::math::DCMotor{
      wpi::units::volt_t{wpi::util::UnpackStruct<double, kNominalVoltageOff>(data)},
      wpi::units::newton_meter_t{wpi::util::UnpackStruct<double, kStallTorqueOff>(data)},
      wpi::units::ampere_t{wpi::util::UnpackStruct<double, kStallCurrentOff>(data)},
      wpi::units::ampere_t{wpi::util::UnpackStruct<double, kFreeCurrentOff>(data)},
      wpi::units::radians_per_second_t{
          wpi::util::UnpackStruct<double, kFreeSpeedOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t> data, const wpi::math::DCMotor& value) {
  wpi::util::PackStruct<kNominalVoltageOff>(data, value.nominalVoltage.value());
  wpi::util::PackStruct<kStallTorqueOff>(data, value.stallTorque.value());
  wpi::util::PackStruct<kStallCurrentOff>(data, value.stallCurrent.value());
  wpi::util::PackStruct<kFreeCurrentOff>(data, value.freeCurrent.value());
  wpi::util::PackStruct<kFreeSpeedOff>(data, value.freeSpeed.value());
}
