// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/geometry/Rectangle2d.h"
#include "wpimath/protobuf/geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::Rectangle2d> {
  using MessageStruct = wpi_proto_ProtobufRectangle2d;
  using InputStream = wpi::ProtoInputStream<wpimath::Rectangle2d>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::Rectangle2d>;
  static std::optional<wpimath::Rectangle2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::Rectangle2d& value);
};
