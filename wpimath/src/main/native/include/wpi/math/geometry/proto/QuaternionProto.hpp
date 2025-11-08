// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Quaternion.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Quaternion> {
  using MessageStruct = wpi_proto_ProtobufQuaternion;
  using InputStream = wpi::ProtoInputStream<frc::Quaternion>;
  using OutputStream = wpi::ProtoOutputStream<frc::Quaternion>;
  static std::optional<frc::Quaternion> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Quaternion& value);
};
