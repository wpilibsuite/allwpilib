// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/geometry/Transform3d.h"
#include "wpimath/protobuf/geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Transform3d> {
  using MessageStruct = wpi_proto_ProtobufTransform3d;
  using InputStream = wpi::ProtoInputStream<frc::Transform3d>;
  using OutputStream = wpi::ProtoOutputStream<frc::Transform3d>;
  static std::optional<frc::Transform3d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Transform3d& value);
};
