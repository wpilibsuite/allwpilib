// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpi/math/geometry/Quaternion.h"
#include "wpimath/protobuf/geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::Quaternion> {
  using MessageStruct = wpi_proto_ProtobufQuaternion;
  using InputStream = wpi::ProtoInputStream<wpi::math::Quaternion>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::Quaternion>;
  static std::optional<wpi::math::Quaternion> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Quaternion& value);
};
