// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <fmt/format.h>
#include <wpi/ct_string.h>
#include <wpi/struct/Struct.h>

#include "frc/EigenCore.h"

template <int Size, int Options, int MaxRows, int MaxCols>
struct wpi::Struct<frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>> {
  static constexpr ct_string kTypeName =
      wpi::Concat("Vector__"_ct_string, wpi::NumToCtString<Size>());
  static constexpr std::string_view GetTypeName() { return kTypeName; }
  static constexpr size_t GetSize() { return Size * 8; }
  static constexpr ct_string kSchema = wpi::Concat(
      "double data["_ct_string, wpi::NumToCtString<Size>(), "]"_ct_string);
  static constexpr std::string_view GetSchema() { return kSchema; }

  static frc::Matrixd<Size, 1, Options, MaxRows, MaxCols> Unpack(
      std::span<const uint8_t> data);
  static void Pack(
      std::span<uint8_t> data,
      const frc::Matrixd<Size, 1, Options, MaxRows, MaxCols>& value);
};

static_assert(wpi::StructSerializable<frc::Vectord<1>>);
static_assert(wpi::StructSerializable<frc::Vectord<3>>);

#include "frc/struct/VectorStruct.inc"
