/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/leb128.h"

#include "support/raw_istream.h"

namespace wpi {

/**
 * Get size of unsigned LEB128 data
 * @val: value
 *
 * Determine the number of bytes required to encode an unsigned LEB128 datum.
 * The algorithm is taken from Appendix C of the DWARF 3 spec. For information
 * on the encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes required.
 */
std::size_t SizeUleb128(unsigned long val) {
  std::size_t count = 0;
  do {
    val >>= 7;
    ++count;
  } while (val != 0);
  return count;
}

/**
 * Write unsigned LEB128 data
 * @addr: the address where the ULEB128 data is to be stored
 * @val: value to be stored
 *
 * Encode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes written.
 */
std::size_t WriteUleb128(llvm::SmallVectorImpl<char>& dest, unsigned long val) {
  std::size_t count = 0;

  do {
    unsigned char byte = val & 0x7f;
    val >>= 7;

    if (val != 0)
      byte |= 0x80;  // mark this byte to show that more bytes will follow

    dest.push_back(byte);
    count++;
  } while (val != 0);

  return count;
}

/**
 * Read unsigned LEB128 data
 * @addr: the address where the ULEB128 data is stored
 * @ret: address to store the result
 *
 * Decode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes read.
 */
std::size_t ReadUleb128(const char* addr, unsigned long* ret) {
  unsigned long result = 0;
  int shift = 0;
  std::size_t count = 0;

  while (1) {
    unsigned char byte = *reinterpret_cast<const unsigned char*>(addr);
    addr++;
    count++;

    result |= (byte & 0x7f) << shift;
    shift += 7;

    if (!(byte & 0x80)) break;
  }

  *ret = result;

  return count;
}

/**
 * Read unsigned LEB128 data from a stream
 * @is: the input stream where the ULEB128 data is to be read from
 * @ret: address to store the result
 *
 * Decode an unsigned LEB128 encoded datum. The algorithm is taken
 * from Appendix C of the DWARF 3 spec. For information on the
 * encodings refer to section "7.6 - Variable Length Data". Return
 * false on stream error, true on success.
 */
bool ReadUleb128(raw_istream& is, unsigned long* ret) {
  unsigned long result = 0;
  int shift = 0;

  while (1) {
    unsigned char byte;
    is.read((char*)&byte, 1);
    if (is.has_error()) return false;

    result |= (byte & 0x7f) << shift;
    shift += 7;

    if (!(byte & 0x80)) break;
  }

  *ret = result;

  return true;
}

}  // namespace wpi
