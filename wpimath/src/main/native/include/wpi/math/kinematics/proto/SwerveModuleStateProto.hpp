// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/SwerveModuleState.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/protobuf_kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::SwerveModuleState> {
  using MessageStruct = wpi_proto_ProtobufSwerveModuleState;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::SwerveModuleState>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::SwerveModuleState>;
  static std::optional<wpi::math::SwerveModuleState> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::SwerveModuleState& value);
};
