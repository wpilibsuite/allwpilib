// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/spline.npb.h"
#include "wpimath/spline/CubicHermiteSpline.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::CubicHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufCubicHermiteSpline;
  using InputStream = wpi::ProtoInputStream<wpimath::CubicHermiteSpline>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::CubicHermiteSpline>;
  static std::optional<wpimath::CubicHermiteSpline> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::CubicHermiteSpline& value);
};
