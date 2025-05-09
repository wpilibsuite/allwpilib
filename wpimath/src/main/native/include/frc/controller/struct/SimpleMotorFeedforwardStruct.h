// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/length.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters> or
// frc::SimpleMotorFeedforward<units::radians>

template <class Distance>
  requires units::length_unit<Distance> || units::angle_unit<Distance>
struct wpi::Struct<frc::SimpleMotorFeedforward<Distance>> {
  static constexpr std::string_view GetTypeName() {
    return "SimpleMotorFeedforward";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double ks;double kv;double ka;double dt";
  }

  static frc::SimpleMotorFeedforward<Distance> Unpack(
      std::span<const uint8_t> data) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = frc::SimpleMotorFeedforward<BaseUnit>;
    constexpr size_t KS_OFF = 0;
    constexpr size_t KV_OFF = KS_OFF + 8;
    constexpr size_t KA_OFF = KV_OFF + 8;
    constexpr size_t DT_OFF = KA_OFF + 8;
    return {units::volt_t{wpi::UnpackStruct<double, KS_OFF>(data)},
            units::unit_t<typename BaseFeedforward::kv_unit>{
                wpi::UnpackStruct<double, KV_OFF>(data)},
            units::unit_t<typename BaseFeedforward::ka_unit>{
                wpi::UnpackStruct<double, KA_OFF>(data)},
            units::second_t{wpi::UnpackStruct<double, DT_OFF>(data)}};
  }

  static void Pack(std::span<uint8_t> data,
                   const frc::SimpleMotorFeedforward<Distance>& value) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = frc::SimpleMotorFeedforward<BaseUnit>;
    constexpr size_t KS_OFF = 0;
    constexpr size_t KV_OFF = KS_OFF + 8;
    constexpr size_t KA_OFF = KV_OFF + 8;
    constexpr size_t DT_OFF = KA_OFF + 8;
    wpi::PackStruct<KS_OFF>(data, value.GetKs().value());
    wpi::PackStruct<KV_OFF>(
        data, units::unit_t<typename BaseFeedforward::kv_unit>{value.GetKv()}
                  .value());
    wpi::PackStruct<KA_OFF>(
        data, units::unit_t<typename BaseFeedforward::ka_unit>{value.GetKa()}
                  .value());
    wpi::PackStruct<DT_OFF>(data, units::second_t{value.GetDt()}.value());
  }
};

static_assert(
    wpi::StructSerializable<frc::SimpleMotorFeedforward<units::meters>>);
static_assert(
    wpi::StructSerializable<frc::SimpleMotorFeedforward<units::feet>>);
static_assert(
    wpi::StructSerializable<frc::SimpleMotorFeedforward<units::radians>>);
