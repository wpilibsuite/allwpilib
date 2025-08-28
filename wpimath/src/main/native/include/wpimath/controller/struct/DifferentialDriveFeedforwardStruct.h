// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/struct/Struct.h>

#include "wpimath/controller/DifferentialDriveFeedforward.h"

template <>
struct WPILIB_DLLEXPORT wpi::Struct<wpimath::DifferentialDriveFeedforward> {
  static constexpr std::string_view GetTypeName() {
    return "DifferentialDriveFeedforward";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double kv_linear;double ka_linear;double kv_angular;double "
           "ka_angular";
  }

  static wpimath::DifferentialDriveFeedforward Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const wpimath::DifferentialDriveFeedforward& value);
};

static_assert(wpi::StructSerializable<wpimath::DifferentialDriveFeedforward>);
