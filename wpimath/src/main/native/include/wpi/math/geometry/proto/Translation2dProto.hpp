// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/geometry/Translation2d.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpimath/protobuf/geometry2d.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::Translation2d> {
  using MessageStruct = wpi_proto_ProtobufTranslation2d;
  using InputStream = wpi::ProtoInputStream<wpi::math::Translation2d>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::Translation2d>;
  static std::optional<wpi::math::Translation2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Translation2d& value);
};
