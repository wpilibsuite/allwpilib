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

#include "leb128.h"

using namespace ntimpl;

WireEncoder::WireEncoder(unsigned int proto_rev) {
  // Start with a 1024-byte buffer.  Use malloc instead of new so we can
  // realloc.
  m_start = m_cur = static_cast<char*>(std::malloc(1024));
  m_end = m_start + 1024;
  m_proto_rev = proto_rev;
  m_error = nullptr;
}

WireEncoder::~WireEncoder() { std::free(m_start); }

void WireEncoder::WriteDouble(double val) {
  Reserve(8);
  // The highest performance way to do this, albeit non-portable.
  std::uint64_t v = reinterpret_cast<uint64_t&>(val);
  m_cur[7] = (char)(v & 0xff); v >>= 8;
  m_cur[6] = (char)(v & 0xff); v >>= 8;
  m_cur[5] = (char)(v & 0xff); v >>= 8;
  m_cur[4] = (char)(v & 0xff); v >>= 8;
  m_cur[3] = (char)(v & 0xff); v >>= 8;
  m_cur[2] = (char)(v & 0xff); v >>= 8;
  m_cur[1] = (char)(v & 0xff); v >>= 8;
  m_cur[0] = (char)(v & 0xff);
  m_cur += 8;
}

void WireEncoder::ReserveSlow(std::size_t len) {
  assert(m_end >= m_cur);
  // should never happen due to checks in Reserve(), but check anyway
  if (static_cast<std::size_t>(m_end - m_cur) >= len) return;

  // Double current buffer size until we have enough space.  Since we're
  // reserving space, it's likely more will be required soon in any case.
  std::size_t pos = m_cur - m_start;
  std::size_t newlen = (m_end - m_start) * 2;
  while (newlen < (pos + len)) newlen *= 2;
  m_start = static_cast<char*>(std::realloc(m_start, newlen));
  m_cur = m_start + pos;
  m_end = m_start + newlen;
}

void WireEncoder::WriteUleb128(unsigned long val) {
  Reserve(SizeUleb128(val));
  m_cur += ntimpl::WriteUleb128(m_cur, val);
}

void WireEncoder::WriteType(NT_Type type) {
  Reserve(1);
  // Convert from enum to actual byte value.
  switch (type) {
    case NT_BOOLEAN:
      *m_cur = 0x00;
      break;
    case NT_DOUBLE:
      *m_cur = 0x01;
      break;
    case NT_STRING:
      *m_cur = 0x02;
      break;
    case NT_RAW:
      if (m_proto_rev < 0x0300u) {
        m_error = "raw type not supported in protocol < 3.0";
        return;
      }
      *m_cur = 0x03;
      break;
    case NT_BOOLEAN_ARRAY:
      *m_cur = 0x10;
      break;
    case NT_DOUBLE_ARRAY:
      *m_cur = 0x11;
      break;
    case NT_STRING_ARRAY:
      *m_cur = 0x12;
      break;
    case NT_RPC:
      if (m_proto_rev < 0x0300u) {
        m_error = "RPC type not supported in protocol < 3.0";
        return;
      }
      *m_cur = 0x20;
      break;
    default:
      m_error = "unrecognized type";
      return;
  }
  ++m_cur;
}

std::size_t WireEncoder::GetValueSize(const NT_Value& value) const {
  switch (value.type) {
    case NT_BOOLEAN:
      return 1;
    case NT_DOUBLE:
      return 8;
    case NT_STRING:
      return GetStringSize(value.data.v_string);
    case NT_RAW:
    case NT_RPC:
      if (m_proto_rev < 0x0300u) return 0;
      return GetStringSize(value.data.v_raw);
    case NT_BOOLEAN_ARRAY: {
      // 1-byte size, 1 byte per element
      std::size_t size = value.data.arr_boolean.size;
      if (size > 0xff) size = 0xff; // size is only 1 byte, truncate
      return 1 + size;
    }
    case NT_DOUBLE_ARRAY: {
      // 1-byte size, 8 bytes per element
      std::size_t size = value.data.arr_double.size;
      if (size > 0xff) size = 0xff; // size is only 1 byte, truncate
      return 1 + size * 8;
    }
    case NT_STRING_ARRAY: {
      std::size_t size = value.data.arr_string.size;
      if (size > 0xff) size = 0xff; // size is only 1 byte, truncate
      std::size_t len = 1; // 1-byte size
      for (std::size_t i = 0; i < size; ++i)
        len += GetStringSize(value.data.arr_string.arr[i]);
      return len;
    }
    default:
      return 0;
  }
}

void WireEncoder::WriteValue(const NT_Value& value) {
  switch (value.type) {
    case NT_BOOLEAN:
      Write8(value.data.v_boolean ? 1 : 0);
      break;
    case NT_DOUBLE:
      WriteDouble(value.data.v_double);
      break;
    case NT_STRING:
      WriteString(value.data.v_string);
      break;
    case NT_RAW:
    case NT_RPC:
      if (m_proto_rev < 0x0300u) {
        m_error = "raw and rpc values not supported in protocol < 3.0";
        return;
      }
      WriteString(value.data.v_raw);
      break;
    case NT_BOOLEAN_ARRAY: {
      std::size_t size = value.data.arr_boolean.size;
      if (size > 0xff) size = 0xff; // size is only 1 byte, truncate
      Reserve(1 + size);
      Write8(size);

      for (std::size_t i = 0; i < size; ++i)
        Write8(value.data.arr_boolean.arr[i] ? 1 : 0);
      break;
    }
    case NT_DOUBLE_ARRAY: {
      std::size_t size = value.data.arr_double.size;
      if (size > 0xff) size = 0xff; // size is only 1 byte, truncate
      Reserve(1 + size * 8);
      Write8(size);

      for (std::size_t i = 0; i < size; ++i)
        WriteDouble(value.data.arr_double.arr[i]);
      break;
    }
    case NT_STRING_ARRAY: {
      std::size_t size = value.data.arr_string.size;
      if (size > 0xff) size = 0xff; // size is only 1 byte, truncate
      Write8(size);

      for (std::size_t i = 0; i < size; ++i)
        WriteString(value.data.arr_string.arr[i]);
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
    if (len > 0xffff) len = 0xffff; // Limited to 64K length; truncate
    return 2 + len;
  }
  return SizeUleb128(str.size()) + str.size();
}

void WireEncoder::WriteString(llvm::StringRef str) {
  // length
  std::size_t len = str.size();
  if (m_proto_rev < 0x0300u) {
    if (len > 0xffff) len = 0xffff; // Limited to 64K length; truncate
    Write16(len);
  } else
    WriteUleb128(len);

  // contents
  Reserve(len);
  std::memcpy(m_cur, str.data(), len);
  m_cur += len;
}
