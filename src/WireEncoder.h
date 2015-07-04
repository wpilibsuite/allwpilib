/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef NT_WIREENCODER_H_
#define NT_WIREENCODER_H_

#include <cassert>
#include <cstddef>

#include "ntcore.h"

namespace ntimpl {

/* Encodes native data for network transmission.
 * This class maintains an internal memory buffer for written data so that
 * it can be efficiently bursted to the network after a number of writes
 * have been performed.  For this reason, all operations are non-blocking.
 */
class WireEncoder {
 public:
  explicit WireEncoder(unsigned int proto_rev);
  ~WireEncoder();

  /* Change the protocol revision (mostly affects value encoding). */
  void set_proto_rev(unsigned int proto_rev) { m_proto_rev = proto_rev; }

  /* Get the active protocol revision. */
  unsigned int proto_rev() const { return m_proto_rev; }

  /* Clears buffer and error indicator. */
  void Reset() {
    m_cur = m_start;
    m_error = nullptr;
  }

  /* Returns error indicator (a string describing the error).  Returns nullptr
   * if no error has occurred.
   */
  const char* error() const { return m_error; }

  /* Returns pointer to start of memory buffer with written data. */
  const char* data() const { return m_start; }

  /* Returns number of bytes written to memory buffer. */
  std::size_t size() const { return m_cur - m_start; }

  /* Ensures the buffer has sufficient space to write len bytes.  Reallocates
   * the buffer if necessary.
   */
  void Reserve(std::size_t len) {
    // assert(m_end > m_cur);
    if (static_cast<size_t>(m_end - m_cur) < len)
      ReserveSlow(len);  // Need to reallocate memory
  }

  /* Writes a single byte. */
  void Write8(unsigned int val) {
    Reserve(1);
    *m_cur++ = (char)(val & 0xff);
  }

  /* Writes a 16-bit word. */
  void Write16(unsigned int val) {
    Reserve(2);
    *m_cur++ = (char)((val >> 8) & 0xff);
    *m_cur++ = (char)(val & 0xff);
  }

  /* Writes a 32-bit word. */
  void Write32(unsigned long val) {
    Reserve(4);
    *m_cur++ = (char)((val >> 24) & 0xff);
    *m_cur++ = (char)((val >> 16) & 0xff);
    *m_cur++ = (char)((val >> 8) & 0xff);
    *m_cur++ = (char)(val & 0xff);
  }

  /* Writes a double. */
  void WriteDouble(double val);

  /* Writes an ULEB128-encoded unsigned integer. */
  void WriteUleb128(unsigned long val);

  void WriteType(NT_Type type);
  void WriteValue(const NT_Value& value);
  void WriteString(const NT_String& str);

  /* Utility function to get the written size of a value (without actually
   * writing it).
   */
  std::size_t GetValueSize(const NT_Value& value) const;

  /* Utility function to get the written size of a string (without actually
   * writing it).
   */
  std::size_t GetStringSize(const NT_String& str) const;

  WireEncoder(const WireEncoder&) = delete;
  WireEncoder& operator=(const WireEncoder&) = delete;

 protected:
  /* The protocol revision.  E.g. 0x0200 for version 2.0. */
  unsigned int m_proto_rev;

  /* Error indicator. */
  const char* m_error;

 private:
  /* The slow path for Reserve() when we need to reallocate memory. */
  void ReserveSlow(std::size_t len);

  /* The start of the memory buffer. */
  char* m_start;

  /* Where to write the next byte. */
  char* m_cur;

  /* The end of the allocated buffer. */
  char* m_end;
};

}  // namespace ntimpl

#endif  // NT_WIREENCODER_H_
