// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpi/math/geometry/Pose2d.h"
#include "wpimath/protobuf/geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::Pose2d> {
  using MessageStruct = wpi_proto_ProtobufPose2d;
  using InputStream = wpi::ProtoInputStream<wpi::math::Pose2d>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::Pose2d>;
  static std::optional<wpi::math::Pose2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Pose2d& value);
};
