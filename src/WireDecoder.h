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
#include "leb128.h"
#include "raw_istream.h"
#include "Value.h"

namespace ntimpl {

/* Decodes network data into native representation.
 * This class is designed to read from a raw_istream, which provides a blocking
 * read interface.  There are no provisions in this class for resuming a read
 * that was interrupted partway.  Read functions return false if
 * raw_istream.read() returned false (indicating the end of the input data
 * stream).
 */
class WireDecoder {
 public:
  explicit WireDecoder(raw_istream& is, unsigned int proto_rev);
  ~WireDecoder();

  void set_proto_rev(unsigned int proto_rev) { m_proto_rev = proto_rev; }

  /* Get the active protocol revision. */
  unsigned int proto_rev() const { return m_proto_rev; }

  /* Clears error indicator. */
  void Reset() { m_error = nullptr; }

  /* Returns error indicator (a string describing the error).  Returns nullptr
   * if no error has occurred.
   */
  const char* error() const { return m_error; }

  /* Reads the specified number of bytes.
   * @param buf pointer to read data (output parameter)
   * @param len number of bytes to read
   */
  bool Read(char** buf, std::size_t len) {
    if (len > m_allocated) Realloc(len);
    *buf = m_buf;
    return m_is.read(m_buf, len);
  }

  /* Reads a single byte. */
  bool Read8(unsigned int* val) {
    char* buf;
    if (!Read(&buf, 1)) return false;
    *val = (*((unsigned char*)buf)) & 0xff;
    return true;
  }

  /* Reads a 16-bit word. */
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

  /* Reads a 32-bit word. */
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

  /* Reads a double. */
  bool ReadDouble(double* val);

  /* Reads an ULEB128-encoded unsigned integer. */
  bool ReadUleb128(unsigned long* val) {
    return ntimpl::ReadUleb128(m_is, val);
  }

  bool ReadType(NT_Type* type);
  bool ReadValue(NT_Type type, NT_Value* value);
  bool ReadString(NT_String* str);

  bool ReadValue(NT_Type type, Value* value) {
    NT_DisposeValue(value);
    return ReadValue(type, static_cast<NT_Value*>(value));
  }
  bool ReadString(StringValue* str) {
    NT_DisposeString(str);
    return ReadString(static_cast<NT_String*>(str));
  }

  WireDecoder(const WireDecoder&) = delete;
  WireDecoder& operator=(const WireDecoder&) = delete;

 protected:
  /* The protocol revision.  E.g. 0x0200 for version 2.0. */
  unsigned int m_proto_rev;

  /* Error indicator. */
  const char* m_error;

 private:
  /* Reallocate temporary buffer to specified length. */
  void Realloc(std::size_t len);

  /* input stream */
  raw_istream& m_is;

  /* temporary buffer */
  char* m_buf;

  /* allocated size of temporary buffer */
  std::size_t m_allocated;
};

}  // namespace ntimpl

#endif  // NT_WIREDECODER_H_
