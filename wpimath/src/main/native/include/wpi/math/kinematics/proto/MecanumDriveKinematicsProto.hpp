// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::math::MecanumDriveKinematics> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveKinematics;
  using InputStream = wpi::util::ProtoInputStream<wpi::math::MecanumDriveKinematics>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::math::MecanumDriveKinematics>;
  static std::optional<wpi::math::MecanumDriveKinematics> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::MecanumDriveKinematics& value);
};
