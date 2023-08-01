// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireDecoder3.h"

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <wpi/MathExtras.h>
#include <wpi/SpanExtras.h>
#include <wpi/leb128.h>

#include "Message3.h"

using namespace nt;
using namespace nt::net3;

namespace {

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

struct WDImpl {
  explicit WDImpl(MessageHandler3& out) : m_out{out} {}

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

  void Execute(std::span<const uint8_t>* in);

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

}  // namespace

static uint8_t Read8(std::span<const uint8_t>* in) {
  uint8_t val = in->front();
  *in = wpi::drop_front(*in);
  return val;
}

std::optional<uint16_t> SimpleValueReader::Read16(
    std::span<const uint8_t>* in) {
  while (!in->empty()) {
    m_value <<= 8;
    m_value |= in->front() & 0xff;
    *in = wpi::drop_front(*in);
    if (++m_count >= 2) {
      uint16_t val = static_cast<uint16_t>(m_value);
      m_count = 0;
      m_value = 0;
      return val;
    }
  }
  return std::nullopt;
}

std::optional<uint32_t> SimpleValueReader::Read32(
    std::span<const uint8_t>* in) {
  while (!in->empty()) {
    m_value <<= 8;
    m_value |= in->front() & 0xff;
    *in = wpi::drop_front(*in);
    if (++m_count >= 4) {
      uint32_t val = static_cast<uint32_t>(m_value);
      m_count = 0;
      m_value = 0;
      return val;
    }
  }
  return std::nullopt;
}

std::optional<uint64_t> SimpleValueReader::Read64(
    std::span<const uint8_t>* in) {
  while (!in->empty()) {
    m_value <<= 8;
    m_value |= in->front() & 0xff;
    *in = wpi::drop_front(*in);
    if (++m_count >= 8) {
      uint64_t val = m_value;
      m_count = 0;
      m_value = 0;
      return val;
    }
  }
  return std::nullopt;
}

std::optional<double> SimpleValueReader::ReadDouble(
    std::span<const uint8_t>* in) {
  if (auto val = Read64(in)) {
    return wpi::BitsToDouble(val.value());
  } else {
    return std::nullopt;
  }
}

void WDImpl::Execute(std::span<const uint8_t>* in) {
  while (!in->empty()) {
    switch (m_state) {
      case kStart: {
        uint8_t msgType = Read8(in);
        switch (msgType) {
          case Message3::kKeepAlive:
            m_out.KeepAlive();
            break;
          case Message3::kClientHello:
            m_state = kClientHello_1ProtoRev;
            break;
          case Message3::kProtoUnsup:
            m_state = kProtoUnsup_1ProtoRev;
            break;
          case Message3::kServerHello:
            m_state = kServerHello_1Flags;
            break;
          case Message3::kServerHelloDone:
            m_out.ServerHelloDone();
            break;
          case Message3::kClientHelloDone:
            m_out.ClientHelloDone();
            break;
          case Message3::kEntryAssign:
            m_state = kEntryAssign_1Name;
            break;
          case Message3::kEntryUpdate:
            m_state = kEntryUpdate_1Id;
            break;
          case Message3::kFlagsUpdate:
            m_state = kFlagsUpdate_1Id;
            break;
          case Message3::kEntryDelete:
            m_state = kEntryDelete_1Id;
            break;
          case Message3::kClearEntries:
            m_state = kClearEntries_1Magic;
            break;
          case Message3::kExecuteRpc:
            m_state = kExecuteRpc_1Id;
            break;
          case Message3::kRpcResponse:
            m_state = kRpcResponse_1Id;
            break;
          default:
            EmitError(fmt::format("unrecognized message type: {}",
                                  static_cast<uint32_t>(msgType)));
            return;
        }
        break;
      }
      case kClientHello_1ProtoRev:
        if (auto val = m_simpleReader.Read16(in)) {
          if (val < 0x0300u) {
            m_state = kStart;
            m_out.ClientHello("", val.value());
          } else {
            m_state = kClientHello_2Id;
            m_id = val.value();
          }
        }
        break;
      case kClientHello_2Id:
        if (auto val = ReadString(in)) {
          m_state = kStart;
          m_out.ClientHello(val.value(), m_id);
        }
        break;
      case kProtoUnsup_1ProtoRev:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kStart;
          m_out.ProtoUnsup(val.value());
        }
        break;
      case kServerHello_1Flags: {
        m_state = kServerHello_2Id;
        m_flags = Read8(in);
        break;
      }
      case kServerHello_2Id:
        if (auto val = ReadString(in)) {
          m_state = kStart;
          m_out.ServerHello(m_flags, val.value());
        }
        break;
      case kEntryAssign_1Name:
        if (auto val = ReadString(in)) {
          m_state = kEntryAssign_2Type;
          m_str = std::move(val.value());
        }
        break;
      case kEntryAssign_2Type:
        if (auto val = ReadType(in)) {
          m_state = kEntryAssign_3Id;
          m_valueReader = ValueReader{val.value()};
        }
        break;
      case kEntryAssign_3Id:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kEntryAssign_4SeqNum;
          m_id = val.value();
        }
        break;
      case kEntryAssign_4SeqNum:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kEntryAssign_5Flags;
          m_seq_num_uid = val.value();
        }
        break;
      case kEntryAssign_5Flags: {
        m_state = kEntryAssign_6Value;
        m_flags = Read8(in);
        break;
      }
      case kEntryAssign_6Value:
        if (auto val = ReadValue(in)) {
          m_state = kStart;
          m_out.EntryAssign(m_str, m_id, m_seq_num_uid, val.value(), m_flags);
        }
        break;
      case kEntryUpdate_1Id:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kEntryUpdate_2SeqNum;
          m_id = val.value();
        }
        break;
      case kEntryUpdate_2SeqNum:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kEntryUpdate_3Type;
          m_seq_num_uid = val.value();
        }
        break;
      case kEntryUpdate_3Type:
        if (auto val = ReadType(in)) {
          m_state = kEntryUpdate_4Value;
          m_valueReader = ValueReader{val.value()};
        }
        break;
      case kEntryUpdate_4Value:
        if (auto val = ReadValue(in)) {
          m_state = kStart;
          m_out.EntryUpdate(m_id, m_seq_num_uid, val.value());
        }
        break;
      case kFlagsUpdate_1Id:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kFlagsUpdate_2Flags;
          m_id = val.value();
        }
        break;
      case kFlagsUpdate_2Flags: {
        m_state = kStart;
        m_out.FlagsUpdate(m_id, Read8(in));
        break;
      }
      case kEntryDelete_1Id:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kStart;
          m_out.EntryDelete(val.value());
        }
        break;
      case kClearEntries_1Magic:
        if (auto val = m_simpleReader.Read32(in)) {
          m_state = kStart;
          if (val.value() == Message3::kClearAllMagic) {
            m_out.ClearEntries();
          } else {
            EmitError("received incorrect CLEAR_ENTRIES magic value");
          }
          break;
        }
        break;
      case kExecuteRpc_1Id:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kExecuteRpc_2Uid;
          m_id = val.value();
        }
        break;
      case kExecuteRpc_2Uid:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kExecuteRpc_3Params;
          m_seq_num_uid = val.value();
        }
        break;
      case kExecuteRpc_3Params:
        if (auto val = ReadRaw(in)) {
          m_state = kStart;
          m_out.ExecuteRpc(m_id, m_seq_num_uid, val.value());
        }
        break;
      case kRpcResponse_1Id:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kRpcResponse_2Uid;
          m_id = val.value();
        }
        break;
      case kRpcResponse_2Uid:
        if (auto val = m_simpleReader.Read16(in)) {
          m_state = kRpcResponse_3Result;
          m_seq_num_uid = val.value();
        }
        break;
      case kRpcResponse_3Result:
        if (auto val = ReadRaw(in)) {
          m_state = kStart;
          m_out.RpcResponse(m_id, m_seq_num_uid, val.value());
        }
        break;
      case kError:
        return;
    }
  }
}

