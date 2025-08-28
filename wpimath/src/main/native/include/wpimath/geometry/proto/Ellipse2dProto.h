// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/geometry/Ellipse2d.h"
#include "wpimath/protobuf/geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::Ellipse2d> {
  using MessageStruct = wpi_proto_ProtobufEllipse2d;
  using InputStream = wpi::ProtoInputStream<wpimath::Ellipse2d>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::Ellipse2d>;
  static std::optional<wpimath::Ellipse2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::Ellipse2d& value);
};
