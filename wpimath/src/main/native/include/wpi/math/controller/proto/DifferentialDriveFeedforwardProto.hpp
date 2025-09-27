// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/protobuf/Protobuf.h>

#include "frc/controller/DifferentialDriveFeedforward.h"
#include "pb.h"
#include "wpimath/protobuf/controller.npb.h"

template <>
struct WPILIB_DLLEXPORT wpi::Protobuf<frc::DifferentialDriveFeedforward> {
  using MessageStruct = wpi_proto_ProtobufDifferentialDriveFeedforward;
  using InputStream = wpi::ProtoInputStream<frc::DifferentialDriveFeedforward>;
  using OutputStream =
      wpi::ProtoOutputStream<frc::DifferentialDriveFeedforward>;
  static std::optional<frc::DifferentialDriveFeedforward> Unpack(
      InputStream& stream);
  static bool Pack(OutputStream& stream,
                   const frc::DifferentialDriveFeedforward& value);
};
