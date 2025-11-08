// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "pb.h"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/geometry2d.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::Translation2d> {
  using MessageStruct = wpi_proto_ProtobufTranslation2d;
  using InputStream = wpi::ProtoInputStream<frc::Translation2d>;
  using OutputStream = wpi::ProtoOutputStream<frc::Translation2d>;
  static std::optional<frc::Translation2d> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::Translation2d& value);
};
