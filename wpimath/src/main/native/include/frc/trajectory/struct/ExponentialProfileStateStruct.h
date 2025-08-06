// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/trajectory/ExponentialProfile.h"
#include "units/length.h"
#include "units/voltage.h"
#include "wpimath/MathShared.h"


template <class Distance, class Input>
struct wpi::Struct<
    typename frc::ExponentialProfile<Distance, Input>::template State<Distance, Input>> {
  static constexpr std::string_view GetTypeName() {
    return "ExponentialProfileState";
  }
  static constexpr size_t GetSize() { return 16; }
  static constexpr std::string_view GetSchema() {
    return "double position;double velocity";
  }

  static frc::ExponentialProfile<Distance, Input>::State
  Unpack(std::span<const uint8_t> data) {
    constexpr size_t kPositionOff = 0;
    constexpr size_t kVelocityOff = 8;
    return frc::ExponentialProfile<Distance, Input>::State{
        wpi::UnpackStruct<double, kPositionOff>(data),
        wpi::UnpackStruct<double, kVelocityOff>(data)};
  }

  static void Pack(
      std::span<uint8_t> data,
      const frc::ExponentialProfile<Distance, Input>::State&
          value) {
    wpi::PackStruct<0>(data, value.position.value());
    wpi::PackStruct<8>(data, value.velocity.value());
  }
};

static_assert(
    wpi::StructSerializable<
        typename frc::ExponentialProfile<units::meter, units::volts>::State>);
