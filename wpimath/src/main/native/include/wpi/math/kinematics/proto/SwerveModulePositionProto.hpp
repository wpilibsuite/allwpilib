// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/kinematics/SwerveModulePosition.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/kinematics.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::SwerveModulePosition> {
  using MessageStruct = wpi_proto_ProtobufSwerveModulePosition;
  using InputStream = wpi::ProtoInputStream<wpi::math::SwerveModulePosition>;
  using OutputStream = wpi::ProtoOutputStream<wpi::math::SwerveModulePosition>;
  static std::optional<wpi::math::SwerveModulePosition> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::SwerveModulePosition& value);
};
