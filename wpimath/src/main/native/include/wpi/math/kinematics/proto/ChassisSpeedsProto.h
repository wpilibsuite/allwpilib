// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "wpi/math/kinematics/ChassisSpeeds.h"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::ChassisSpeeds> {
  using MessageStruct = wpi_proto_ProtobufChassisSpeeds;
  using InputStream = wpi::ProtoInputStream<wpi::math::ChassisSpeeds>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::ChassisSpeeds>;
  static std::optional<wpi::math::ChassisSpeeds> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::ChassisSpeeds& value);
};
