// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/spline/CubicHermiteSpline.h"
#include "wpimath/protobuf/spline.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::CubicHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufCubicHermiteSpline;
  using InputStream = wpi::ProtoInputStream<frc::CubicHermiteSpline>;
  using OutputStream = wpi::ProtoOutputStream<frc::CubicHermiteSpline>;
  static std::optional<frc::CubicHermiteSpline> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::CubicHermiteSpline& value);
};
