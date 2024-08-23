// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/struct/Struct.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "units/length.h"

// Everything is converted into units for
// frc::SimpleMotorFeedforward<units::meters>

template <class Distance>
struct wpi::Struct<frc::SimpleMotorFeedforward<Distance>> {
  static constexpr std::string_view GetTypeName() {
    return "SimpleMotorFeedforward";
  }
  static constexpr size_t GetSize() { return 32; }
  static constexpr std::string_view GetSchema() {
    return "double ks;double kv;double ka;double dt";
  }

  static frc::SimpleMotorFeedforward<Distance> Unpack(
      std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data,
                   const frc::SimpleMotorFeedforward<Distance>& value);
};

static_assert(
    wpi::StructSerializable<frc::SimpleMotorFeedforward<units::meters>>);
static_assert(
    wpi::StructSerializable<frc::SimpleMotorFeedforward<units::feet>>);

#include "frc/controller/struct/SimpleMotorFeedforwardStruct.inc"
