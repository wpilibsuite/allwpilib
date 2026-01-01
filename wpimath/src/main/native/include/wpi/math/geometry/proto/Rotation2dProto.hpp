// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/protobuf_geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::Rotation2d> {
  using MessageStruct = wpi_proto_ProtobufRotation2d;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::Rotation2d>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::Rotation2d>;
  static std::optional<wpi::math::Rotation2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Rotation2d& value);
};
