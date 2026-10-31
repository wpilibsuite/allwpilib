// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/fields/Field.hpp"
#include "wpi/fields/protobuf/fields.npb.h"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::fields::Field> {
  using MessageStruct = wpi_proto_ProtobufField;
  using InputStream = wpi::util::ProtoInputStream<wpi::fields::Field>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::fields::Field>;
  static std::optional<wpi::fields::Field> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::fields::Field& value);
};
