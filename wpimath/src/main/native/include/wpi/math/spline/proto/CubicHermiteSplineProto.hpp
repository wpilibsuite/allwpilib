// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/spline/CubicHermiteSpline.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/spline.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::CubicHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufCubicHermiteSpline;
  using InputStream = wpi::ProtoInputStream<wpi::math::CubicHermiteSpline>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::CubicHermiteSpline>;
  static std::optional<wpi::math::CubicHermiteSpline> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::CubicHermiteSpline& value);
};
