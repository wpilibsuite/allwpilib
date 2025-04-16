// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/struct/ElevatorFeedforwardStruct.h"

namespace {
constexpr size_t kKsOff = 0;
constexpr size_t kKgOff = kKsOff + 8;
constexpr size_t kKvOff = kKgOff + 8;
constexpr size_t kKaOff = kKvOff + 8;
}  // namespace

namespace wpi {

template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
frc::ElevatorFeedforward<Input> Struct<frc::ElevatorFeedforward<Input>>::Unpack(std::span<const uint8_t> data) {
  return frc::ElevatorFeedforward<Input>{
      units::unit_t<Input>{UnpackStruct<double, kKsOff>(data)},
      units::unit_t<Input>{UnpackStruct<double, kKgOff>(data)},
      units::unit_t<typename frc::ElevatorFeedforward<Input>::kv_unit>{
          UnpackStruct<double, kKvOff>(data)},
      units::unit_t<typename frc::ElevatorFeedforward<Input>::ka_unit>{
          UnpackStruct<double, kKaOff>(data)}};
}

template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
void Struct<frc::ElevatorFeedforward<Input>>::Pack(std::span<uint8_t> data,
                                                   const frc::ElevatorFeedforward<Input>& value) {
  PackStruct<kKsOff>(data, value.ks.value());
  PackStruct<kKgOff>(data, value.kg.value());
  PackStruct<kKvOff>(data, value.kv.value());
  PackStruct<kKaOff>(data, value.ka.value());
}

// Explicit instantiations
template struct Struct<frc::ElevatorFeedforward<units::volt_t>>;
template struct Struct<frc::ElevatorFeedforward<units::ampere_t>>;

}  // namespace wpi
