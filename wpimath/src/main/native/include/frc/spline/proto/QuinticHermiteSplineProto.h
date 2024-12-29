// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/spline/QuinticHermiteSpline.h"
#include "wpimath/protobuf/spline.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::QuinticHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufQuinticHermiteSpline;
  using InputStream = wpi::ProtoInputStream<frc::QuinticHermiteSpline>;
  using OutputStream = wpi::ProtoOutputStream<frc::QuinticHermiteSpline>;
  static std::optional<frc::QuinticHermiteSpline> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::QuinticHermiteSpline& value);
};
