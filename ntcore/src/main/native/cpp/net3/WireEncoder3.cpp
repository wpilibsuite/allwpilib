// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "WireEncoder3.h"

#include <bit>

#include <wpi/Endian.h>
#include <wpi/SmallVector.h>
#include <wpi/leb128.h>
#include <wpi/raw_ostream.h>

#include "Message3.h"

using namespace nt;
using namespace nt::net3;

static void Write8(wpi::raw_ostream& os, uint8_t val) {
  os << val;
}

static void Write16(wpi::raw_ostream& os, uint16_t val) {
  uint8_t buf[2];
  wpi::support::endian::write16be(buf, val);
  os << buf;
}

static void Write32(wpi::raw_ostream& os, uint32_t val) {
  uint8_t buf[4];
  wpi::support::endian::write32be(buf, val);
  os << buf;
}

static void WriteDouble(wpi::raw_ostream& os, double val) {
  uint8_t buf[8];
  wpi::support::endian::write64be(buf, std::bit_cast<uint64_t>(val));
  os << buf;
}

static void WriteString(wpi::raw_ostream& os, std::string_view str) {
  wpi::WriteUleb128(os, str.size());
  os << str;
}

static void WriteRaw(wpi::raw_ostream& os, std::span<const uint8_t> str) {
  wpi::WriteUleb128(os, str.size());
  os << str;
}

static bool WriteType(wpi::raw_ostream& os, NT_Type type) {
  char ch;
  // Convert from enum to actual byte value.
  switch (type) {
    case NT_BOOLEAN:
      ch = Message3::kBoolean;
      break;
    case NT_INTEGER:
    case NT_FLOAT:
    case NT_DOUBLE:
      ch = Message3::kDouble;
      break;
    case NT_STRING:
      ch = Message3::kString;
      break;
    case NT_RAW:
      ch = Message3::kRaw;
      break;
    case NT_BOOLEAN_ARRAY:
      ch = Message3::kBooleanArray;
      break;
    case NT_INTEGER_ARRAY:
    case NT_FLOAT_ARRAY:
    case NT_DOUBLE_ARRAY:
      ch = Message3::kDoubleArray;
      break;
    case NT_STRING_ARRAY:
      ch = Message3::kStringArray;
      break;
    case NT_RPC:
      ch = Message3::kRpcDef;
      break;
    default:
      return false;
  }
  os << ch;
  return true;
}

static bool WriteValue(wpi::raw_ostream& os, const Value& value) {
  switch (value.type()) {
    case NT_BOOLEAN:
      Write8(os, value.GetBoolean() ? 1 : 0);
      break;
    case NT_INTEGER:
      WriteDouble(os, value.GetInteger());
      break;
    case NT_FLOAT:
      WriteDouble(os, value.GetFloat());
      break;
    case NT_DOUBLE:
      WriteDouble(os, value.GetDouble());
      break;
    case NT_STRING:
      WriteString(os, value.GetString());
      break;
    case NT_RAW:
      WriteRaw(os, value.GetRaw());
      break;
    case NT_RPC:
      WriteRaw(os, value.GetRaw());
      break;
    case NT_BOOLEAN_ARRAY: {
      auto v = value.GetBooleanArray();
      size_t size = v.size();
      if (size > 0xff) {
        size = 0xff;  // size is only 1 byte, truncate
      }
      Write8(os, size);

      for (size_t i = 0; i < size; ++i) {
        Write8(os, v[i] ? 1 : 0);
      }
      break;
    }
    case NT_INTEGER_ARRAY: {
      auto v = value.GetIntegerArray();
      size_t size = v.size();
      if (size > 0xff) {
        size = 0xff;  // size is only 1 byte, truncate
      }
      Write8(os, size);

      for (size_t i = 0; i < size; ++i) {
        WriteDouble(os, v[i]);
      }
      break;
    }
    case NT_FLOAT_ARRAY: {
      auto v = value.GetFloatArray();
      size_t size = v.size();
      if (size > 0xff) {
        size = 0xff;  // size is only 1 byte, truncate
      }
      Write8(os, size);

      for (size_t i = 0; i < size; ++i) {
        WriteDouble(os, v[i]);
      }
      break;
    }
    case NT_DOUBLE_ARRAY: {
      auto v = value.GetDoubleArray();
      size_t size = v.size();
      if (size > 0xff) {
        size = 0xff;  // size is only 1 byte, truncate
      }
      Write8(os, size);

      for (size_t i = 0; i < size; ++i) {
        WriteDouble(os, v[i]);
      }
      break;
    }
    case NT_STRING_ARRAY: {
      auto v = value.GetStringArray();
      size_t size = v.size();
      if (size > 0xff) {
        size = 0xff;  // size is only 1 byte, truncate
      }
      Write8(os, size);

      for (size_t i = 0; i < size; ++i) {
        WriteString(os, v[i]);
      }
      break;
    }
    default:
      return false;
  }
  return true;
}

