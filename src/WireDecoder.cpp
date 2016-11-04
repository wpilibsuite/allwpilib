/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WireDecoder.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "llvm/MathExtras.h"
#include "support/leb128.h"

using namespace nt;

static double ReadDouble(const char*& buf) {
  // Fast but non-portable!
  std::uint64_t val = (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  val <<= 8;
  val |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
  ++buf;
  return llvm::BitsToDouble(val);
}

WireDecoder::WireDecoder(wpi::raw_istream& is, unsigned int proto_rev)
    : m_is(is) {
  // Start with a 1K temporary buffer.  Use malloc instead of new so we can
  // realloc.
  m_allocated = 1024;
  m_buf = static_cast<char*>(std::malloc(m_allocated));
  m_proto_rev = proto_rev;
  m_error = nullptr;
}

WireDecoder::~WireDecoder() { std::free(m_buf); }

bool WireDecoder::ReadDouble(double* val) {
  const char* buf;
  if (!Read(&buf, 8)) return false;
  *val = ::ReadDouble(buf);
  return true;
}

void WireDecoder::Realloc(std::size_t len) {
  // Double current buffer size until we have enough space.
  if (m_allocated >= len) return;
  std::size_t newlen = m_allocated * 2;
  while (newlen < len) newlen *= 2;
  m_buf = static_cast<char*>(std::realloc(m_buf, newlen));
  m_allocated = newlen;
}

bool WireDecoder::ReadType(NT_Type* type) {
  unsigned int itype;
  if (!Read8(&itype)) return false;
  // Convert from byte value to enum
  switch (itype) {
    case 0x00:
      *type = NT_BOOLEAN;
      break;
    case 0x01:
      *type = NT_DOUBLE;
      break;
    case 0x02:
      *type = NT_STRING;
      break;
    case 0x03:
      *type = NT_RAW;
      break;
    case 0x10:
      *type = NT_BOOLEAN_ARRAY;
      break;
    case 0x11:
      *type = NT_DOUBLE_ARRAY;
      break;
    case 0x12:
      *type = NT_STRING_ARRAY;
      break;
    case 0x20:
      *type = NT_RPC;
      break;
    default:
      *type = NT_UNASSIGNED;
      m_error = "unrecognized value type";
      return false;
  }
  return true;
}

std::shared_ptr<Value> WireDecoder::ReadValue(NT_Type type) {
  switch (type) {
    case NT_BOOLEAN: {
      unsigned int v;
      if (!Read8(&v)) return nullptr;
      return Value::MakeBoolean(v != 0);
    }
    case NT_DOUBLE: {
      double v;
      if (!ReadDouble(&v)) return nullptr;
      return Value::MakeDouble(v);
    }
    case NT_STRING: {
      std::string v;
      if (!ReadString(&v)) return nullptr;
      return Value::MakeString(std::move(v));
    }
    case NT_RAW: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received raw value in protocol < 3.0";
        return nullptr;
      }
      std::string v;
      if (!ReadString(&v)) return nullptr;
      return Value::MakeRaw(std::move(v));
    }
    case NT_RPC: {
      if (m_proto_rev < 0x0300u) {
        m_error = "received RPC value in protocol < 3.0";
        return nullptr;
      }
      std::string v;
      if (!ReadString(&v)) return nullptr;
      return Value::MakeRpc(std::move(v));
    }
    case NT_BOOLEAN_ARRAY: {
      // size
      unsigned int size;
      if (!Read8(&size)) return nullptr;

      // array values
      const char* buf;
      if (!Read(&buf, size)) return nullptr;
      std::vector<int> v(size);
      for (unsigned int i = 0; i < size; ++i) v[i] = buf[i] ? 1 : 0;
      return Value::MakeBooleanArray(std::move(v));
    }
    case NT_DOUBLE_ARRAY: {
      // size
      unsigned int size;
      if (!Read8(&size)) return nullptr;

      // array values
      const char* buf;
      if (!Read(&buf, size * 8)) return nullptr;
      std::vector<double> v(size);
      for (unsigned int i = 0; i < size; ++i) v[i] = ::ReadDouble(buf);
      return Value::MakeDoubleArray(std::move(v));
    }
    case NT_STRING_ARRAY: {
      // size
      unsigned int size;
      if (!Read8(&size)) return nullptr;

      // array values
      std::vector<std::string> v(size);
      for (unsigned int i = 0; i < size; ++i) {
        if (!ReadString(&v[i])) return nullptr;
      }
      return Value::MakeStringArray(std::move(v));
    }
    default:
      m_error = "invalid type when trying to read value";
      return nullptr;
  }
}

bool WireDecoder::ReadString(std::string* str) {
  size_t len;
  if (m_proto_rev < 0x0300u) {
    unsigned int v;
    if (!Read16(&v)) return false;
    len = v;
  } else {
    unsigned long v;
    if (!ReadUleb128(&v)) return false;
    len = v;
  }
  const char* buf;
  if (!Read(&buf, len)) return false;
  *str = llvm::StringRef(buf, len);
  return true;
}
