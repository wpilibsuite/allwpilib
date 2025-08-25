// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/SwerveModuleAccelerations.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::SwerveModuleAccelerations> {
  using MessageStruct = wpi_proto_ProtobufSwerveModuleAccelerations;
  using InputStream = wpi::ProtoInputStream<frc::SwerveModuleAccelerations>;
  using OutputStream = wpi::ProtoOutputStream<frc::SwerveModuleAccelerations>;
  static std::optional<frc::SwerveModuleAccelerations> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::SwerveModuleAccelerations& value);
};
