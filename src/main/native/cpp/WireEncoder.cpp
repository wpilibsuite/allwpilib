/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WireEncoder.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "llvm/MathExtras.h"
#include "support/leb128.h"

using namespace nt;

WireEncoder::WireEncoder(unsigned int proto_rev) {
  m_proto_rev = proto_rev;
  m_error = nullptr;
}

void WireEncoder::WriteDouble(double val) {
  // The highest performance way to do this, albeit non-portable.
  std::uint64_t v = llvm::DoubleToBits(val);
  m_data.append({(char)((v >> 56) & 0xff), (char)((v >> 48) & 0xff),
                 (char)((v >> 40) & 0xff), (char)((v >> 32) & 0xff),
                 (char)((v >> 24) & 0xff), (char)((v >> 16) & 0xff),
                 (char)((v >> 8) & 0xff), (char)(v & 0xff)});
}

void WireEncoder::WriteUleb128(unsigned long val) {
  wpi::WriteUleb128(m_data, val);
}

void WireEncoder::WriteType(NT_Type type) {
  char ch;
  // Convert from enum to actual byte value.
  switch (type) {
    case NT_BOOLEAN:
      ch = 0x00;
      break;
    case NT_DOUBLE:
      ch = 0x01;
      break;
    case NT_STRING:
      ch = 0x02;
      break;
    case NT_RAW:
      if (m_proto_rev < 0x0300u) {
        m_error = "raw type not supported in protocol < 3.0";
        return;
      }
      ch = 0x03;
      break;
    case NT_BOOLEAN_ARRAY:
      ch = 0x10;
      break;
    case NT_DOUBLE_ARRAY:
      ch = 0x11;
      break;
    case NT_STRING_ARRAY:
      ch = 0x12;
      break;
    case NT_RPC:
      if (m_proto_rev < 0x0300u) {
        m_error = "RPC type not supported in protocol < 3.0";
        return;
      }
      ch = 0x20;
      break;
    default:
      m_error = "unrecognized type";
      return;
  }
  m_data.push_back(ch);
}

std::size_t WireEncoder::GetValueSize(const Value& value) const {
  switch (value.type()) {
    case NT_BOOLEAN:
      return 1;
    case NT_DOUBLE:
      return 8;
    case NT_STRING:
      return GetStringSize(value.GetString());
    case NT_RAW:
      if (m_proto_rev < 0x0300u) return 0;
      return GetStringSize(value.GetRaw());
    case NT_RPC:
      if (m_proto_rev < 0x0300u) return 0;
      return GetStringSize(value.GetRpc());
    case NT_BOOLEAN_ARRAY: {
      // 1-byte size, 1 byte per element
      std::size_t size = value.GetBooleanArray().size();
      if (size > 0xff) size = 0xff;  // size is only 1 byte, truncate
      return 1 + size;
    }
    case NT_DOUBLE_ARRAY: {
      // 1-byte size, 8 bytes per element
      std::size_t size = value.GetDoubleArray().size();
      if (size > 0xff) size = 0xff;  // size is only 1 byte, truncate
      return 1 + size * 8;
    }
    case NT_STRING_ARRAY: {
      auto v = value.GetStringArray();
      std::size_t size = v.size();
      if (size > 0xff) size = 0xff;  // size is only 1 byte, truncate
      std::size_t len = 1;           // 1-byte size
      for (std::size_t i = 0; i < size; ++i) len += GetStringSize(v[i]);
      return len;
    }
    default:
      return 0;
  }
}

void WireEncoder::WriteValue(const Value& value) {
  switch (value.type()) {
    case NT_BOOLEAN:
      Write8(value.GetBoolean() ? 1 : 0);
      break;
    case NT_DOUBLE:
      WriteDouble(value.GetDouble());
      break;
    case NT_STRING:
      WriteString(value.GetString());
      break;
    case NT_RAW:
      if (m_proto_rev < 0x0300u) {
        m_error = "raw values not supported in protocol < 3.0";
        return;
      }
      WriteString(value.GetRaw());
      break;
    case NT_RPC:
      if (m_proto_rev < 0x0300u) {
        m_error = "RPC values not supported in protocol < 3.0";
        return;
      }
      WriteString(value.GetRpc());
      break;
    case NT_BOOLEAN_ARRAY: {
      auto v = value.GetBooleanArray();
      std::size_t size = v.size();
      if (size > 0xff) size = 0xff;  // size is only 1 byte, truncate
      Write8(size);

      for (std::size_t i = 0; i < size; ++i) Write8(v[i] ? 1 : 0);
      break;
    }
    case NT_DOUBLE_ARRAY: {
      auto v = value.GetDoubleArray();
      std::size_t size = v.size();
      if (size > 0xff) size = 0xff;  // size is only 1 byte, truncate
      Write8(size);

      for (std::size_t i = 0; i < size; ++i) WriteDouble(v[i]);
      break;
    }
    case NT_STRING_ARRAY: {
      auto v = value.GetStringArray();
      std::size_t size = v.size();
      if (size > 0xff) size = 0xff;  // size is only 1 byte, truncate
      Write8(size);

      for (std::size_t i = 0; i < size; ++i) WriteString(v[i]);
      break;
    }
    default:
      m_error = "unrecognized type when writing value";
      return;
  }
}

std::size_t WireEncoder::GetStringSize(llvm::StringRef str) const {
  if (m_proto_rev < 0x0300u) {
    std::size_t len = str.size();
    if (len > 0xffff) len = 0xffff;  // Limited to 64K length; truncate
    return 2 + len;
  }
  return wpi::SizeUleb128(str.size()) + str.size();
}

void WireEncoder::WriteString(llvm::StringRef str) {
  // length
  std::size_t len = str.size();
  if (m_proto_rev < 0x0300u) {
    if (len > 0xffff) len = 0xffff;  // Limited to 64K length; truncate
    Write16(len);
  } else
    WriteUleb128(len);

  // contents
  m_data.append(str.data(), str.data() + len);
}
