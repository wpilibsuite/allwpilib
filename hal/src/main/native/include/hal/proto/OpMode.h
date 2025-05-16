#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <stdint.h>
#include "mrc/NetComm.h"
#include "MrcComm.npb.h"

template <>
struct wpi::Protobuf<mrc::OpMode> {
  using MessageStruct = mrc_proto_ProtobufOpMode;
  using InputStream = wpi::ProtoInputStream<mrc::OpMode>;
  using OutputStream = wpi::ProtoOutputStream<mrc::OpMode>;
  static std::optional<mrc::OpMode> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const mrc::OpMode& Value);
};

template <>
struct wpi::Protobuf<std::vector<mrc::OpMode>> {
  using MessageStruct = mrc_proto_ProtobufAvailableOpModes;
  using InputStream = wpi::ProtoInputStream<std::vector<mrc::OpMode>>;
  using OutputStream = wpi::ProtoOutputStream<std::vector<mrc::OpMode>>;
  static std::optional<std::vector<mrc::OpMode>> Unpack(InputStream& Stream);
  static bool Pack(OutputStream& Stream, const std::vector<mrc::OpMode>& Value);
};
