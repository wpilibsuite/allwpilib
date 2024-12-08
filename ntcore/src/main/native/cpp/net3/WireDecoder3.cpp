// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireDecoder3.h"

#include <algorithm>
#include <bit>
#include <string>
#include <utility>

#include <fmt/format.h>
#include <wpi/SpanExtras.h>

#include "Message3.h"

using namespace nt;
using namespace nt::net3;

static uint8_t Read8(std::span<const uint8_t>* in) {
  uint8_t val = in->front();
  *in = wpi::drop_front(*in);
  return val;
}

std::optional<uint16_t> WireDecoder3::SimpleValueReader::Read16(
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

std::optional<uint32_t> WireDecoder3::SimpleValueReader::Read32(
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

std::optional<uint64_t> WireDecoder3::SimpleValueReader::Read64(
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

std::optional<double> WireDecoder3::SimpleValueReader::ReadDouble(
    std::span<const uint8_t>* in) {
  if (auto val = Read64(in)) {
    return std::bit_cast<double>(val.value());
  } else {
    return std::nullopt;
  }
}

void WireDecoder3::DoExecute(std::span<const uint8_t>* in) {
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

std::optional<std::string> WireDecoder3::ReadString(
    std::span<const uint8_t>* in) {
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

std::optional<std::vector<uint8_t>> WireDecoder3::ReadRaw(
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

std::optional<NT_Type> WireDecoder3::ReadType(std::span<const uint8_t>* in) {
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

std::optional<Value> WireDecoder3::ReadValue(std::span<const uint8_t>* in) {
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
