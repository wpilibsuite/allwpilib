// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/kinematics/ChassisSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::ChassisSpeeds> {
  using MessageStruct = wpi_proto_ProtobufChassisSpeeds;
  using InputStream = wpi::ProtoInputStream<wpimath::ChassisSpeeds>;
  using OutputStream = wpi::ProtoOutputStream<wpimath::ChassisSpeeds>;
  static std::optional<wpimath::ChassisSpeeds> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpimath::ChassisSpeeds& value);
};
