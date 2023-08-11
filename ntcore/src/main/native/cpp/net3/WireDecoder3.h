// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <wpi/leb128.h>

#include "ntcore_c.h"

namespace nt {
class Value;
}  // namespace nt

namespace nt::net3 {

class MessageHandler3 {
 public:
  virtual ~MessageHandler3() = default;

  virtual void KeepAlive() = 0;
  virtual void ServerHelloDone() = 0;
  virtual void ClientHelloDone() = 0;
  virtual void ClearEntries() = 0;
  virtual void ProtoUnsup(unsigned int proto_rev) = 0;
  virtual void ClientHello(std::string_view self_id,
                           unsigned int proto_rev) = 0;
  virtual void ServerHello(unsigned int flags, std::string_view self_id) = 0;
  virtual void EntryAssign(std::string_view name, unsigned int id,
                           unsigned int seq_num, const Value& value,
                           unsigned int flags) = 0;
  virtual void EntryUpdate(unsigned int id, unsigned int seq_num,
                           const Value& value) = 0;
  virtual void FlagsUpdate(unsigned int id, unsigned int flags) = 0;
  virtual void EntryDelete(unsigned int id) = 0;
  virtual void ExecuteRpc(unsigned int id, unsigned int uid,
                          std::span<const uint8_t> params) = 0;
  virtual void RpcResponse(unsigned int id, unsigned int uid,
                           std::span<const uint8_t> result) = 0;
};

/* Decodes NT3 protocol into native representation. */
class WireDecoder3 {
 public:
  explicit WireDecoder3(MessageHandler3& out) : m_out{out} {}

  /**
   * Executes the decoder.  All input data will be consumed unless an error
   * occurs.
   * @param in input data (updated during parse)
   * @return false if error occurred
   */
  bool Execute(std::span<const uint8_t>* in) {
    DoExecute(in);
    return m_state != kError;
  }

  void SetError(std::string_view message) { EmitError(message); }
  std::string GetError() const { return m_error; }

 private:
  class SimpleValueReader {
   public:
    std::optional<uint16_t> Read16(std::span<const uint8_t>* in);
    std::optional<uint32_t> Read32(std::span<const uint8_t>* in);
    std::optional<uint64_t> Read64(std::span<const uint8_t>* in);
    std::optional<double> ReadDouble(std::span<const uint8_t>* in);

   private:
    uint64_t m_value = 0;
    int m_count = 0;
  };

  struct StringReader {
    void SetLen(uint64_t len_) {
      len = len_;
      buf.clear();
    }

    std::optional<uint64_t> len;
    std::string buf;
  };

  struct RawReader {
    void SetLen(uint64_t len_) {
      len = len_;
      buf.clear();
    }

    std::optional<uint64_t> len;
    std::vector<uint8_t> buf;
  };

  struct ValueReader {
    ValueReader() = default;
    explicit ValueReader(NT_Type type_) : type{type_} {}

    void SetSize(uint32_t size_) {
      haveSize = true;
      size = size_;
      ints.clear();
      doubles.clear();
      strings.clear();
    }

    NT_Type type = NT_UNASSIGNED;
    bool haveSize = false;
    uint32_t size = 0;
    std::vector<int> ints;
    std::vector<double> doubles;
    std::vector<std::string> strings;
  };

  MessageHandler3& m_out;

  // primary (message) decode state
  enum {
    kStart,
    kClientHello_1ProtoRev,
    kClientHello_2Id,
    kProtoUnsup_1ProtoRev,
    kServerHello_1Flags,
    kServerHello_2Id,
    kEntryAssign_1Name,
    kEntryAssign_2Type,
    kEntryAssign_3Id,
    kEntryAssign_4SeqNum,
    kEntryAssign_5Flags,
    kEntryAssign_6Value,
    kEntryUpdate_1Id,
    kEntryUpdate_2SeqNum,
    kEntryUpdate_3Type,
    kEntryUpdate_4Value,
    kFlagsUpdate_1Id,
    kFlagsUpdate_2Flags,
    kEntryDelete_1Id,
    kClearEntries_1Magic,
    kExecuteRpc_1Id,
    kExecuteRpc_2Uid,
    kExecuteRpc_3Params,
    kRpcResponse_1Id,
    kRpcResponse_2Uid,
    kRpcResponse_3Result,
    kError
  } m_state = kStart;

  // detail decoders
  wpi::Uleb128Reader m_ulebReader;
  SimpleValueReader m_simpleReader;
  StringReader m_stringReader;
  RawReader m_rawReader;
  ValueReader m_valueReader;

  std::string m_error;

  std::string m_str;
  unsigned int m_id{0};  // also used for proto_rev
  unsigned int m_flags{0};
  unsigned int m_seq_num_uid{0};

  void DoExecute(std::span<const uint8_t>* in);

  std::nullopt_t EmitError(std::string_view msg) {
    m_state = kError;
    m_error = msg;
    return std::nullopt;
  }

  std::optional<std::string> ReadString(std::span<const uint8_t>* in);
  std::optional<std::vector<uint8_t>> ReadRaw(std::span<const uint8_t>* in);
  std::optional<NT_Type> ReadType(std::span<const uint8_t>* in);
  std::optional<Value> ReadValue(std::span<const uint8_t>* in);
};

}  // namespace nt::net3
