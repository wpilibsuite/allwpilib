// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/spline.npb.h"
#include "wpimath/spline/QuinticHermiteSpline.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::QuinticHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufQuinticHermiteSpline;
  using InputStream = wpi::ProtoInputStream<wpimath::QuinticHermiteSpline>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::QuinticHermiteSpline>;
  static std::optional<wpimath::QuinticHermiteSpline> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::QuinticHermiteSpline& value);
};
