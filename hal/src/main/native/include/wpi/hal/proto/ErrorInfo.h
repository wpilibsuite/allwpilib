// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/util/protobuf/Protobuf.hpp"

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"

template <>
struct wpi::Protobuf<mrc::ErrorInfo> {
  using MessageStruct = mrc_proto_ProtobufErrorInfo;
  using InputStream = wpi::ProtoInputStream<mrc::ErrorInfo>;
  using OutputStream = wpi::ProtoOutputStream<mrc::ErrorInfo>;
  static std::optional<mrc::ErrorInfo> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::ErrorInfo& Value);
};
