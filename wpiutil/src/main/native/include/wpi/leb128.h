// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_LEB128_H_
#define WPIUTIL_WPI_LEB128_H_

#include <stdint.h>

#include <optional>
#include <span>

namespace wpi {

template <typename T>
class SmallVectorImpl;
class raw_istream;
class raw_ostream;

/**
 * Get size of unsigned LEB128 data.
 *
 * Determine the number of bytes required to encode an unsigned LEB128 datum.
 * The algorithm is taken from Appendix C of the DWARF 3 spec. For information
 * on the encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes required.
 *
 * @param val LEB128 data.
 */
uint64_t SizeUleb128(uint64_t val);

/**
 * Write unsigned LEB128 data.
 *
 * Encode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes written.
 *
 * @param dest The address where the ULEB128 data is to be stored.
 * @param val Value to be stored.
 */
uint64_t WriteUleb128(SmallVectorImpl<char>& dest, uint64_t val);

/**
 * Write unsigned LEB128 data.
 *
 * Encode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data".
 *
 * @param os Output stream.
 * @param val Value to be stored.
 */
void WriteUleb128(raw_ostream& os, uint64_t val);

/**
 * Read unsigned LEB128 data.
 *
 * Decode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes read.
 *
 * @param addr The address where the ULEB128 data is stored.
 * @param ret Address to store the result.
 */
uint64_t ReadUleb128(const char* addr, uint64_t* ret);

/**
 * Read unsigned LEB128 data from a stream.
 *
 * Decode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Return
 * false on stream error, true on success.
 *
 * @param is The input stream where the ULEB128 data is to be read from.
 * @param ret Address to store the result.
 */
bool ReadUleb128(raw_istream& is, uint64_t* ret);

/**
 * Unsigned LEB128 streaming reader.
 *
 * Decode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data".
 */
class Uleb128Reader {
 public:
  /**
   * Decode a single ULEB128 value.  Returns after a single ULEB128 value has
   * been read or insufficient input (call in a loop to get multiple values).
   * If a value is returned, internal state is reset so it's safe to immediately
   * call this function again to decode another value.
   *
   * @param in Input data; modified as data is consumed (any unconsumed data
   *           is left when function returns).
   * @return value (in std::optional)
   */
  std::optional<uint64_t> ReadOne(std::span<const uint8_t>* in);

 private:
  uint64_t m_result = 0;
  int m_shift = 0;
};

}  // namespace wpi

#endif  // WPIUTIL_WPI_LEB128_H_
