// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Quaternion.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/geometry3d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::Quaternion> {
  using MessageStruct = wpi_proto_ProtobufQuaternion;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::Quaternion>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::Quaternion>;
  static std::optional<wpi::math::Quaternion> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::Quaternion& value);
};
