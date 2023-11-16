// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/system/plant/struct/DCMotorStruct.h"

namespace {
constexpr size_t kNominalVoltageOff = 0;
constexpr size_t kStallTorqueOff = kNominalVoltageOff + 8;
constexpr size_t kStallCurrentOff = kStallTorqueOff + 8;
constexpr size_t kFreeCurrentOff = kStallCurrentOff + 8;
constexpr size_t kFreeSpeedOff = kFreeCurrentOff + 8;
constexpr size_t kROff = kFreeSpeedOff + 8;
constexpr size_t kKvOff = kROff + 8;
constexpr size_t kKtOff = kKvOff + 8;
}  // namespace

using StructType = wpi::Struct<frc::DCMotor>;

frc::DCMotor StructType::Unpack(
    std::span<const uint8_t, kSize> data) {
  return frc::DCMotor{
      units::volt_t{wpi::UnpackStruct<double, kNominalVoltageOff>(data)},
      units::newton_meter_t{wpi::UnpackStruct<double, kStallTorqueOff>(data)},
      units::ampere_t{wpi::UnpackStruct<double, kStallCurrentOff>(data)},
      units::ampere_t{wpi::UnpackStruct<double, kFreeCurrentOff>(data)},
      units::radians_per_second_t{wpi::UnpackStruct<double, kFreeSpeedOff>(data)},
      units::ohm_t{wpi::UnpackStruct<double, kROff>(data)},
      frc::DCMotor::radians_per_second_per_volt_t{wpi::UnpackStruct<double, kKvOff>(data)},
      frc::DCMotor::newton_meters_per_ampere_t{wpi::UnpackStruct<double, kKtOff>(data)},
  };
}

void StructType::Pack(std::span<uint8_t, kSize> data,
                      const frc::DCMotor& value) {
  wpi::PackStruct<kNominalVoltageOff>(data, value.nominalVoltage.value());
  wpi::PackStruct<kStallTorqueOff>(data, value.stallTorque.value());
  wpi::PackStruct<kStallCurrentOff>(data, value.stallCurrent.value());
  wpi::PackStruct<kFreeCurrentOff>(data, value.freeCurrent.value());
  wpi::PackStruct<kFreeSpeedOff>(data, value.freeSpeed.value());
  wpi::PackStruct<kROff>(data, value.R.value());
  wpi::PackStruct<kKvOff>(data, value.Kv.value());
  wpi::PackStruct<kKtOff>(data, value.Kt.value());
}
