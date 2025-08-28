// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/spline/QuinticHermiteSpline.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/spline.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::QuinticHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufQuinticHermiteSpline;
  using InputStream = wpi::ProtoInputStream<wpi::math::QuinticHermiteSpline>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::QuinticHermiteSpline>;
  static std::optional<wpi::math::QuinticHermiteSpline> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::QuinticHermiteSpline& value);
};
