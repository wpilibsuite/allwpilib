// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/util/protobuf/Protobuf.hpp"

#include "MrcComm.npb.h"
#include "mrc/NetComm.h"

template <>
struct wpi::Protobuf<mrc::MatchInfo> {
  using MessageStruct = mrc_proto_ProtobufMatchInfo;
  using InputStream = wpi::ProtoInputStream<mrc::MatchInfo>;
  using OutputStream = wpi::ProtoOutputStream<mrc::MatchInfo>;
  static std::optional<mrc::MatchInfo> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::MatchInfo& Value);
};
