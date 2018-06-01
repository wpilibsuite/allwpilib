/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_LEB128_H_
#define WPIUTIL_WPI_LEB128_H_

#include <cstddef>

#include "wpi/SmallVector.h"

namespace wpi {

class raw_istream;

/**
 * Get size of unsigned LEB128 data
 * @val: value
 *
 * Determine the number of bytes required to encode an unsigned LEB128 datum.
 * The algorithm is taken from Appendix C of the DWARF 3 spec. For information
 * on the encodings refer to section "7.6 - Variable Length Data". Return
 * the number of bytes required.
 */
uint64_t SizeUleb128(uint64_t val);

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
uint64_t WriteUleb128(SmallVectorImpl<char>& dest, uint64_t val);

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
uint64_t ReadUleb128(const char* addr, uint64_t* ret);

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
bool ReadUleb128(raw_istream& is, uint64_t* ret);

}  // namespace wpi

#endif  // WPIUTIL_WPI_LEB128_H_
