// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>
#include <wpi/util/protobuf/Protobuf.hpp>

#include "pb.h"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpimath/protobuf/geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Pose2d> {
  using MessageStruct = wpi_proto_ProtobufPose2d;
  using InputStream = wpi::ProtoInputStream<frc::Pose2d>;
  using OutputStream = wpi::ProtoOutputStream<frc::Pose2d>;
  static std::optional<frc::Pose2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Pose2d& value);
};
