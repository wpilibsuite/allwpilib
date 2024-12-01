// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/protobuf/Protobuf.h>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"

template <>
struct wpi::Protobuf<mrc::JoystickOutputData> {
  using MessageStruct = mrc_proto_ProtobufJoystickOutputData;
  using InputStream = wpi::ProtoInputStream<mrc::JoystickOutputData>;
  using OutputStream = wpi::ProtoOutputStream<mrc::JoystickOutputData>;
  static std::optional<mrc::JoystickOutputData> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickOutputData& Value);
};
