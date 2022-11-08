// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string_view>

namespace wpi {
class raw_ostream;
}  // namespace wpi

namespace nt {
class Value;
}  // namespace nt

namespace nt::net3 {

class Message3;

// encoders for messages (avoids need to construct a Message struct)
void WireEncodeKeepAlive(wpi::raw_ostream& os);
void WireEncodeServerHelloDone(wpi::raw_ostream& os);
void WireEncodeClientHelloDone(wpi::raw_ostream& os);
void WireEncodeClearEntries(wpi::raw_ostream& os);
void WireEncodeProtoUnsup(wpi::raw_ostream& os, unsigned int proto_rev);
void WireEncodeClientHello(wpi::raw_ostream& os, std::string_view self_id,
                           unsigned int proto_rev);
void WireEncodeServerHello(wpi::raw_ostream& os, unsigned int flags,
                           std::string_view self_id);
bool WireEncodeEntryAssign(wpi::raw_ostream& os, std::string_view name,
                           unsigned int id, unsigned int seq_num,
                           const Value& value, unsigned int flags);
bool WireEncodeEntryUpdate(wpi::raw_ostream& os, unsigned int id,
                           unsigned int seq_num, const Value& value);
void WireEncodeFlagsUpdate(wpi::raw_ostream& os, unsigned int id,
                           unsigned int flags);
void WireEncodeEntryDelete(wpi::raw_ostream& os, unsigned int id);
void WireEncodeExecuteRpc(wpi::raw_ostream& os, unsigned int id,
                          unsigned int uid, std::span<const uint8_t> params);
void WireEncodeRpcResponse(wpi::raw_ostream& os, unsigned int id,
                           unsigned int uid, std::span<const uint8_t> result);

bool WireEncode(wpi::raw_ostream& os, const Message3& msg);

}  // namespace nt::net3