std::optional<std::string> WDImpl::ReadString(std::span<const uint8_t>* in) {
  // string length
  if (!m_stringReader.len) {
    if (auto val = m_ulebReader.ReadOne(in)) {
      m_stringReader.SetLen(val.value());
      m_stringReader.buf.clear();
    } else {
      return std::nullopt;
    }
  }

  // string data; nolint to avoid clang-tidy false positive
  size_t toCopy =
      (std::min)(in->size(),
                 static_cast<size_t>(m_stringReader.len.value() -
                                     m_stringReader.buf.size()));  // NOLINT
  m_stringReader.buf.append(reinterpret_cast<const char*>(in->data()), toCopy);
  *in = wpi::drop_front(*in, toCopy);
  if (m_stringReader.buf.size() >= m_stringReader.len) {
    m_stringReader.len.reset();
    return std::move(m_stringReader.buf);
  }
  return std::nullopt;
}

std::optional<std::vector<uint8_t>> WDImpl::ReadRaw(
    std::span<const uint8_t>* in) {
  // string length
  if (!m_rawReader.len) {
    if (auto val = m_ulebReader.ReadOne(in)) {
      m_rawReader.SetLen(val.value());
      m_rawReader.buf.clear();
    } else {
      return std::nullopt;
    }
  }

  // string data
  size_t toCopy = (std::min)(
      static_cast<size_t>(in->size()),
      static_cast<size_t>(m_rawReader.len.value() - m_rawReader.buf.size()));
  m_rawReader.buf.insert(m_rawReader.buf.end(), in->begin(),
                         in->begin() + toCopy);
  *in = wpi::drop_front(*in, toCopy);
  if (m_rawReader.buf.size() >= m_rawReader.len) {
    m_rawReader.len.reset();
    return std::move(m_rawReader.buf);
  }
  return std::nullopt;
}

