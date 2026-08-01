//===- unittests/Support/MathExtrasTest.cpp - math utils tests ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/MathExtras.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <limits>

using namespace wpi::util;

namespace {

TEST_CASE("MathExtras onesMask", "[wpiutil][llvm]") {
  CHECK((0U) == (maskLeadingOnes<uint8_t>(0)));
  CHECK((0U) == (maskTrailingOnes<uint8_t>(0)));
  CHECK((0U) == (maskLeadingOnes<uint16_t>(0)));
  CHECK((0U) == (maskTrailingOnes<uint16_t>(0)));
  CHECK((0U) == (maskLeadingOnes<uint32_t>(0)));
  CHECK((0U) == (maskTrailingOnes<uint32_t>(0)));
  CHECK((0U) == (maskLeadingOnes<uint64_t>(0)));
  CHECK((0U) == (maskTrailingOnes<uint64_t>(0)));

  CHECK((0x00000003U) == (maskTrailingOnes<uint32_t>(2U)));
  CHECK((0xC0000000U) == (maskLeadingOnes<uint32_t>(2U)));

  CHECK((0x000007FFU) == (maskTrailingOnes<uint32_t>(11U)));
  CHECK((0xFFE00000U) == (maskLeadingOnes<uint32_t>(11U)));

  CHECK((0xFFFFFFFFU) == (maskTrailingOnes<uint32_t>(32U)));
  CHECK((0xFFFFFFFFU) == (maskLeadingOnes<uint32_t>(32U)));
  CHECK((0xFFFFFFFFFFFFFFFFULL) == (maskTrailingOnes<uint64_t>(64U)));
  CHECK((0xFFFFFFFFFFFFFFFFULL) == (maskLeadingOnes<uint64_t>(64U)));

  CHECK((0x0000FFFFFFFFFFFFULL) == (maskTrailingOnes<uint64_t>(48U)));
  CHECK((0xFFFFFFFFFFFF0000ULL) == (maskLeadingOnes<uint64_t>(48U)));
}

TEST_CASE("MathExtras isIntN", "[wpiutil][llvm]") {
  CHECK(isIntN(16, 32767));
  CHECK_FALSE(isIntN(16, 32768));
  CHECK(isIntN(0, 0));
  CHECK_FALSE(isIntN(0, 1));
  CHECK_FALSE(isIntN(0, -1));
}

TEST_CASE("MathExtras isUIntN", "[wpiutil][llvm]") {
  CHECK(isUIntN(16, 65535));
  CHECK_FALSE(isUIntN(16, 65536));
  CHECK(isUIntN(1, 0));
  CHECK(isUIntN(6, 63));
  CHECK(isUIntN(0, 0));
  CHECK_FALSE(isUIntN(0, 1));
}

TEST_CASE("MathExtras maxIntN", "[wpiutil][llvm]") {
  CHECK((32767) == (maxIntN(16)));
  CHECK((2147483647) == (maxIntN(32)));
  CHECK((std::numeric_limits<int32_t>::max()) == (maxIntN(32)));
  CHECK((std::numeric_limits<int64_t>::max()) == (maxIntN(64)));
  CHECK((0) == (maxIntN(0)));
}

TEST_CASE("MathExtras minIntN", "[wpiutil][llvm]") {
  CHECK((-32768LL) == (minIntN(16)));
  CHECK((-64LL) == (minIntN(7)));
  CHECK((std::numeric_limits<int32_t>::min()) == (minIntN(32)));
  CHECK((std::numeric_limits<int64_t>::min()) == (minIntN(64)));
  CHECK((0) == (minIntN(0)));
}

TEST_CASE("MathExtras maxUIntN", "[wpiutil][llvm]") {
  CHECK((0xffffULL) == (maxUIntN(16)));
  CHECK((0xffffffffULL) == (maxUIntN(32)));
  CHECK((0xffffffffffffffffULL) == (maxUIntN(64)));
  CHECK((1ULL) == (maxUIntN(1)));
  CHECK((0x0fULL) == (maxUIntN(4)));
  CHECK((0ULL) == (maxUIntN(0)));
}

TEST_CASE("MathExtras reverseBits", "[wpiutil][llvm]") {
  uint8_t NZ8 = 42;
  uint16_t NZ16 = 42;
  uint32_t NZ32 = 42;
  uint64_t NZ64 = 42;
  CHECK((0x54ULL) == (reverseBits(NZ8)));
  CHECK((0x5400ULL) == (reverseBits(NZ16)));
  CHECK((0x54000000ULL) == (reverseBits(NZ32)));
  CHECK((0x5400000000000000ULL) == (reverseBits(NZ64)));
}

TEST_CASE("MathExtras isShiftedMask_32", "[wpiutil][llvm]") {
  CHECK_FALSE(isShiftedMask_32(0x01010101));
  CHECK(isShiftedMask_32(0xf0000000));
  CHECK(isShiftedMask_32(0xffff0000));
  CHECK(isShiftedMask_32(0xff << 1));

  unsigned MaskIdx, MaskLen;
  CHECK_FALSE(isShiftedMask_32(0x01010101, MaskIdx, MaskLen));
  CHECK(isShiftedMask_32(0xf0000000, MaskIdx, MaskLen));
  CHECK((28) == ((int)MaskIdx));
  CHECK((4) == ((int)MaskLen));
  CHECK(isShiftedMask_32(0xffff0000, MaskIdx, MaskLen));
  CHECK((16) == ((int)MaskIdx));
  CHECK((16) == ((int)MaskLen));
  CHECK(isShiftedMask_32(0xff << 1, MaskIdx, MaskLen));
  CHECK((1) == ((int)MaskIdx));
  CHECK((8) == ((int)MaskLen));
}

TEST_CASE("MathExtras isShiftedMask_64", "[wpiutil][llvm]") {
  CHECK_FALSE(isShiftedMask_64(0x0101010101010101ull));
  CHECK(isShiftedMask_64(0xf000000000000000ull));
  CHECK(isShiftedMask_64(0xffff000000000000ull));
  CHECK(isShiftedMask_64(0xffull << 55));

  unsigned MaskIdx, MaskLen;
  CHECK_FALSE(isShiftedMask_64(0x0101010101010101ull, MaskIdx, MaskLen));
  CHECK(isShiftedMask_64(0xf000000000000000ull, MaskIdx, MaskLen));
  CHECK((60) == ((int)MaskIdx));
  CHECK((4) == ((int)MaskLen));
  CHECK(isShiftedMask_64(0xffff000000000000ull, MaskIdx, MaskLen));
  CHECK((48) == ((int)MaskIdx));
  CHECK((16) == ((int)MaskLen));
  CHECK(isShiftedMask_64(0xffull << 55, MaskIdx, MaskLen));
  CHECK((55) == ((int)MaskIdx));
  CHECK((8) == ((int)MaskLen));
}

TEST_CASE("MathExtras isPowerOf2_32", "[wpiutil][llvm]") {
  CHECK_FALSE(isPowerOf2_32(0));
  CHECK(isPowerOf2_32(1 << 6));
  CHECK(isPowerOf2_32(1 << 12));
  CHECK_FALSE(isPowerOf2_32((1 << 19) + 3));
  CHECK_FALSE(isPowerOf2_32(0xABCDEF0));
}

TEST_CASE("MathExtras isPowerOf2_64", "[wpiutil][llvm]") {
  CHECK_FALSE(isPowerOf2_64(0));
  CHECK(isPowerOf2_64(1LL << 46));
  CHECK(isPowerOf2_64(1LL << 12));
  CHECK_FALSE(isPowerOf2_64((1LL << 53) + 3));
  CHECK_FALSE(isPowerOf2_64(0xABCDEF0ABCDEF0LL));
}

TEST_CASE("MathExtras PowerOf2Ceil", "[wpiutil][llvm]") {
  CHECK((0U) == (PowerOf2Ceil(0U)));
  CHECK((8U) == (PowerOf2Ceil(8U)));
  CHECK((8U) == (PowerOf2Ceil(7U)));
}

TEST_CASE("MathExtras ConstantLog2", "[wpiutil][llvm]") {
  CHECK((ConstantLog2<1ULL << 0>()) == (0U));
  CHECK((ConstantLog2<1ULL << 1>()) == (1U));
  CHECK((ConstantLog2<1ULL << 2>()) == (2U));
  CHECK((ConstantLog2<1ULL << 3>()) == (3U));
  CHECK((ConstantLog2<1ULL << 4>()) == (4U));
  CHECK((ConstantLog2<1ULL << 5>()) == (5U));
  CHECK((ConstantLog2<1ULL << 6>()) == (6U));
  CHECK((ConstantLog2<1ULL << 7>()) == (7U));
  CHECK((ConstantLog2<1ULL << 8>()) == (8U));
  CHECK((ConstantLog2<1ULL << 9>()) == (9U));
  CHECK((ConstantLog2<1ULL << 10>()) == (10U));
  CHECK((ConstantLog2<1ULL << 11>()) == (11U));
  CHECK((ConstantLog2<1ULL << 12>()) == (12U));
  CHECK((ConstantLog2<1ULL << 13>()) == (13U));
  CHECK((ConstantLog2<1ULL << 14>()) == (14U));
  CHECK((ConstantLog2<1ULL << 15>()) == (15U));
}

TEST_CASE("MathExtras MinAlign", "[wpiutil][llvm]") {
  CHECK((1u) == (MinAlign(2, 3)));
  CHECK((2u) == (MinAlign(2, 4)));
  CHECK((1u) == (MinAlign(17, 64)));
  CHECK((256u) == (MinAlign(256, 512)));
  CHECK((2u) == (MinAlign(0, 2)));
}

TEST_CASE("MathExtras NextPowerOf2", "[wpiutil][llvm]") {
  CHECK((4u) == (NextPowerOf2(3)));
  CHECK((16u) == (NextPowerOf2(15)));
  CHECK((256u) == (NextPowerOf2(128)));
}

TEST_CASE("MathExtras AlignTo", "[wpiutil][llvm]") {
  CHECK((8u) == (alignTo(5, 8)));
  CHECK((24u) == (alignTo(17, 8)));
  CHECK((0u) == (alignTo(~0LL, 8)));
  CHECK((8u) == (alignTo(5ULL, 8ULL)));

  CHECK((8u) == (alignTo<8>(5)));
  CHECK((24u) == (alignTo<8>(17)));
  CHECK((0u) == (alignTo<8>(~0LL)));
  CHECK((254u) == (alignTo<static_cast<uint8_t>(127)>(static_cast<uint8_t>(200))));

  CHECK((7u) == (alignTo(5, 8, 7)));
  CHECK((17u) == (alignTo(17, 8, 1)));
  CHECK((3u) == (alignTo(~0LL, 8, 3)));
  CHECK((552u) == (alignTo(321, 255, 42)));
  CHECK((std::numeric_limits<uint32_t>::max()) == (alignTo(std::numeric_limits<uint32_t>::max(), 2, 1)));

  // Overflow.
  CHECK((0u) == (alignTo(static_cast<uint8_t>(200), static_cast<uint8_t>(128))));
  CHECK((0u) == (alignTo<static_cast<uint8_t>(128)>(static_cast<uint8_t>(200))));
  CHECK((0u) == (alignTo(static_cast<uint8_t>(200), static_cast<uint8_t>(128),
                        static_cast<uint8_t>(0))));
  CHECK((0u) == (alignTo(std::numeric_limits<uint32_t>::max(), 2)));
}

TEST_CASE("MathExtras AlignToPowerOf2", "[wpiutil][llvm]") {
  CHECK((0u) == (alignToPowerOf2(0u, 8)));
  CHECK((8u) == (alignToPowerOf2(5, 8)));
  CHECK((24u) == (alignToPowerOf2(17, 8)));
  CHECK((0u) == (alignToPowerOf2(~0LL, 8)));
  CHECK((240u) == (alignToPowerOf2(240, 16)));

  // Overflow.
  CHECK((0u) == (alignToPowerOf2(static_cast<uint8_t>(200),
                                static_cast<uint8_t>(128))));
  CHECK((0u) == (alignToPowerOf2(std::numeric_limits<uint32_t>::max(), 2)));
}

TEST_CASE("MathExtras AlignDown", "[wpiutil][llvm]") {
  CHECK((0u) == (alignDown(5, 8)));
  CHECK((16u) == (alignDown(17, 8)));
  CHECK((std::numeric_limits<uint32_t>::max() - 1) == (alignDown(std::numeric_limits<uint32_t>::max(), 2)));
}

template <typename T> void SaturatingAddTestHelper() {
  const T Max = std::numeric_limits<T>::max();
  bool ResultOverflowed;

  CHECK((T(3)) == (SaturatingAdd(T(1), T(2))));
  CHECK((T(3)) == (SaturatingAdd(T(1), T(2), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(Max, T(1))));
  CHECK((Max) == (SaturatingAdd(Max, T(1), &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(T(1), T(Max - 1))));
  CHECK((Max) == (SaturatingAdd(T(1), T(Max - 1), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(T(1), Max)));
  CHECK((Max) == (SaturatingAdd(T(1), Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(Max, Max)));
  CHECK((Max) == (SaturatingAdd(Max, Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((T(6)) == (SaturatingAdd(T(1), T(2), T(3))));
  CHECK((T(6)) == (SaturatingAdd(T(1), T(2), T(3), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(10)) == (SaturatingAdd(T(1), T(2), T(3), T(4))));
  CHECK((T(10)) == (SaturatingAdd(T(1), T(2), T(3), T(4), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(Max, T(0), T(0))));
  CHECK((Max) == (SaturatingAdd(Max, T(0), T(0), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(T(0), T(0), Max)));
  CHECK((Max) == (SaturatingAdd(T(0), T(0), Max, &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(Max, T(0), T(1))));
  CHECK((Max) == (SaturatingAdd(Max, T(0), T(1), &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(T(0), T(1), Max)));
  CHECK((Max) == (SaturatingAdd(T(0), T(1), Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(T(1), T(Max - 2), T(1))));
  CHECK((Max) == (SaturatingAdd(T(1), T(Max - 2), T(1), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(T(1), T(1), T(Max - 2))));
  CHECK((Max) == (SaturatingAdd(T(1), T(1), T(Max - 2), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((Max) == (SaturatingAdd(Max, Max, Max)));
  CHECK((Max) == (SaturatingAdd(Max, Max, Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);
}

TEST_CASE("MathExtras SaturatingAdd", "[wpiutil][llvm]") {
  SaturatingAddTestHelper<uint8_t>();
  SaturatingAddTestHelper<uint16_t>();
  SaturatingAddTestHelper<uint32_t>();
  SaturatingAddTestHelper<uint64_t>();
}

template<typename T>
void SaturatingMultiplyTestHelper()
{
  const T Max = std::numeric_limits<T>::max();
  bool ResultOverflowed;

  // Test basic multiplication.
  CHECK((T(6)) == (SaturatingMultiply(T(2), T(3))));
  CHECK((T(6)) == (SaturatingMultiply(T(2), T(3), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(6)) == (SaturatingMultiply(T(3), T(2))));
  CHECK((T(6)) == (SaturatingMultiply(T(3), T(2), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  // Test multiplication by zero.
  CHECK((T(0)) == (SaturatingMultiply(T(0), T(0))));
  CHECK((T(0)) == (SaturatingMultiply(T(0), T(0), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(0)) == (SaturatingMultiply(T(1), T(0))));
  CHECK((T(0)) == (SaturatingMultiply(T(1), T(0), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(0)) == (SaturatingMultiply(T(0), T(1))));
  CHECK((T(0)) == (SaturatingMultiply(T(0), T(1), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(0)) == (SaturatingMultiply(Max, T(0))));
  CHECK((T(0)) == (SaturatingMultiply(Max, T(0), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(0)) == (SaturatingMultiply(T(0), Max)));
  CHECK((T(0)) == (SaturatingMultiply(T(0), Max, &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  // Test multiplication by maximum value.
  CHECK((Max) == (SaturatingMultiply(Max, T(2))));
  CHECK((Max) == (SaturatingMultiply(Max, T(2), &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingMultiply(T(2), Max)));
  CHECK((Max) == (SaturatingMultiply(T(2), Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingMultiply(Max, Max)));
  CHECK((Max) == (SaturatingMultiply(Max, Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  // Test interesting boundary conditions for algorithm -
  // ((1 << A) - 1) * ((1 << B) + K) for K in [-1, 0, 1]
  // and A + B == std::numeric_limits<T>::digits.
  // We expect overflow iff A > B and K = 1.
  const int Digits = std::numeric_limits<T>::digits;
  for (int A = 1, B = Digits - 1; B >= 1; ++A, --B) {
    for (int K = -1; K <= 1; ++K) {
      T X = (T(1) << A) - T(1);
      T Y = (T(1) << B) + K;
      bool OverflowExpected = A > B && K == 1;

      if(OverflowExpected) {
        CHECK((Max) == (SaturatingMultiply(X, Y)));
        CHECK((Max) == (SaturatingMultiply(X, Y, &ResultOverflowed)));
        CHECK(ResultOverflowed);
      } else {
        CHECK((X * Y) == (SaturatingMultiply(X, Y)));
        CHECK((X * Y) == (SaturatingMultiply(X, Y, &ResultOverflowed)));
        CHECK_FALSE(ResultOverflowed);
      }
    }
  }
}

TEST_CASE("MathExtras SaturatingMultiply", "[wpiutil][llvm]") {
  SaturatingMultiplyTestHelper<uint8_t>();
  SaturatingMultiplyTestHelper<uint16_t>();
  SaturatingMultiplyTestHelper<uint32_t>();
  SaturatingMultiplyTestHelper<uint64_t>();
}

template<typename T>
void SaturatingMultiplyAddTestHelper()
{
  const T Max = std::numeric_limits<T>::max();
  bool ResultOverflowed;

  // Test basic multiply-add.
  CHECK((T(16)) == (SaturatingMultiplyAdd(T(2), T(3), T(10))));
  CHECK((T(16)) == (SaturatingMultiplyAdd(T(2), T(3), T(10), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  // Test multiply overflows, add doesn't overflow
  CHECK((Max) == (SaturatingMultiplyAdd(Max, Max, T(0), &ResultOverflowed)));
  CHECK(ResultOverflowed);

  // Test multiply doesn't overflow, add overflows
  CHECK((Max) == (SaturatingMultiplyAdd(T(1), T(1), Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  // Test multiply-add with Max as operand
  CHECK((Max) == (SaturatingMultiplyAdd(T(1), T(1), Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingMultiplyAdd(T(1), Max, T(1), &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingMultiplyAdd(Max, Max, T(1), &ResultOverflowed)));
  CHECK(ResultOverflowed);

  CHECK((Max) == (SaturatingMultiplyAdd(Max, Max, Max, &ResultOverflowed)));
  CHECK(ResultOverflowed);

  // Test multiply-add with 0 as operand
  CHECK((T(1)) == (SaturatingMultiplyAdd(T(1), T(1), T(0), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(1)) == (SaturatingMultiplyAdd(T(1), T(0), T(1), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(1)) == (SaturatingMultiplyAdd(T(0), T(0), T(1), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

  CHECK((T(0)) == (SaturatingMultiplyAdd(T(0), T(0), T(0), &ResultOverflowed)));
  CHECK_FALSE(ResultOverflowed);

}

TEST_CASE("MathExtras SaturatingMultiplyAdd", "[wpiutil][llvm]") {
  SaturatingMultiplyAddTestHelper<uint8_t>();
  SaturatingMultiplyAddTestHelper<uint16_t>();
  SaturatingMultiplyAddTestHelper<uint32_t>();
  SaturatingMultiplyAddTestHelper<uint64_t>();
}

TEST_CASE("MathExtras IsShiftedUInt", "[wpiutil][llvm]") {
  CHECK((isShiftedUInt<1, 0>(0)));
  CHECK((isShiftedUInt<1, 0>(1)));
  CHECK_FALSE((isShiftedUInt<1, 0>(2)));
  CHECK_FALSE((isShiftedUInt<1, 0>(3)));
  CHECK_FALSE((isShiftedUInt<1, 0>(0x8000000000000000)));
  CHECK((isShiftedUInt<1, 63>(0x8000000000000000)));
  CHECK((isShiftedUInt<2, 62>(0xC000000000000000)));
  CHECK_FALSE((isShiftedUInt<2, 62>(0xE000000000000000)));

  // 0x201 is ten bits long and has a 1 in the MSB and LSB.
  CHECK((isShiftedUInt<10, 5>(uint64_t(0x201) << 5)));
  CHECK_FALSE((isShiftedUInt<10, 5>(uint64_t(0x201) << 4)));
  CHECK_FALSE((isShiftedUInt<10, 5>(uint64_t(0x201) << 6)));
}

TEST_CASE("MathExtras IsShiftedInt", "[wpiutil][llvm]") {
  CHECK((isShiftedInt<1, 0>(0)));
  CHECK((isShiftedInt<1, 0>(-1)));
  CHECK_FALSE((isShiftedInt<1, 0>(2)));
  CHECK_FALSE((isShiftedInt<1, 0>(3)));
  CHECK_FALSE((isShiftedInt<1, 0>(0x8000000000000000)));
  CHECK((isShiftedInt<1, 63>(0x8000000000000000)));
  CHECK((isShiftedInt<2, 62>(0xC000000000000000)));
  CHECK_FALSE((isShiftedInt<2, 62>(0xE000000000000000)));

  // 0x201 is ten bits long and has a 1 in the MSB and LSB.
  CHECK((isShiftedInt<11, 5>(int64_t(0x201) << 5)));
  CHECK_FALSE((isShiftedInt<11, 5>(int64_t(0x201) << 3)));
  CHECK_FALSE((isShiftedInt<11, 5>(int64_t(0x201) << 6)));
  CHECK((isShiftedInt<11, 5>(-(int64_t(0x201) << 5))));
  CHECK_FALSE((isShiftedInt<11, 5>(-(int64_t(0x201) << 3))));
  CHECK_FALSE((isShiftedInt<11, 5>(-(int64_t(0x201) << 6))));

  CHECK((isShiftedInt<6, 10>(-(int64_t(1) << 15))));
  CHECK_FALSE((isShiftedInt<6, 10>(int64_t(1) << 15)));
}

TEST_CASE("MathExtras DivideNearest", "[wpiutil][llvm]") {
  CHECK((divideNearest(14, 3)) == (5u));
  CHECK((divideNearest(15, 3)) == (5u));
  CHECK((divideNearest(0, 3)) == (0u));
  CHECK((divideNearest(5, 4)) == (1u));
  CHECK((divideNearest(6, 4)) == (2u));
  CHECK((divideNearest(3, 1)) == (3u));
  CHECK((divideNearest(3, 6)) == (1u));
  CHECK((divideNearest(3, 7)) == (0u));
  CHECK((divideNearest(std::numeric_limits<uint32_t>::max(), 2)) == (std::numeric_limits<uint32_t>::max() / 2 + 1));
  CHECK((divideNearest(std::numeric_limits<uint64_t>::max(), 2)) == (std::numeric_limits<uint64_t>::max() / 2 + 1));
  CHECK((divideNearest(std::numeric_limits<uint64_t>::max(), 1)) == (std::numeric_limits<uint64_t>::max()));
  CHECK((divideNearest(std::numeric_limits<uint64_t>::max() - 1,
                          std::numeric_limits<uint64_t>::max())) == (1u));
}

TEST_CASE("MathExtras DivideCeil", "[wpiutil][llvm]") {
  CHECK((divideCeil(14, 3)) == (5u));
  CHECK((divideCeil(15, 3)) == (5u));
  CHECK((divideCeil(0, 3)) == (0u));
  CHECK((divideCeil(5, 4)) == (2u));
  CHECK((divideCeil(6, 4)) == (2u));
  CHECK((divideCeil(3, 1)) == (3u));
  CHECK((divideCeil(3, 6)) == (1u));
  CHECK((divideCeil(3, 7)) == (1u));
  CHECK((divideCeil(std::numeric_limits<uint32_t>::max(), 2)) == (std::numeric_limits<uint32_t>::max() / 2 + 1));
  CHECK((divideCeil(std::numeric_limits<uint64_t>::max(), 2)) == (std::numeric_limits<uint64_t>::max() / 2 + 1));
  CHECK((divideCeil(std::numeric_limits<uint64_t>::max(), 1)) == (std::numeric_limits<uint64_t>::max()));

  CHECK((divideCeilSigned(14, 3)) == (5));
  CHECK((divideCeilSigned(15, 3)) == (5));
  CHECK((divideCeilSigned(14, -3)) == (-4));
  CHECK((divideCeilSigned(-14, -3)) == (5));
  CHECK((divideCeilSigned(-14, 3)) == (-4));
  CHECK((divideCeilSigned(-15, 3)) == (-5));
  CHECK((divideCeilSigned(0, 3)) == (0));
  CHECK((divideCeilSigned(0, -3)) == (0));
  CHECK((divideCeilSigned(std::numeric_limits<int32_t>::max(), 2)) == (std::numeric_limits<int32_t>::max() / 2 + 1));
  CHECK((divideCeilSigned(std::numeric_limits<int64_t>::max(), 2)) == (std::numeric_limits<int64_t>::max() / 2 + 1));
  CHECK((divideCeilSigned(std::numeric_limits<int32_t>::max(), -2)) == (std::numeric_limits<int32_t>::min() / 2 + 1));
  CHECK((divideCeilSigned(std::numeric_limits<int64_t>::max(), -2)) == (std::numeric_limits<int64_t>::min() / 2 + 1));
  CHECK((divideCeilSigned(std::numeric_limits<int64_t>::min(), 1)) == (std::numeric_limits<int64_t>::min()));

  // Overflow.
  CHECK(divideSignedWouldOverflow(std::numeric_limits<int8_t>::min(), -1));
  CHECK(divideSignedWouldOverflow(std::numeric_limits<int64_t>::min(), -1));
}

TEST_CASE("MathExtras DivideFloorSigned", "[wpiutil][llvm]") {
  CHECK((divideFloorSigned(14, 3)) == (4));
  CHECK((divideFloorSigned(15, 3)) == (5));
  CHECK((divideFloorSigned(14, -3)) == (-5));
  CHECK((divideFloorSigned(-14, -3)) == (4));
  CHECK((divideFloorSigned(-14, 3)) == (-5));
  CHECK((divideFloorSigned(-15, 3)) == (-5));
  CHECK((divideFloorSigned(0, 3)) == (0));
  CHECK((divideFloorSigned(0, -3)) == (0));
  CHECK((divideFloorSigned(std::numeric_limits<int32_t>::max(), 2)) == (std::numeric_limits<int32_t>::max() / 2));
  CHECK((divideFloorSigned(std::numeric_limits<int64_t>::max(), 2)) == (std::numeric_limits<int64_t>::max() / 2));
  CHECK((divideFloorSigned(std::numeric_limits<int32_t>::max(), -2)) == (std::numeric_limits<int32_t>::min() / 2));
  CHECK((divideFloorSigned(std::numeric_limits<int64_t>::max(), -2)) == (std::numeric_limits<int64_t>::min() / 2));
  CHECK((divideFloorSigned(std::numeric_limits<int64_t>::min(), 1)) == (std::numeric_limits<int64_t>::min()));

  // Same overflow condition, divideSignedWouldOverflow, applies.
}

TEST_CASE("MathExtras Mod", "[wpiutil][llvm]") {
  CHECK((mod(1, 14)) == (1));
  CHECK((mod(-1, 14)) == (13));
  CHECK((mod(14, 3)) == (2));
  CHECK((mod(15, 3)) == (0));
  CHECK((mod(-14, 3)) == (1));
  CHECK((mod(-15, 3)) == (0));
  CHECK((mod(0, 3)) == (0));
}

#define WPIUTIL_TEST_TYPES_OverflowTest signed char, short, int, long, long long

template <typename T>
class OverflowTest {};

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest AddNoOverflow", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  CHECK_FALSE(AddOverflow<TestType>(1, 2, Result));
  CHECK((Result) == (TestType(3)));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest AddOverflowToNegative", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MaxValue = std::numeric_limits<TestType>::max();
  CHECK(AddOverflow<TestType>(MaxValue, MaxValue, Result));
  CHECK((Result) == (TestType(-2)));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest AddOverflowToMin", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MaxValue = std::numeric_limits<TestType>::max();
  CHECK(AddOverflow<TestType>(MaxValue, TestType(1), Result));
  CHECK((Result) == (std::numeric_limits<TestType>::min()));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest AddOverflowToZero", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(AddOverflow<TestType>(MinValue, MinValue, Result));
  CHECK((Result) == (TestType(0)));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest AddOverflowToMax", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(AddOverflow<TestType>(MinValue, TestType(-1), Result));
  CHECK((Result) == (std::numeric_limits<TestType>::max()));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest SubNoOverflow", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  CHECK_FALSE(SubOverflow<TestType>(1, 2, Result));
  CHECK((Result) == (TestType(-1)));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest SubOverflowToMax", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(SubOverflow<TestType>(0, MinValue, Result));
  CHECK((Result) == (MinValue));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest SubOverflowToMin", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(SubOverflow<TestType>(0, MinValue, Result));
  CHECK((Result) == (MinValue));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest SubOverflowToNegative", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MaxValue = std::numeric_limits<TestType>::max();
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(SubOverflow<TestType>(MaxValue, MinValue, Result));
  CHECK((Result) == (TestType(-1)));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest SubOverflowToPositive", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MaxValue = std::numeric_limits<TestType>::max();
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(SubOverflow<TestType>(MinValue, MaxValue, Result));
  CHECK((Result) == (TestType(1)));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest MulNoOverflow", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  CHECK_FALSE(MulOverflow<TestType>(1, 2, Result));
  CHECK((Result) == (2));
  CHECK_FALSE(MulOverflow<TestType>(-1, 3, Result));
  CHECK((Result) == (-3));
  CHECK_FALSE(MulOverflow<TestType>(4, -2, Result));
  CHECK((Result) == (-8));
  CHECK_FALSE(MulOverflow<TestType>(-6, -5, Result));
  CHECK((Result) == (30));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest MulNoOverflowToMax", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MaxValue = std::numeric_limits<TestType>::max();
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK_FALSE(MulOverflow<TestType>(MinValue + 1, -1, Result));
  CHECK((Result) == (MaxValue));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest MulOverflowToMin", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MinValue = std::numeric_limits<TestType>::min();
  CHECK(MulOverflow<TestType>(MinValue, -1, Result));
  CHECK((Result) == (MinValue));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest MulOverflowMax", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  auto MinValue = std::numeric_limits<TestType>::min();
  auto MaxValue = std::numeric_limits<TestType>::max();
  CHECK(MulOverflow<TestType>(MinValue, MinValue, Result));
  CHECK((Result) == (0));
  CHECK(MulOverflow<TestType>(MaxValue, MaxValue, Result));
  CHECK((Result) == (1));
}

TEMPLATE_TEST_CASE_METHOD(OverflowTest, "OverflowTest MulResultZero", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_OverflowTest) {
  TestType Result;
  CHECK_FALSE(MulOverflow<TestType>(4, 0, Result));
  CHECK((Result) == (TestType(0)));
  CHECK_FALSE(MulOverflow<TestType>(-5, 0, Result));
  CHECK((Result) == (TestType(0)));
  CHECK_FALSE(MulOverflow<TestType>(0, 5, Result));
  CHECK((Result) == (TestType(0)));
  CHECK_FALSE(MulOverflow<TestType>(0, -5, Result));
  CHECK((Result) == (TestType(0)));
}
} // namespace
