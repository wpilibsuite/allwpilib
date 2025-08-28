// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/math/controller/SimpleMotorFeedforward.hpp>
#include <wpi/struct/Struct.h>

#include "units/length.hpp"

// Everything is converted into units for
// wpi::math::SimpleMotorFeedforward<units::meters> or
// wpi::math::SimpleMotorFeedforward<units::radians>

template <class Distance>
  requires units::length_unit<Distance> || units::angle_unit<Distance>
struct wpi::Struct<wpi::math::SimpleMotorFeedforward<Distance>> {
  static constexpr std::string_view GetTypeName() {
    return "SimpleMotorFeedforward";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double ks;double kv;double ka;double dt";
  }

  static wpi::math::SimpleMotorFeedforward<Distance> Unpack(
      std::span<const uint8_t> data) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    constexpr size_t kKsOff = 0;
    constexpr size_t kKvOff = kKsOff + 8;
    constexpr size_t kKaOff = kKvOff + 8;
    constexpr size_t kDtOff = kKaOff + 8;
    return {units::volt_t{wpi::UnpackStruct<double, kKsOff>(data)},
            units::unit_t<typename BaseFeedforward::kv_unit>{
                wpi::UnpackStruct<double, kKvOff>(data)},
            units::unit_t<typename BaseFeedforward::ka_unit>{
                wpi::UnpackStruct<double, kKaOff>(data)},
            units::second_t{wpi::UnpackStruct<double, kDtOff>(data)}};
  }

  static void Pack(std::span<uint8_t> data,
                   const wpi::math::SimpleMotorFeedforward<Distance>& value) {
    using BaseUnit =
        units::unit<std::ratio<1>, units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    constexpr size_t kKsOff = 0;
    constexpr size_t kKvOff = kKsOff + 8;
    constexpr size_t kKaOff = kKvOff + 8;
    constexpr size_t kDtOff = kKaOff + 8;
    wpi::PackStruct<kKsOff>(data, value.GetKs().value());
    wpi::PackStruct<kKvOff>(
        data, units::unit_t<typename BaseFeedforward::kv_unit>{value.GetKv()}
                  .value());
    wpi::PackStruct<kKaOff>(
        data, units::unit_t<typename BaseFeedforward::ka_unit>{value.GetKa()}
                  .value());
    wpi::PackStruct<kDtOff>(data, units::second_t{value.GetDt()}.value());
  }
};

static_assert(
    wpi::StructSerializable<wpi::math::SimpleMotorFeedforward<units::meters>>);
static_assert(
    wpi::StructSerializable<wpi::math::SimpleMotorFeedforward<units::feet>>);
static_assert(
    wpi::StructSerializable<wpi::math::SimpleMotorFeedforward<units::radians>>);
