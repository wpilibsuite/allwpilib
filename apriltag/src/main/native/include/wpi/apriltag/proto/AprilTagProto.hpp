// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "apriltag/protobuf/apriltag.npb.h"
#include "wpi/apriltag/AprilTag.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct WPILIB_DLLEXPORT wpi::util::Protobuf<wpi::apriltag::AprilTag> {
  using MessageStruct = wpi_proto_ProtobufAprilTag;
  using InputStream = wpi::util::ProtoInputStream<wpi::apriltag::AprilTag>;
  using OutputStream = wpi::util::ProtoOutputStream<wpi::apriltag::AprilTag>;
  static std::optional<wpi::apriltag::AprilTag> Unpack(InputStream& stream);
  static bool Pack(OutputStream& stream, const wpi::apriltag::AprilTag& value);
};
