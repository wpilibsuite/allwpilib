// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/struct/Struct.hpp"

// Everything is converted into units for
// wpi::math::SimpleMotorFeedforward<wpi::units::meters> or
// wpi::math::SimpleMotorFeedforward<wpi::units::radians>

template <class Distance>
  requires wpi::units::length_unit<Distance> || wpi::units::angle_unit<Distance>
struct wpi::util::Struct<wpi::math::SimpleMotorFeedforward<Distance>> {
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
        wpi::units::unit<std::ratio<1>,
                         wpi::units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    constexpr size_t kKsOff = 0;
    constexpr size_t kKvOff = kKsOff + 8;
    constexpr size_t kKaOff = kKvOff + 8;
    constexpr size_t kDtOff = kKaOff + 8;
    return {
        wpi::units::volt_t{wpi::util::UnpackStruct<double, kKsOff>(data)},
        wpi::units::unit_t<typename BaseFeedforward::kv_unit>{
            wpi::util::UnpackStruct<double, kKvOff>(data)},
        wpi::units::unit_t<typename BaseFeedforward::ka_unit>{
            wpi::util::UnpackStruct<double, kKaOff>(data)},
        wpi::units::second_t{wpi::util::UnpackStruct<double, kDtOff>(data)}};
  }

  static void Pack(std::span<uint8_t> data,
                   const wpi::math::SimpleMotorFeedforward<Distance>& value) {
    using BaseUnit =
        wpi::units::unit<std::ratio<1>,
                         wpi::units::traits::base_unit_of<Distance>>;
    using BaseFeedforward = wpi::math::SimpleMotorFeedforward<BaseUnit>;
    constexpr size_t kKsOff = 0;
    constexpr size_t kKvOff = kKsOff + 8;
    constexpr size_t kKaOff = kKvOff + 8;
    constexpr size_t kDtOff = kKaOff + 8;
    wpi::util::PackStruct<kKsOff>(data, value.GetKs().value());
    wpi::util::PackStruct<kKvOff>(
        data,
        wpi::units::unit_t<typename BaseFeedforward::kv_unit>{value.GetKv()}
            .value());
    wpi::util::PackStruct<kKaOff>(
        data,
        wpi::units::unit_t<typename BaseFeedforward::ka_unit>{value.GetKa()}
            .value());
    wpi::util::PackStruct<kDtOff>(data,
                                  wpi::units::second_t{value.GetDt()}.value());
  }
};

static_assert(wpi::util::StructSerializable<
              wpi::math::SimpleMotorFeedforward<wpi::units::meters>>);
static_assert(wpi::util::StructSerializable<
              wpi::math::SimpleMotorFeedforward<wpi::units::feet>>);
static_assert(wpi::util::StructSerializable<
              wpi::math::SimpleMotorFeedforward<wpi::units::radians>>);
