#pragma once

#include <wpi/protobuf/Protobuf.h>
#include <stdint.h>
#include "mrc/NetComm.h"
#include "MrcComm.npb.h"

template <>
struct wpi::Protobuf<mrc::VersionInfo> {
    using MessageStruct = mrc_proto_ProtobufVersionInfo;
    using InputStream = wpi::ProtoInputStream<mrc::VersionInfo>;
    using OutputStream = wpi::ProtoOutputStream<mrc::VersionInfo>;
    static std::optional<mrc::VersionInfo> Unpack(InputStream& Stream);
    static bool Pack(OutputStream& Stream, const mrc::VersionInfo& Value);
};
