// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"
#include "wpi/util/protobuf/Protobuf.hpp"

template <>
struct wpi::Protobuf<mrc::VersionInfo> {
  using MessageStruct = mrc_proto_ProtobufVersionInfo;
  using InputStream = wpi::ProtoInputStream<mrc::VersionInfo>;
  using OutputStream = wpi::ProtoOutputStream<mrc::VersionInfo>;
  static std::optional<mrc::VersionInfo> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::VersionInfo& Value);
};
