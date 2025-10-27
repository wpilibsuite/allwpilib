// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/util/protobuf/Protobuf.hpp>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"

template <>
struct wpi::util::Protobuf<mrc::JoystickDescriptor> {
  using MessageStruct = mrc_proto_ProtobufJoystickDescriptor;
  using InputStream = wpi::util::ProtoInputStream<mrc::JoystickDescriptor>;
  using OutputStream = wpi::util::ProtoOutputStream<mrc::JoystickDescriptor>;
  static std::optional<mrc::JoystickDescriptor> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::JoystickDescriptor& Value);
};
