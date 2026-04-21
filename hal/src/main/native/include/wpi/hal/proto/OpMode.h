// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <vector>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct wpi::util::Protobuf<mrc::OpMode> {
  using MessageStruct = mrc_proto_ProtobufOpMode;
  using InputStream = wpi::util::ProtoInputStream<mrc::OpMode>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::OpMode>;
  static std::optional<mrc::OpMode> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::OpMode& Value);
};

template <>
struct wpi::util::Protobuf<std::vector<mrc::OpMode>> {
  using MessageStruct = mrc_proto_ProtobufAvailableOpModes;
  using InputStream = wpi::util::ProtoInputStream<std::vector<mrc::OpMode>>;
  using OutputStream = wpi::util::ProtoOutputStream<std::vector<mrc::OpMode>>;
  static std::optional<std::vector<mrc::OpMode>> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const std::vector<mrc::OpMode>& Value);
};
