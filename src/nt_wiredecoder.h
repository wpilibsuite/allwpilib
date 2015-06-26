/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_WIREDECODER_H_
#define NT_WIREDECODER_H_

#include <cstddef>

#include "ntcore.h"
#include "nt_leb128.h"
#include "nt_raw_istream.h"

namespace ntimpl {

class WireDecoder {
 public:
  explicit WireDecoder(raw_istream& is, unsigned int proto_rev);
  ~WireDecoder();

  void SetProtocolRev(unsigned int proto_rev) { m_proto_rev = proto_rev; }

  void Reset() { m_error = nullptr; }

  const char* GetError() const { return m_error; }

  bool Read(char** buf, std::size_t len) {
    if (len > m_allocated) Realloc(len);
    *buf = m_buf;
    return m_is.read(m_buf, len);
  }

  bool Read8(unsigned int* val) {
    char* buf;
    if (!Read(&buf, 1)) return false;
    *val = (*((unsigned char*)buf)) & 0xff;
    return true;
  }

  bool Read16(unsigned int* val) {
    char* buf;
    if (!Read(&buf, 2)) return false;
    unsigned int v = (*((unsigned char*)buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*((unsigned char*)buf)) & 0xff;
    *val = v;
    return true;
  }

  bool Read32(unsigned long* val) {
    char* buf;
    if (!Read(&buf, 4)) return false;
    unsigned int v = (*((unsigned char*)buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*((unsigned char*)buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*((unsigned char*)buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*((unsigned char*)buf)) & 0xff;
    *val = v;
    return true;
  }

  bool ReadDouble(double* val);

  bool ReadULEB128(unsigned long* val) { return read_uleb128(m_is, val); }

  bool ReadType(NT_Type* type);
  bool ReadValue(NT_Type type, NT_Value* value);
  bool ReadString(NT_String* str);

  WireDecoder(const WireDecoder&) = delete;
  WireDecoder& operator=(const WireDecoder&) = delete;

 protected:
  unsigned int m_proto_rev;
  const char* m_error;

 private:
  void Realloc(std::size_t len);

  raw_istream& m_is;

  char* m_buf;
  std::size_t m_allocated;
};

}  // namespace ntimpl

#endif  // NT_WIREDECODER_H_
