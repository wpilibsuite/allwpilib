// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/kinematics/ChassisSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::ChassisSpeeds> {
  using MessageStruct = wpi_proto_ProtobufChassisSpeeds;
  using InputStream = wpi::ProtoInputStream<frc::ChassisSpeeds>;
  using OutputStream = wpi::ProtoOutputStream<frc::ChassisSpeeds>;
  static std::optional<frc::ChassisSpeeds> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const frc::ChassisSpeeds& value);
};
