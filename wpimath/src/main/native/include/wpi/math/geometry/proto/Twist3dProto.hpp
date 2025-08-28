// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/geometry/Twist3d.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/geometry3d.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::Twist3d> {
  using MessageStruct = wpi_proto_ProtobufTwist3d;
  using InputStream = wpi::ProtoInputStream<wpi::math::Twist3d>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::Twist3d>;
  static std::optional<wpi::math::Twist3d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Twist3d& value);
};
