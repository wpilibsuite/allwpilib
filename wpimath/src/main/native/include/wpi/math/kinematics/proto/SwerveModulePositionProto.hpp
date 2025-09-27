// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/SwerveModulePosition.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::SwerveModulePosition> {
  using MessageStruct = wpi_proto_ProtobufSwerveModulePosition;
  using InputStream = wpi::ProtoInputStream<frc::SwerveModulePosition>;
  using OutputStream = wpi::ProtoOutputStream<frc::SwerveModulePosition>;
  static std::optional<frc::SwerveModulePosition> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::SwerveModulePosition& value);
};
