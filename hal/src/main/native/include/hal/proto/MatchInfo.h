#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <stdint.h>
#include "mrc/NetComm.h"
#include "MrcComm.npb.h"

template <>
struct wpi::Protobuf<mrc::MatchInfo> {
  using MessageStruct = mrc_proto_ProtobufMatchInfo;
  using InputStream = wpi::ProtoInputStream<mrc::MatchInfo>;
  using OutputStream = wpi::ProtoOutputStream<mrc::MatchInfo>;
  static std::optional<mrc::MatchInfo> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::MatchInfo& Value);
};
