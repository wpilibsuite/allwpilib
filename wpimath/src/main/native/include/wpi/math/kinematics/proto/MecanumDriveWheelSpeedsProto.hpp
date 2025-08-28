// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp>
#include <wpi/protobuf/Protobuf.h>

#include "wpimath/protobuf/kinematics.npb.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpi::math::MecanumDriveWheelSpeeds> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelSpeeds;
  using InputStream = wpi::ProtoInputStream<wpi::math::MecanumDriveWheelSpeeds>;
  using OutputStream =
      wpi::ProtoOutputStream<wpi::math::MecanumDriveWheelSpeeds>;
  static std::optional<wpi::math::MecanumDriveWheelSpeeds> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::MecanumDriveWheelSpeeds& value);
};
