// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpi/math/kinematics/SwerveModuleState.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::SwerveModuleState> {
  using MessageStruct = wpi_proto_ProtobufSwerveModuleState;
  using InputStream = wpi::ProtoInputStream<wpi::math::SwerveModuleState>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::SwerveModuleState>;
  static std::optional<wpi::math::SwerveModuleState> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::SwerveModuleState& value);
};