std::optional<NT_Type> WDImpl::ReadType(std::span<const uint8_t>* in) {
  // Convert from byte value to enum
  switch (Read8(in)) {
    case Message3::kBoolean:
      return NT_BOOLEAN;
    case Message3::kDouble:
      return NT_DOUBLE;
    case Message3::kString:
      return NT_STRING;
    case Message3::kRaw:
      return NT_RAW;
    case Message3::kBooleanArray:
      return NT_BOOLEAN_ARRAY;
    case Message3::kDoubleArray:
      return NT_DOUBLE_ARRAY;
    case Message3::kStringArray:
      return NT_STRING_ARRAY;
    case Message3::kRpcDef:
      return NT_RPC;
    default:
      return EmitError("unrecognized value type");
  }
}

std::optional<Value> WDImpl::ReadValue(std::span<const uint8_t>* in) {
  while (!in->empty()) {
    switch (m_valueReader.type) {
      case NT_BOOLEAN:
        return Value::MakeBoolean(Read8(in) != 0);
      case NT_DOUBLE:
        if (auto val = m_simpleReader.ReadDouble(in)) {
          return Value::MakeDouble(val.value());
        }
        break;
      case NT_STRING:
        if (auto val = ReadString(in)) {
          return Value::MakeString(std::move(val.value()));
        }
        break;
      case NT_RAW:
      case NT_RPC:
        if (auto val = ReadRaw(in)) {
          return Value::MakeRaw(std::move(val.value()));
        }
        break;
#if 0
      case NT_RPC:
        if (auto val = ReadRaw(in)) {
          return Value::MakeRpc(std::move(val.value()));
        }
        break;
#endif
      case NT_BOOLEAN_ARRAY:
        // size
        if (!m_valueReader.haveSize) {
          m_valueReader.SetSize(Read8(in));
          break;
        }

        // array values
        while (!in->empty() && m_valueReader.ints.size() < m_valueReader.size) {
          m_valueReader.ints.emplace_back(Read8(in) ? 1 : 0);
        }
        if (m_valueReader.ints.size() == m_valueReader.size) {
          return Value::MakeBooleanArray(std::move(m_valueReader.ints));
        }
        break;
      case NT_DOUBLE_ARRAY:
        // size
        if (!m_valueReader.haveSize) {
          m_valueReader.SetSize(Read8(in));
          break;
        }

        // array values
        while (!in->empty() &&
               m_valueReader.doubles.size() < m_valueReader.size) {
          if (auto val = m_simpleReader.ReadDouble(in)) {
            m_valueReader.doubles.emplace_back(std::move(val.value()));
          }
        }
        if (m_valueReader.doubles.size() == m_valueReader.size) {
          return Value::MakeDoubleArray(std::move(m_valueReader.doubles));
        }
        break;
      case NT_STRING_ARRAY:
        // size
        if (!m_valueReader.haveSize) {
          m_valueReader.SetSize(Read8(in));
          break;
        }

        // array values
        while (!in->empty() &&
               m_valueReader.strings.size() < m_valueReader.size) {
          if (auto val = ReadString(in)) {
            m_valueReader.strings.emplace_back(std::move(val.value()));
          }
        }
        if (m_valueReader.strings.size() == m_valueReader.size) {
          return Value::MakeStringArray(std::move(m_valueReader.strings));
        }
        break;
      default:
        return EmitError("invalid type when trying to read value");
    }
  }
  return std::nullopt;
}

struct WireDecoder3::Impl : public WDImpl {
  explicit Impl(MessageHandler3& out) : WDImpl{out} {}
};

WireDecoder3::WireDecoder3(MessageHandler3& out) : m_impl{new Impl{out}} {}

WireDecoder3::~WireDecoder3() = default;

bool WireDecoder3::Execute(std::span<const uint8_t>* in) {
  m_impl->Execute(in);
  return m_impl->m_state != Impl::kError;
}

void WireDecoder3::SetError(std::string_view message) {
  m_impl->EmitError(message);
}

std::string WireDecoder3::GetError() const {
  return m_impl->m_error;
}
