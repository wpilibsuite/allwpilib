// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/fields/FieldTag.hpp"
#include "wpi/fields/protobuf/fields.npb.h"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::fields::FieldTag> {
  using MessageStruct = wpi_proto_ProtobufFieldTag;
  using InputStream = wpi::util::ProtoInputStream<wpi::fields::FieldTag>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::fields::FieldTag>;
  static std::optional<wpi::fields::FieldTag> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::fields::FieldTag& value);
};
