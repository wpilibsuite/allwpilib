// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpimath/protobuf/protobuf_kinematics.npb.h"

template <>
struct WPILIB_DLLEXPORT
    wpi::util::Protobuf<wpi::math::MecanumDriveWheelAccelerations> {
  using MessageStruct = wpi_proto_ProtobufMecanumDriveWheelAccelerations;
  using InputStream =
      wpi::util::ProtoInputStream<wpi::math::MecanumDriveWheelAccelerations>;
  using OutputStream =
      wpi::util::ProtoOutputStream<wpi::math::MecanumDriveWheelAccelerations>;
  static std::optional<wpi::math::MecanumDriveWheelAccelerations> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpi::math::MecanumDriveWheelAccelerations& value);
};
