// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/trajectory/TrapezoidProfile.h"
#include "units/length.h"
#include "units/time.h"
#include "wpimath/MathShared.h"


template <class Distance>
struct wpi::Struct<typename frc::TrapezoidProfile<Distance>::template State<Distance>> {
  static constexpr std::string_view GetTypeName() {
    return "TrapezoidProfileState";
  }
  static constexpr size_t GetSize() { return 16; }
  static constexpr std::string_view GetSchema() {
    return "double position;double velocity";
  }

  static frc::TrapezoidProfile<Distance>::State Unpack(
      std::span<const uint8_t> data) {
    return frc::TrapezoidProfile<Distance>::State{
        wpi::UnpackStruct<double, 0>(data), wpi::UnpackStruct<double, 8>(data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const frc::TrapezoidProfile<Distance>::State& value) {
    wpi::PackStruct<0>(data, value.position.value());
    wpi::PackStruct<8>(data, value.velocity.value());
  }
};

static_assert(wpi::StructSerializable<
              typename frc::TrapezoidProfile<units::meter>::State>);