void nt::net3::WireEncodeKeepAlive(wpi::raw_ostream& os) {
  Write8(os, Message3::kKeepAlive);
}

void nt::net3::WireEncodeServerHelloDone(wpi::raw_ostream& os) {
  Write8(os, Message3::kServerHelloDone);
}

void nt::net3::WireEncodeClientHelloDone(wpi::raw_ostream& os) {
  Write8(os, Message3::kClientHelloDone);
}

void nt::net3::WireEncodeClearEntries(wpi::raw_ostream& os) {
  Write8(os, Message3::kClearEntries);
  Write32(os, Message3::kClearAllMagic);
}

void nt::net3::WireEncodeProtoUnsup(wpi::raw_ostream& os,
                                    unsigned int proto_rev) {
  Write8(os, Message3::kProtoUnsup);
  Write16(os, proto_rev);
}

void nt::net3::WireEncodeClientHello(wpi::raw_ostream& os,
                                     std::string_view self_id,
                                     unsigned int proto_rev) {
  Write8(os, Message3::kClientHello);
  Write16(os, proto_rev);
  WriteString(os, self_id);
}

void nt::net3::WireEncodeServerHello(wpi::raw_ostream& os, unsigned int flags,
                                     std::string_view self_id) {
  Write8(os, Message3::kServerHello);
  Write8(os, flags);
  WriteString(os, self_id);
}

bool nt::net3::WireEncodeEntryAssign(wpi::raw_ostream& os,
                                     std::string_view name, unsigned int id,
                                     unsigned int seq_num, const Value& value,
                                     unsigned int flags) {
  Write8(os, Message3::kEntryAssign);
  WriteString(os, name);
  WriteType(os, value.type());
  Write16(os, id);
  Write16(os, seq_num);
  Write8(os, flags);
  return WriteValue(os, value);
}

bool nt::net3::WireEncodeEntryUpdate(wpi::raw_ostream& os, unsigned int id,
                                     unsigned int seq_num, const Value& value) {
  Write8(os, Message3::kEntryUpdate);
  Write16(os, id);
  Write16(os, seq_num);
  WriteType(os, value.type());
  return WriteValue(os, value);
}

void nt::net3::WireEncodeFlagsUpdate(wpi::raw_ostream& os, unsigned int id,
                                     unsigned int flags) {
  Write8(os, Message3::kFlagsUpdate);
  Write16(os, id);
  Write8(os, flags);
}

void nt::net3::WireEncodeEntryDelete(wpi::raw_ostream& os, unsigned int id) {
  Write8(os, Message3::kEntryDelete);
  Write16(os, id);
}

void nt::net3::WireEncodeExecuteRpc(wpi::raw_ostream& os, unsigned int id,
                                    unsigned int uid,
                                    std::span<const uint8_t> params) {
  Write8(os, Message3::kExecuteRpc);
  Write16(os, id);
  Write16(os, uid);
  WriteRaw(os, params);
}

void nt::net3::WireEncodeRpcResponse(wpi::raw_ostream& os, unsigned int id,
                                     unsigned int uid,
                                     std::span<const uint8_t> result) {
  Write8(os, Message3::kRpcResponse);
  Write16(os, id);
  Write16(os, uid);
  WriteRaw(os, result);
}

bool nt::net3::WireEncode(wpi::raw_ostream& os, const Message3& msg) {
  switch (msg.type()) {
    case Message3::kKeepAlive:
      WireEncodeKeepAlive(os);
      break;
    case Message3::kServerHelloDone:
      WireEncodeServerHelloDone(os);
      break;
    case Message3::kClientHelloDone:
      WireEncodeClientHelloDone(os);
      break;
    case Message3::kClientHello:
      WireEncodeClientHello(os, msg.str(), msg.id());
      break;
    case Message3::kProtoUnsup:
      WireEncodeProtoUnsup(os, msg.id());
      break;
    case Message3::kServerHello:
      WireEncodeServerHello(os, msg.flags(), msg.str());
      break;
    case Message3::kEntryAssign:
      return WireEncodeEntryAssign(os, msg.str(), msg.id(), msg.seq_num_uid(),
                                   msg.value(), msg.flags());
    case Message3::kEntryUpdate:
      return WireEncodeEntryUpdate(os, msg.id(), msg.seq_num_uid(),
                                   msg.value());
    case Message3::kFlagsUpdate:
      WireEncodeFlagsUpdate(os, msg.id(), msg.flags());
      break;
    case Message3::kEntryDelete:
      WireEncodeEntryDelete(os, msg.id());
      break;
    case Message3::kClearEntries:
      WireEncodeClearEntries(os);
      break;
    case Message3::kExecuteRpc:
      WireEncodeExecuteRpc(os, msg.id(), msg.seq_num_uid(), msg.bytes());
      break;
    case Message3::kRpcResponse:
      WireEncodeRpcResponse(os, msg.id(), msg.seq_num_uid(), msg.bytes());
      break;
    case Message3::kUnknown:
      return true;  // ignore
    default:
      return false;
  }
  return true;
}
