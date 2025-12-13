// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/spline/CubicHermiteSpline.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/spline.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::CubicHermiteSpline> {
  using MessageStruct = wpi_proto_ProtobufCubicHermiteSpline;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::CubicHermiteSpline>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::CubicHermiteSpline>;
  static std::optional<wpi::math::CubicHermiteSpline> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::CubicHermiteSpline& value);
};
