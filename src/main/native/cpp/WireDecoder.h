/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_WIREDECODER_H_
#define NT_WIREDECODER_H_

#include <cstddef>

#include "nt_Value.h"
#include "support/leb128.h"
#include "support/raw_istream.h"
//#include "Log.h"

namespace nt {

/* Decodes network data into native representation.
 * This class is designed to read from a raw_istream, which provides a blocking
 * read interface.  There are no provisions in this class for resuming a read
 * that was interrupted partway.  Read functions return false if
 * raw_istream.read() returned false (indicating the end of the input data
 * stream).
 */
class WireDecoder {
 public:
  explicit WireDecoder(wpi::raw_istream& is, unsigned int proto_rev);
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

  void set_error(const char* error) { m_error = error; }

  /* Reads the specified number of bytes.
   * @param buf pointer to read data (output parameter)
   * @param len number of bytes to read
   * Caution: the buffer is only temporarily valid.
   */
  bool Read(const char** buf, std::size_t len) {
    if (len > m_allocated) Realloc(len);
    *buf = m_buf;
    m_is.read(m_buf, len);
#if 0
    nt::Logger& logger = nt::Logger::GetInstance();
    if (logger.min_level() <= NT_LOG_DEBUG4 && logger.HasLogger()) {
      std::ostringstream oss;
      oss << "read " << len << " bytes:" << std::hex;
      if (!rv)
        oss << "error";
      else {
        for (std::size_t i=0; i < len; ++i)
          oss << ' ' << (unsigned int)((*buf)[i]);
      }
      logger.Log(NT_LOG_DEBUG4, __FILE__, __LINE__, oss.str().c_str());
    }
#endif
    return !m_is.has_error();
  }

  /* Reads a single byte. */
  bool Read8(unsigned int* val) {
    const char* buf;
    if (!Read(&buf, 1)) return false;
    *val = (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    return true;
  }

  /* Reads a 16-bit word. */
  bool Read16(unsigned int* val) {
    const char* buf;
    if (!Read(&buf, 2)) return false;
    unsigned int v = (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    *val = v;
    return true;
  }

  /* Reads a 32-bit word. */
  bool Read32(unsigned long* val) {
    const char* buf;
    if (!Read(&buf, 4)) return false;
    unsigned int v = (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    ++buf;
    v <<= 8;
    v |= (*reinterpret_cast<const unsigned char*>(buf)) & 0xff;
    *val = v;
    return true;
  }

  /* Reads a double. */
  bool ReadDouble(double* val);

  /* Reads an ULEB128-encoded unsigned integer. */
  bool ReadUleb128(unsigned long* val) { return wpi::ReadUleb128(m_is, val); }

  bool ReadType(NT_Type* type);
  bool ReadString(std::string* str);
  std::shared_ptr<Value> ReadValue(NT_Type type);

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
  wpi::raw_istream& m_is;

  /* temporary buffer */
  char* m_buf;

  /* allocated size of temporary buffer */
  std::size_t m_allocated;
};

}  // namespace nt

#endif  // NT_WIREDECODER_H_
