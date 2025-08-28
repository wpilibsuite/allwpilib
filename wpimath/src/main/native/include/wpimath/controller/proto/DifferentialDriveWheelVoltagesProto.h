// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "pb.h"
#include "wpimath/controller/DifferentialDriveWheelVoltages.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<wpimath::DifferentialDriveWheelVoltages> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveWheelVoltages;
  using InputStream =
      wpi::ProtoInputStream<wpimath::DifferentialDriveWheelVoltages>;
  using OutputStream =
      wpi::ProtoOutputStream<wpimath::DifferentialDriveWheelVoltages>;
  static std::optional<wpimath::DifferentialDriveWheelVoltages> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const wpimath::DifferentialDriveWheelVoltages& value);
};
