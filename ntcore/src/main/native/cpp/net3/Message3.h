// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>

#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"

namespace nt::net3 {

class WireDecoder3;

class Message3 {
  struct private_init {};
  friend class WireDecoder3;

 public:
  enum MsgType {
    kUnknown = -1,
    kKeepAlive = 0x00,
    kClientHello = 0x01,
    kProtoUnsup = 0x02,
    kServerHelloDone = 0x03,
    kServerHello = 0x04,
    kClientHelloDone = 0x05,
    kEntryAssign = 0x10,
    kEntryUpdate = 0x11,
    kFlagsUpdate = 0x12,
    kEntryDelete = 0x13,
    kClearEntries = 0x14,
    kExecuteRpc = 0x20,
    kRpcResponse = 0x21
  };
  enum DataType {
    kBoolean = 0x00,
    kDouble = 0x01,
    kString = 0x02,
    kRaw = 0x03,
    kBooleanArray = 0x10,
    kDoubleArray = 0x11,
    kStringArray = 0x12,
    kRpcDef = 0x20
  };
  static constexpr uint32_t kClearAllMagic = 0xD06CB27Aul;

  Message3() = default;
  Message3(MsgType type, const private_init&) : m_type(type) {}

  MsgType type() const { return m_type; }
  bool Is(MsgType type) const { return type == m_type; }

  // Message data accessors.  Callers are responsible for knowing what data is
  // actually provided for a particular message.
  std::string_view str() const { return m_str; }
  std::span<const uint8_t> bytes() const {
    return {reinterpret_cast<const uint8_t*>(m_str.data()), m_str.size()};
  }
  const Value& value() const { return m_value; }
  unsigned int id() const { return m_id; }
  unsigned int flags() const { return m_flags; }
  unsigned int seq_num_uid() const { return m_seq_num_uid; }

  void SetValue(const Value& value) { m_value = value; }

  // Create messages without data
  static Message3 KeepAlive() { return {kKeepAlive, {}}; }
  static Message3 ServerHelloDone() { return {kServerHelloDone, {}}; }
  static Message3 ClientHelloDone() { return {kClientHelloDone, {}}; }
  static Message3 ClearEntries() { return {kClearEntries, {}}; }

  // Create messages with data
  static Message3 ProtoUnsup(unsigned int proto_rev = 0x0300u) {
    Message3 msg{kProtoUnsup, {}};
    msg.m_id = proto_rev;
    return msg;
  }
  static Message3 ClientHello(std::string_view self_id,
                              unsigned int proto_rev = 0x0300u) {
    Message3 msg{kClientHello, {}};
    msg.m_str = self_id;
    msg.m_id = proto_rev;
    return msg;
  }
  static Message3 ServerHello(unsigned int flags, std::string_view self_id) {
    Message3 msg{kServerHello, {}};
    msg.m_str = self_id;
    msg.m_flags = flags;
    return msg;
  }
  static Message3 EntryAssign(std::string_view name, unsigned int id,
                              unsigned int seq_num, const Value& value,
                              unsigned int flags) {
    Message3 msg{kEntryAssign, {}};
    msg.m_str = name;
    msg.m_value = value;
    msg.m_id = id;
    msg.m_flags = flags;
    msg.m_seq_num_uid = seq_num;
    return msg;
  }
  static Message3 EntryUpdate(unsigned int id, unsigned int seq_num,
                              const Value& value) {
    Message3 msg{kEntryUpdate, {}};
    msg.m_value = value;
    msg.m_id = id;
    msg.m_seq_num_uid = seq_num;
    return msg;
  }
  static Message3 FlagsUpdate(unsigned int id, unsigned int flags) {
    Message3 msg{kFlagsUpdate, {}};
    msg.m_id = id;
    msg.m_flags = flags;
    return msg;
  }
  static Message3 EntryDelete(unsigned int id) {
    Message3 msg{kEntryDelete, {}};
    msg.m_id = id;
    return msg;
  }
  static Message3 ExecuteRpc(unsigned int id, unsigned int uid,
                             std::span<const uint8_t> params) {
    Message3 msg{kExecuteRpc, {}};
    msg.m_str.assign(reinterpret_cast<const char*>(params.data()),
                     params.size());
    msg.m_id = id;
    msg.m_seq_num_uid = uid;
    return msg;
  }
  static Message3 RpcResponse(unsigned int id, unsigned int uid,
                              std::span<const uint8_t> result) {
    Message3 msg{kRpcResponse, {}};
    msg.m_str.assign(reinterpret_cast<const char*>(result.data()),
                     result.size());
    msg.m_id = id;
    msg.m_seq_num_uid = uid;
    return msg;
  }

 private:
  MsgType m_type{kUnknown};

  // Message data.  Use varies by message type.
  std::string m_str;
  Value m_value;
  unsigned int m_id{0};  // also used for proto_rev
  unsigned int m_flags{0};
  unsigned int m_seq_num_uid{0};
};

}  // namespace nt::net3
