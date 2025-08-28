// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/geometry/Transform3d.h"
#include "wpimath/protobuf/geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::Transform3d> {
  using MessageStruct = wpi_proto_ProtobufTransform3d;
  using InputStream = wpi::ProtoInputStream<wpimath::Transform3d>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::Transform3d>;
  static std::optional<wpimath::Transform3d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::Transform3d& value);
};
