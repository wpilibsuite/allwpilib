// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/SwerveModulePosition.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::SwerveModulePosition> {
  using MessageStruct = wpi_proto_ProtobufSwerveModulePosition;
  using InputStream = wpi::ProtoInputStream<wpimath::SwerveModulePosition>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::SwerveModulePosition>;
  static std::optional<wpimath::SwerveModulePosition> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::SwerveModulePosition& value);
};
