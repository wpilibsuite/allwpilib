// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/controller/ElevatorFeedforward.h"
#include "units/length.h"

template <class Input>
  requires(units::current_unit<Input> || units::voltage_unit<Input>)
struct wpi::Struct<frc::ElevatorFeedforward<Input>> {
  static constexpr std::string_view GetTypeName() {
    return "ElevatorFeedforward";
  }
  static constexpr size_t GetSize() { return 40; }
  static constexpr std::string_view GetSchema() {
    return "double ks;double kg;double kv;double ka;double dt";
  }

  static frc::ElevatorFeedforward<Input> Unpack(std::span<const uint8_t> data) {
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::ElevatorFeedforward<InputUnit>;
    constexpr size_t kKsOff = 0;
    constexpr size_t kKgOff = kKsOff + 8;
    constexpr size_t kKvOff = kKgOff + 8;
    constexpr size_t kKaOff = kKvOff + 8;
    constexpr size_t kDtOff = kKaOff + 8;
    return {units::unit_t<Input>{wpi::UnpackStruct<double, kKsOff>(data)},
            units::unit_t<Input>{wpi::UnpackStruct<double, kKgOff>(data)},
            units::unit_t<typename BaseFeedforward::kv_unit>{
                wpi::UnpackStruct<double, kKvOff>(data)},
            units::unit_t<typename BaseFeedforward::ka_unit>{
                wpi::UnpackStruct<double, kKaOff>(data)},
            units::second_t{wpi::UnpackStruct<double, kDtOff>(data)}};
  }

  static void Pack(std::span<uint8_t> data,
                   const frc::ElevatorFeedforward<Input>& value) {
    using InputUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Input>>;
    using BaseFeedforward = frc::ElevatorFeedforward<InputUnit>;
    constexpr size_t kKsOff = 0;
    constexpr size_t kKgOff = kKsOff + 8;
    constexpr size_t kKvOff = kKgOff + 8;
    constexpr size_t kKaOff = kKvOff + 8;
    constexpr size_t kDtOff = kKaOff + 8;
    wpi::PackStruct<kKsOff>(data, value.GetKs().value());
    wpi::PackStruct<kKgOff>(data, value.GetKg().value());
    wpi::PackStruct<kKvOff>(
        data, units::unit_t<typename BaseFeedforward::kv_unit>{value.GetKv()}
                  .value());
    wpi::PackStruct<kKaOff>(
        data, units::unit_t<typename BaseFeedforward::ka_unit>{value.GetKa()}
                  .value());
    wpi::PackStruct<kDtOff>(data, units::second_t{value.GetDt()}.value());
  }
};

static_assert(wpi::StructSerializable<frc::ElevatorFeedforward<units::volt>>);
static_assert(wpi::StructSerializable<frc::ElevatorFeedforward<units::ampere>>);
