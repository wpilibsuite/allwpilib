// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/protobuf_kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::ChassisSpeeds> {
  using MessageStruct = wpi_proto_ProtobufChassisSpeeds;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::ChassisSpeeds>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::ChassisSpeeds>;
  static std::optional<wpi::math::ChassisSpeeds> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::math::ChassisSpeeds& value);
};
