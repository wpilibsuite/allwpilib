// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/SwerveModuleState.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::SwerveModuleState> {
  using MessageStruct = wpi_proto_ProtobufSwerveModuleState;
  using InputStream = wpi::ProtoInputStream<wpimath::SwerveModuleState>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::SwerveModuleState>;
  static std::optional<wpimath::SwerveModuleState> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::SwerveModuleState& value);
};
