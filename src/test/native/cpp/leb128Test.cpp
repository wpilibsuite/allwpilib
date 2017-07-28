/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

//===- llvm/unittest/Support/LEB128Test.cpp - LEB128 function tests -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "support/leb128.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <string>

#include "llvm/SmallString.h"
#include "llvm/StringRef.h"

#include "support/raw_istream.h"

namespace wpi {

TEST(LEB128Test, WriteUleb128) {
#define EXPECT_ULEB128_EQ(EXPECTED, VALUE, PAD) \
  do { \
    llvm::StringRef expected(EXPECTED, sizeof(EXPECTED)-1); \
    llvm::SmallString<32> buf; \
    std::size_t size = WriteUleb128(buf, VALUE); \
    EXPECT_EQ(size, buf.size()); \
    EXPECT_EQ(expected, buf.str()); \
  } while (0)

  // Write ULEB128
  EXPECT_ULEB128_EQ("\x00", 0, 0);
  EXPECT_ULEB128_EQ("\x01", 1, 0);
  EXPECT_ULEB128_EQ("\x3f", 63, 0);
  EXPECT_ULEB128_EQ("\x40", 64, 0);
  EXPECT_ULEB128_EQ("\x7f", 0x7f, 0);
  EXPECT_ULEB128_EQ("\x80\x01", 0x80, 0);
  EXPECT_ULEB128_EQ("\x81\x01", 0x81, 0);
  EXPECT_ULEB128_EQ("\x90\x01", 0x90, 0);
  EXPECT_ULEB128_EQ("\xff\x01", 0xff, 0);
  EXPECT_ULEB128_EQ("\x80\x02", 0x100, 0);
  EXPECT_ULEB128_EQ("\x81\x02", 0x101, 0);

#undef EXPECT_ULEB128_EQ
}

TEST(LEB128Test, ReadUleb128) {
#define EXPECT_READ_ULEB128_EQ(EXPECTED, VALUE) \
  do { \
    unsigned long val = 0; \
    std::size_t size = ReadUleb128(VALUE, &val); \
    EXPECT_EQ(sizeof(VALUE) - 1, size); \
    EXPECT_EQ(EXPECTED, val); \
  } while (0)

  // Read ULEB128
  EXPECT_READ_ULEB128_EQ(0u, "\x00");
  EXPECT_READ_ULEB128_EQ(1u, "\x01");
  EXPECT_READ_ULEB128_EQ(63u, "\x3f");
  EXPECT_READ_ULEB128_EQ(64u, "\x40");
  EXPECT_READ_ULEB128_EQ(0x7fu, "\x7f");
  EXPECT_READ_ULEB128_EQ(0x80u, "\x80\x01");
  EXPECT_READ_ULEB128_EQ(0x81u, "\x81\x01");
  EXPECT_READ_ULEB128_EQ(0x90u, "\x90\x01");
  EXPECT_READ_ULEB128_EQ(0xffu, "\xff\x01");
  EXPECT_READ_ULEB128_EQ(0x100u, "\x80\x02");
  EXPECT_READ_ULEB128_EQ(0x101u, "\x81\x02");
  EXPECT_READ_ULEB128_EQ(8320u, "\x80\xc1\x80\x80\x10");

#undef EXPECT_READ_ULEB128_EQ
}

TEST(LEB128Test, SizeUleb128) {
  // Testing Plan:
  // (1) 128 ^ n ............ need (n+1) bytes
  // (2) 128 ^ n * 64 ....... need (n+1) bytes
  // (3) 128 ^ (n+1) - 1 .... need (n+1) bytes

  EXPECT_EQ(1u, SizeUleb128(0)); // special case

  EXPECT_EQ(1u, SizeUleb128(0x1UL));
  EXPECT_EQ(1u, SizeUleb128(0x40UL));
  EXPECT_EQ(1u, SizeUleb128(0x7fUL));

  EXPECT_EQ(2u, SizeUleb128(0x80UL));
  EXPECT_EQ(2u, SizeUleb128(0x2000UL));
  EXPECT_EQ(2u, SizeUleb128(0x3fffUL));

  EXPECT_EQ(3u, SizeUleb128(0x4000UL));
  EXPECT_EQ(3u, SizeUleb128(0x100000UL));
  EXPECT_EQ(3u, SizeUleb128(0x1fffffUL));

  EXPECT_EQ(4u, SizeUleb128(0x200000UL));
  EXPECT_EQ(4u, SizeUleb128(0x8000000UL));
  EXPECT_EQ(4u, SizeUleb128(0xfffffffUL));

  EXPECT_EQ(5u, SizeUleb128(0x10000000UL));
  EXPECT_EQ(5u, SizeUleb128(0x40000000UL));
  EXPECT_EQ(5u, SizeUleb128(0x7fffffffUL));

  EXPECT_EQ(5u, SizeUleb128(UINT32_MAX));
}

}  // namespace wpi
