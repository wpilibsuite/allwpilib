/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "nt_wireencoder.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "nt_internal.h"
#include "nt_leb128.h"

using namespace ntimpl;

WireEncoder::WireEncoder(unsigned int proto_rev) {
  m_start = m_cur = (char*)std::malloc(1024);
  m_end = m_start + 1024;
  m_proto_rev = proto_rev;
  m_error = nullptr;
}

WireEncoder::~WireEncoder() { std::free(m_start); }

void WireEncoder::WriteDouble(double val) {
  Reserve(8);
  std::uint64_t v = *reinterpret_cast<uint64_t*>(&val);
  *m_cur++ = (char)((v >> 56) & 0xff);
  *m_cur++ = (char)((v >> 48) & 0xff);
  *m_cur++ = (char)((v >> 40) & 0xff);
  *m_cur++ = (char)((v >> 32) & 0xff);
  *m_cur++ = (char)((v >> 24) & 0xff);
  *m_cur++ = (char)((v >> 16) & 0xff);
  *m_cur++ = (char)((v >> 8) & 0xff);
  *m_cur++ = (char)(v & 0xff);
}

void WireEncoder::ReserveSlow(std::size_t len) {
  assert(m_end > m_cur);
  if (static_cast<std::size_t>(m_end - m_cur) >= len) return;
  std::size_t pos = m_cur - m_start;
  std::size_t newlen = (m_end - m_start) * 2;
  while (newlen < (pos + len)) newlen *= 2;
  m_start = (char*)std::realloc(m_start, newlen);
  m_cur = m_start + pos;
  m_end = m_start + newlen;
}

void WireEncoder::WriteULEB128(unsigned long val) {
  Reserve(size_uleb128(val));
  m_cur += write_uleb128(m_cur, val);
}

void WireEncoder::WriteType(NT_Type type) {
  Reserve(1);
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

std::size_t WireEncoder::GetValueSize(const NT_Value& value) {
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
    case NT_BOOLEAN_ARRAY:
      return 1 + value.data.arr_boolean.size;
    case NT_DOUBLE_ARRAY:
      return 1 + value.data.arr_double.size * 8;
    case NT_STRING_ARRAY: {
      size_t len = 1;
      for (size_t i = 0; i < value.data.arr_string.size; ++i)
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
      if (size > 0xff) size = 0xff;
      Reserve(1 + size);
      Write8(size);

      for (std::size_t i = 0; i < size; ++i)
        Write8(value.data.arr_boolean.arr[i] ? 1 : 0);
      break;
    }
    case NT_DOUBLE_ARRAY: {
      std::size_t size = value.data.arr_double.size;
      if (size > 0xff) size = 0xff;
      Reserve(1 + size * 8);
      Write8(size);

      for (std::size_t i = 0; i < size; ++i)
        WriteDouble(value.data.arr_double.arr[i] ? 1 : 0);
      break;
    }
    case NT_STRING_ARRAY: {
      std::size_t size = value.data.arr_string.size;
      if (size > 0xff) size = 0xff;
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

std::size_t WireEncoder::GetStringSize(const NT_String& str) {
  if (m_proto_rev < 0x0300u) return 2 + str.len;
  return size_uleb128(str.len) + str.len;
}

void WireEncoder::WriteString(const NT_String& str) {
  // length
  std::size_t len = str.len;
  if (m_proto_rev < 0x0300u) {
    // Limited to 64K length; truncate if necessary
    if (len > 0xffff) len = 0xffff;
    Write16(len);
  } else
    WriteULEB128(len);

  // contents
  Reserve(len);
  std::memcpy(m_cur, str.str, len);
  m_cur += len;
}
