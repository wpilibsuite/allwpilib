// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::Transform2d> {
  using MessageStruct = wpi_proto_ProtobufTransform2d;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::Transform2d>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::Transform2d>;
  static std::optional<wpi::math::Transform2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Transform2d& value);
};
