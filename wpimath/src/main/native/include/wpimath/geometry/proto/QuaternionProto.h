// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/geometry/Quaternion.h"
#include "wpimath/protobuf/geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::Quaternion> {
  using MessageStruct = wpi_proto_ProtobufQuaternion;
  using InputStream = wpi::ProtoInputStream<wpimath::Quaternion>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::Quaternion>;
  static std::optional<wpimath::Quaternion> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::Quaternion& value);
};
