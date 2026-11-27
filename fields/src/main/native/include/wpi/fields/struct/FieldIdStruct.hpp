// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/fields/fields.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/struct/Struct.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Struct<wpi::fields::FieldId> {
  static constexpr std::string_view GetTypeName() { return "FieldId"; }
  static constexpr size_t GetSize() { return 4; }
  static constexpr std::string_view GetSchema() { return "int32 id"; }

  static wpi::fields::FieldId Unpack(std::span<const uint8_t> data);
  static void Pack(std::span<uint8_t> data, const wpi::fields::FieldId& value);
};

static_assert(wpi::util::StructSerializable<wpi::fields::FieldId>);
