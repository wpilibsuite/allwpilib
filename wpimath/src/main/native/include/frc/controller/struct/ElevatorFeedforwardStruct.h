// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "frc/controller/ElevatorFeedforward.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<frc::ElevatorFeedforward> {
  static constexpr std::string_view GetTypeString() {
    return "struct:ElevatorFeedforward";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double ks;double kg;double kv;double ka";
  }

  static frc::ElevatorFeedforward Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::ElevatorFeedforward& value);
};

static_assert(wpi::StructSerializable<frc::ElevatorFeedforward>);
