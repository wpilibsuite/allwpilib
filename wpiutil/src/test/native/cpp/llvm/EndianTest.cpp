//===- unittests/Support/EndianTest.cpp - Endian.h tests ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/Endian.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <cstdlib>
#include <ctime>
using namespace wpi::util;
using namespace support;

#undef max

namespace {

TEST_CASE("Endian Read", "[wpiutil][llvm]") {
  // These are 5 bytes so we can be sure at least one of the reads is unaligned.
  unsigned char bigval[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  unsigned char littleval[] = {0x00, 0x04, 0x03, 0x02, 0x01};
  int32_t BigAsHost = 0x00010203;
  CHECK((BigAsHost) == ((endian::read<int32_t, unaligned>(bigval, std::endian::big))));
  int32_t LittleAsHost = 0x02030400;
  CHECK((LittleAsHost) == ((endian::read<int32_t, unaligned>(
                              littleval, std::endian::little))));

  CHECK(((endian::read<int32_t, unaligned>(bigval + 1, std::endian::big))) == ((endian::read<int32_t, unaligned>(littleval + 1, std::endian::little))));
}

TEST_CASE("Endian WriteNext", "[wpiutil][llvm]") {
  unsigned char bigval[] = {0x00, 0x00}, *p = bigval;
  endian::writeNext<int16_t, std::endian::big>(p, short(0xaabb));
  CHECK((bigval[0]) == (0xaa));
  CHECK((bigval[1]) == (0xbb));
  CHECK((p) == (bigval + 2));

  char littleval[8] = {}, *q = littleval;
  endian::writeNext<uint32_t, std::endian::little>(q, 0x44556677);
  CHECK((littleval[0]) == (0x77));
  CHECK((littleval[1]) == (0x66));
  CHECK((littleval[2]) == (0x55));
  CHECK((littleval[3]) == (0x44));
  CHECK((q) == (littleval + 4));

  endian::writeNext<uint32_t>(q, 0x11223344, std::endian::little);
  CHECK((littleval[4]) == (0x44));
  CHECK((littleval[5]) == (0x33));
  CHECK((littleval[6]) == (0x22));
  CHECK((littleval[7]) == (0x11));
  CHECK((q) == (littleval + 8));
}

TEST_CASE("Endian ReadBitAligned", "[wpiutil][llvm]") {
  // Simple test to make sure we properly pull out the 0x0 word.
  unsigned char littleval[] = {0x3f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff};
  unsigned char bigval[] = {0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xc0};
  CHECK(((endian::readAtBitAlignment<int, std::endian::little, unaligned>(
          &littleval[0], 6))) == (0x0));
  CHECK(((endian::readAtBitAlignment<int, std::endian::big, unaligned>(
                &bigval[0], 6))) == (0x0));
  // Test to make sure that signed right shift of 0xf0000000 is masked
  // properly.
  unsigned char littleval2[] = {0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00};
  unsigned char bigval2[] = {0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  CHECK(((endian::readAtBitAlignment<int, std::endian::little, unaligned>(
          &littleval2[0], 4))) == (0x0f000000));
  CHECK(((endian::readAtBitAlignment<int, std::endian::big, unaligned>(
                &bigval2[0], 4))) == (0x0f000000));
  // Test to make sure left shift of start bit doesn't overflow.
  CHECK(((endian::readAtBitAlignment<int, std::endian::little, unaligned>(
          &littleval2[0], 1))) == (0x78000000));
  CHECK(((endian::readAtBitAlignment<int, std::endian::big, unaligned>(
                &bigval2[0], 1))) == (0x78000000));
  // Test to make sure 64-bit int doesn't overflow.
  unsigned char littleval3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  unsigned char bigval3[] = {0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  CHECK(((endian::readAtBitAlignment<int64_t, std::endian::little, unaligned>(
          &littleval3[0], 4))) == (0x0f00000000000000));
  CHECK(((endian::readAtBitAlignment<int64_t, std::endian::big, unaligned>(
          &bigval3[0], 4))) == (0x0f00000000000000));
}

TEST_CASE("Endian WriteBitAligned", "[wpiutil][llvm]") {
  // This test ensures that signed right shift of 0xffffaa is masked
  // properly.
  unsigned char bigval[8] = {0x00};
  endian::writeAtBitAlignment<int32_t, std::endian::big, unaligned>(
      bigval, (int)0xffffaaaa, 4);
  CHECK((bigval[0]) == (0xff));
  CHECK((bigval[1]) == (0xfa));
  CHECK((bigval[2]) == (0xaa));
  CHECK((bigval[3]) == (0xa0));
  CHECK((bigval[4]) == (0x00));
  CHECK((bigval[5]) == (0x00));
  CHECK((bigval[6]) == (0x00));
  CHECK((bigval[7]) == (0x0f));

  unsigned char littleval[8] = {0x00};
  endian::writeAtBitAlignment<int32_t, std::endian::little, unaligned>(
      littleval, (int)0xffffaaaa, 4);
  CHECK((littleval[0]) == (0xa0));
  CHECK((littleval[1]) == (0xaa));
  CHECK((littleval[2]) == (0xfa));
  CHECK((littleval[3]) == (0xff));
  CHECK((littleval[4]) == (0x0f));
  CHECK((littleval[5]) == (0x00));
  CHECK((littleval[6]) == (0x00));
  CHECK((littleval[7]) == (0x00));

  // This test makes sure 1<<31 doesn't overflow.
  // Test to make sure left shift of start bit doesn't overflow.
  unsigned char bigval2[8] = {0x00};
  endian::writeAtBitAlignment<int32_t, std::endian::big, unaligned>(
      bigval2, (int)0xffffffff, 1);
  CHECK((bigval2[0]) == (0xff));
  CHECK((bigval2[1]) == (0xff));
  CHECK((bigval2[2]) == (0xff));
  CHECK((bigval2[3]) == (0xfe));
  CHECK((bigval2[4]) == (0x00));
  CHECK((bigval2[5]) == (0x00));
  CHECK((bigval2[6]) == (0x00));
  CHECK((bigval2[7]) == (0x01));

  unsigned char littleval2[8] = {0x00};
  endian::writeAtBitAlignment<int32_t, std::endian::little, unaligned>(
      littleval2, (int)0xffffffff, 1);
  CHECK((littleval2[0]) == (0xfe));
  CHECK((littleval2[1]) == (0xff));
  CHECK((littleval2[2]) == (0xff));
  CHECK((littleval2[3]) == (0xff));
  CHECK((littleval2[4]) == (0x01));
  CHECK((littleval2[5]) == (0x00));
  CHECK((littleval2[6]) == (0x00));
  CHECK((littleval2[7]) == (0x00));

  // Test to make sure 64-bit int doesn't overflow.
  unsigned char bigval64[16] = {0x00};
  endian::writeAtBitAlignment<int64_t, std::endian::big, unaligned>(
      bigval64, (int64_t)0xffffffffffffffff, 1);
  CHECK((bigval64[0]) == (0xff));
  CHECK((bigval64[1]) == (0xff));
  CHECK((bigval64[2]) == (0xff));
  CHECK((bigval64[3]) == (0xff));
  CHECK((bigval64[4]) == (0xff));
  CHECK((bigval64[5]) == (0xff));
  CHECK((bigval64[6]) == (0xff));
  CHECK((bigval64[7]) == (0xfe));
  CHECK((bigval64[8]) == (0x00));
  CHECK((bigval64[9]) == (0x00));
  CHECK((bigval64[10]) == (0x00));
  CHECK((bigval64[11]) == (0x00));
  CHECK((bigval64[12]) == (0x00));
  CHECK((bigval64[13]) == (0x00));
  CHECK((bigval64[14]) == (0x00));
  CHECK((bigval64[15]) == (0x01));

  unsigned char littleval64[16] = {0x00};
  endian::writeAtBitAlignment<int64_t, std::endian::little, unaligned>(
      littleval64, (int64_t)0xffffffffffffffff, 1);
  CHECK((littleval64[0]) == (0xfe));
  CHECK((littleval64[1]) == (0xff));
  CHECK((littleval64[2]) == (0xff));
  CHECK((littleval64[3]) == (0xff));
  CHECK((littleval64[4]) == (0xff));
  CHECK((littleval64[5]) == (0xff));
  CHECK((littleval64[6]) == (0xff));
  CHECK((littleval64[7]) == (0xff));
  CHECK((littleval64[8]) == (0x01));
  CHECK((littleval64[9]) == (0x00));
  CHECK((littleval64[10]) == (0x00));
  CHECK((littleval64[11]) == (0x00));
  CHECK((littleval64[12]) == (0x00));
  CHECK((littleval64[13]) == (0x00));
  CHECK((littleval64[14]) == (0x00));
  CHECK((littleval64[15]) == (0x00));
}

TEST_CASE("Endian Write", "[wpiutil][llvm]") {
  unsigned char data[5];
  endian::write<int32_t, unaligned>(data, -1362446643, std::endian::big);
  CHECK((data[0]) == (0xAE));
  CHECK((data[1]) == (0xCA));
  CHECK((data[2]) == (0xB6));
  CHECK((data[3]) == (0xCD));
  endian::write<int32_t, unaligned>(data + 1, -1362446643,
                                    std::endian::big);
  CHECK((data[1]) == (0xAE));
  CHECK((data[2]) == (0xCA));
  CHECK((data[3]) == (0xB6));
  CHECK((data[4]) == (0xCD));

  endian::write<int32_t, unaligned>(data, -1362446643,
                                    std::endian::little);
  CHECK((data[0]) == (0xCD));
  CHECK((data[1]) == (0xB6));
  CHECK((data[2]) == (0xCA));
  CHECK((data[3]) == (0xAE));
  endian::write<int32_t, unaligned>(data + 1, -1362446643,
                                    std::endian::little);
  CHECK((data[1]) == (0xCD));
  CHECK((data[2]) == (0xB6));
  CHECK((data[3]) == (0xCA));
  CHECK((data[4]) == (0xAE));
}

TEST_CASE("Endian PackedEndianSpecificIntegral", "[wpiutil][llvm]") {
  // These are 5 bytes so we can be sure at least one of the reads is unaligned.
  unsigned char big[] = {0x00, 0x01, 0x02, 0x03, 0x04};
  unsigned char little[] = {0x00, 0x04, 0x03, 0x02, 0x01};
  big32_t    *big_val    =
    reinterpret_cast<big32_t *>(big + 1);
  little32_t *little_val =
    reinterpret_cast<little32_t *>(little + 1);

  CHECK((*big_val) == (*little_val));
  CHECK((big_val->value()) == (little_val->value()));
}

} // end anon namespace
