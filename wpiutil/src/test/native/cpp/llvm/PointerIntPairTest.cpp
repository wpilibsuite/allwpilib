//===- llvm/unittest/ADT/PointerIntPairTest.cpp - Unit tests --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/PointerIntPair.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <limits>
using namespace wpi::util;

namespace {

TEST_CASE("PointerIntPairTest GetSet", "[wpiutil][llvm]") {
  struct S {
    int i;
  };
  S s;

  PointerIntPair<S *, 2> Pair(&s, 1U);
  CHECK(&s == Pair.getPointer());
  CHECK(1U == Pair.getInt());

  Pair.setInt(2);
  CHECK(&s == Pair.getPointer());
  CHECK(2U == Pair.getInt());

  Pair.setPointer(nullptr);
  CHECK(nullptr == Pair.getPointer());
  CHECK(2U == Pair.getInt());

  Pair.setPointerAndInt(&s, 3U);
  CHECK(&s == Pair.getPointer());
  CHECK(3U == Pair.getInt());

  // Make sure that we can perform all of our operations on enum classes.
  //
  // The concern is that enum classes are only explicitly convertible to
  // integers. This means that if we assume in PointerIntPair this, a
  // compilation error will result. This group of tests exercises the enum class
  // code to make sure that we do not run into such issues in the future.
  enum class E : unsigned {
    Case1,
    Case2,
    Case3,
  };
  PointerIntPair<S *, 2, E> Pair2(&s, E::Case1);
  CHECK(&s == Pair2.getPointer());
  CHECK(E::Case1 == Pair2.getInt());

  Pair2.setInt(E::Case2);
  CHECK(&s == Pair2.getPointer());
  CHECK(E::Case2 == Pair2.getInt());

  Pair2.setPointer(nullptr);
  CHECK(nullptr == Pair2.getPointer());
  CHECK(E::Case2 == Pair2.getInt());

  Pair2.setPointerAndInt(&s, E::Case3);
  CHECK(&s == Pair2.getPointer());
  CHECK(E::Case3 == Pair2.getInt());

  auto [Pointer2, Int2] = Pair2;
  CHECK(Pair2.getPointer() == Pointer2);
  CHECK(Pair2.getInt() == Int2);

  static_assert(std::is_trivially_copyable_v<PointerIntPair<S *, 2, E>>,
                "trivially copyable");
}

TEST_CASE("PointerIntPairTest DefaultInitialize", "[wpiutil][llvm]") {
  PointerIntPair<float *, 2> Pair;
  CHECK(nullptr == Pair.getPointer());
  CHECK(0U == Pair.getInt());
}

// In real code this would be a word-sized integer limited to 31 bits.
struct Fixnum31 {
  uintptr_t Value;
};
struct FixnumPointerTraits {
  static inline void *getAsVoidPointer(Fixnum31 Num) {
    return reinterpret_cast<void *>(Num.Value << NumLowBitsAvailable);
  }
  static inline Fixnum31 getFromVoidPointer(void *P) {
    // In real code this would assert that the value is in range.
    return {reinterpret_cast<uintptr_t>(P) >> NumLowBitsAvailable};
  }
  static constexpr int NumLowBitsAvailable =
      std::numeric_limits<uintptr_t>::digits - 31;
};
TEST_CASE("PointerIntPairTest ManyUnusedBits", "[wpiutil][llvm]") {

  PointerIntPair<Fixnum31, 1, bool, FixnumPointerTraits> pair;
  CHECK(static_cast<uintptr_t>(0) == pair.getPointer().Value);
  CHECK_FALSE(pair.getInt());

  pair.setPointerAndInt({ 0x7FFFFFFF }, true );
  CHECK(static_cast<uintptr_t>(0x7FFFFFFF) == pair.getPointer().Value);
  CHECK(pair.getInt());

  CHECK(FixnumPointerTraits::NumLowBitsAvailable - 1 == (int)PointerLikeTypeTraits<decltype(pair)>::NumLowBitsAvailable);

  static_assert(std::is_trivially_copyable_v<
                    PointerIntPair<Fixnum31, 1, bool, FixnumPointerTraits>>,
                "trivially copyable");
}

TEST_CASE("PointerIntPairTest TypePunning", "[wpiutil][llvm]") {
  int I = 0;
  int *IntPtr = &I;

  int **IntPtrBegin = &IntPtr;
  int **IntPtrEnd = IntPtrBegin + 1;

  PointerIntPair<int *, 1> Pair;
  int **PairAddr = Pair.getAddrOfPointer();

  while (IntPtrBegin != IntPtrEnd) {
    *PairAddr = *IntPtrBegin;
    ++PairAddr;
    ++IntPtrBegin;
  }
  CHECK(Pair.getPointer() == IntPtr);
}

} // end anonymous namespace
