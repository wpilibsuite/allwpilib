// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/SwerveModuleState.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::SwerveModuleState> {
  using MessageStruct = wpi_proto_ProtobufSwerveModuleState;
  using InputStream = wpi::ProtoInputStream<frc::SwerveModuleState>;
  using OutputStream = wpi::ProtoOutputStream<frc::SwerveModuleState>;
  static std::optional<frc::SwerveModuleState> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::SwerveModuleState& value);
};
