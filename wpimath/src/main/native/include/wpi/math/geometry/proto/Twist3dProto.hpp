// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/protobuf_geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::Twist3d> {
  using MessageStruct = wpi_proto_ProtobufTwist3d;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::Twist3d>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::Twist3d>;
  static std::optional<wpi::math::Twist3d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Twist3d& value);
};
