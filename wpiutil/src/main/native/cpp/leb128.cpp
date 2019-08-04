/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/leb128.h"

#include "wpi/raw_istream.h"

namespace wpi {

uint64_t SizeUleb128(uint64_t val) {
  size_t count = 0;
  do {
    val >>= 7;
    ++count;
  } while (val != 0);
  return count;
}

uint64_t WriteUleb128(SmallVectorImpl<char>& dest, uint64_t val) {
  size_t count = 0;

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

uint64_t ReadUleb128(const char* addr, uint64_t* ret) {
  uint32_t result = 0;
  int shift = 0;
  size_t count = 0;

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

bool ReadUleb128(raw_istream& is, uint64_t* ret) {
  uint32_t result = 0;
  int shift = 0;

  while (1) {
    unsigned char byte;
    is.read(reinterpret_cast<char*>(&byte), 1);
    if (is.has_error()) return false;

    result |= (byte & 0x7f) << shift;
    shift += 7;

    if (!(byte & 0x80)) break;
  }

  *ret = result;

  return true;
}

}  // namespace wpi
