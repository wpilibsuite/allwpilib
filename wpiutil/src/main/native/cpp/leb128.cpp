// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/leb128.h"

#include "wpi/SpanExtras.h"
#include "wpi/raw_istream.h"
#include "wpi/raw_ostream.h"

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
    uint8_t byte = val & 0x7f;
    val >>= 7;

    if (val != 0) {
      byte |= 0x80;  // mark this byte to show that more bytes will follow
    }

    dest.push_back(byte);
    count++;
  } while (val != 0);

  return count;
}

void WriteUleb128(raw_ostream& os, uint64_t val) {
  do {
    uint8_t byte = val & 0x7f;
    val >>= 7;

    if (val != 0) {
      byte |= 0x80;  // mark this byte to show that more bytes will follow
    }

    os << byte;
  } while (val != 0);
}

uint64_t ReadUleb128(const char* addr, uint64_t* ret) {
  uint64_t result = 0;
  int shift = 0;
  size_t count = 0;

  while (1) {
    unsigned char byte = *reinterpret_cast<const unsigned char*>(addr);
    addr++;
    count++;

    result |= (byte & 0x7fULL) << shift;
    shift += 7;

    if (!(byte & 0x80)) {
      break;
    }
  }

  *ret = result;

  return count;
}

bool ReadUleb128(raw_istream& is, uint64_t* ret) {
  uint64_t result = 0;
  int shift = 0;

  while (1) {
    unsigned char byte;
    is.read(reinterpret_cast<char*>(&byte), 1);
    if (is.has_error()) {
      return false;
    }

    result |= (byte & 0x7fULL) << shift;
    shift += 7;

    if (!(byte & 0x80)) {
      break;
    }
  }

  *ret = result;

  return true;
}

std::optional<uint64_t> Uleb128Reader::ReadOne(std::span<const uint8_t>* in) {
  while (!in->empty()) {
    uint8_t byte = in->front();
    *in = wpi::drop_front(*in);

    m_result |= (byte & 0x7fULL) << m_shift;
    m_shift += 7;

    if (!(byte & 0x80)) {
      uint64_t result = m_result;
      m_result = 0;
      m_shift = 0;
      return result;
    }
  }
  return std::nullopt;
}

}  // namespace wpi
