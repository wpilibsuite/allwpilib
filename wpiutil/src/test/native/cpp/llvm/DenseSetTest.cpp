//===- llvm/unittest/ADT/DenseSetTest.cpp - DenseSet unit tests --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/DenseSet.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <type_traits>
#include <vector>

using namespace wpi::util;

namespace {

static_assert(
    std::is_const_v<
        std::remove_pointer_t<DenseSet<int>::const_iterator::pointer>>,
    "Iterator pointer type should be const");
static_assert(
    std::is_const_v<
        std::remove_reference_t<DenseSet<int>::const_iterator::reference>>,
    "Iterator reference type should be const");

// Test hashing with a set of only two entries.
TEST_CASE("DenseSetTest DoubleEntrySetTest", "[wpiutil][llvm]") {
  wpi::util::DenseSet<unsigned> set(2);
  set.insert(0);
  set.insert(1);
  // Original failure was an infinite loop in this call:
  CHECK(0u == set.count(2));
}

TEST_CASE("DenseSetTest CtorRange", "[wpiutil][llvm]") {
  constexpr unsigned Args[] = {3, 1, 2};
  wpi::util::DenseSet<unsigned> set(wpi::util::from_range, Args);
  CHECK_THAT(set, Catch::Matchers::UnorderedRangeEquals(std::vector<unsigned>{1, 2, 3}));
}

TEST_CASE("DenseSetTest CtorRangeImplicitConversion", "[wpiutil][llvm]") {
  constexpr char Args[] = {3, 1, 2};
  wpi::util::DenseSet<unsigned> set(wpi::util::from_range, Args);
  CHECK_THAT(set, Catch::Matchers::UnorderedRangeEquals(std::vector<unsigned>{1, 2, 3}));
}

TEST_CASE("SmallDenseSetTest CtorRange", "[wpiutil][llvm]") {
  constexpr unsigned Args[] = {9, 7, 8};
  wpi::util::SmallDenseSet<unsigned> set(wpi::util::from_range, Args);
  CHECK_THAT(set, Catch::Matchers::UnorderedRangeEquals(std::vector<unsigned>{7, 8, 9}));
}

TEST_CASE("DenseSetTest InsertRange", "[wpiutil][llvm]") {
  wpi::util::DenseSet<unsigned> set;
  constexpr unsigned Args[] = {3, 1, 2};
  set.insert_range(Args);
  CHECK_THAT(set, Catch::Matchers::UnorderedRangeEquals(std::vector<unsigned>{1, 2, 3}));
}

TEST_CASE("SmallDenseSetTest InsertRange", "[wpiutil][llvm]") {
  wpi::util::SmallDenseSet<unsigned> set;
  constexpr unsigned Args[] = {9, 7, 8};
  set.insert_range(Args);
  CHECK_THAT(set, Catch::Matchers::UnorderedRangeEquals(std::vector<unsigned>{7, 8, 9}));
}

TEST_CASE("DenseSetTest RemoveIf", "[wpiutil][llvm]") {
  wpi::util::DenseSet<unsigned> set;
  for (unsigned I = 0; I < 100; ++I)
    set.insert(I);

  CHECK(set.remove_if([](unsigned V) { return V % 2 == 0; }));
  CHECK(set.size() == 50u);
  for (unsigned I = 0; I < 100; ++I)
    CHECK(set.contains(I) == (I % 2 == 1));

  CHECK_FALSE(set.remove_if([](unsigned) { return false; }));
  CHECK(set.size() == 50u);
  CHECK(set.remove_if([](unsigned) { return true; }));
  CHECK(set.empty());
}

struct TestDenseSetInfo {
  static unsigned getHashValue(const unsigned& Val) { return Val * 37U; }
  static unsigned getHashValue(const char* Val) {
    return (unsigned)(Val[0] - 'a') * 37U;
  }
  static bool isEqual(const unsigned& LHS, const unsigned& RHS) {
    return LHS == RHS;
  }
  static bool isEqual(const char* LHS, const unsigned& RHS) {
    return (unsigned)(LHS[0] - 'a') == RHS;
  }
};

// Test fixture
template <typename T> class DenseSetTest {
protected:
  T Set = GetTestSet();

private:
  static T GetTestSet() {
    std::remove_const_t<T> Set;
    Set.insert(0);
    Set.insert(1);
    Set.insert(2);
    return Set;
  }
};

// Register these types for testing.
#define WPIUTIL_TEST_TYPES_DenseSetTest \
  (DenseSet<unsigned, TestDenseSetInfo>), \
  (const DenseSet<unsigned, TestDenseSetInfo>), \
  (SmallDenseSet<unsigned, 1, TestDenseSetInfo>), \
  (SmallDenseSet<unsigned, 4, TestDenseSetInfo>), \
  (const SmallDenseSet<unsigned, 4, TestDenseSetInfo>), \
  (SmallDenseSet<unsigned, 64, TestDenseSetInfo>)

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest Constructor", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  constexpr unsigned a[] = {1, 2, 4};
  TestType set(std::begin(a), std::end(a));
  CHECK(3u == set.size());
  CHECK(1u == set.count(1));
  CHECK(1u == set.count(2));
  CHECK(1u == set.count(4));
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest InitializerList", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  TestType set({1, 2, 1, 4});
  CHECK(3u == set.size());
  CHECK(1u == set.count(1));
  CHECK(1u == set.count(2));
  CHECK(1u == set.count(4));
  CHECK(0u == set.count(3));
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest InitializerListWithNonPowerOfTwoLength", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  TestType set({1, 2, 3});
  CHECK(3u == set.size());
  CHECK(1u == set.count(1));
  CHECK(1u == set.count(2));
  CHECK(1u == set.count(3));
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest ConstIteratorComparison", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  TestType set({1});
  const TestType &cset = set;
  CHECK(set.begin() == cset.begin());
  CHECK(set.end() == cset.end());
  CHECK(set.end() != cset.begin());
  CHECK(set.begin() != cset.end());
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest DefaultConstruction", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  typename TestType::iterator I, J;
  typename TestType::const_iterator CI, CJ;
  CHECK(I == J);
  CHECK(CI == CJ);
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest EmptyInitializerList", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  TestType set({});
  CHECK(0u == set.size());
  CHECK(0u == set.count(0));
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest FindAsTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  auto &set = this->Set;
  // Size tests
  CHECK(3u == set.size());

  // Normal lookup tests
  CHECK(1u == set.count(1));
  CHECK(0u == *set.find(0));
  CHECK(1u == *set.find(1));
  CHECK(2u == *set.find(2));
  CHECK(set.find(3) == set.end());

  // find_as() tests
  CHECK(0u == *set.find_as("a"));
  CHECK(1u == *set.find_as("b"));
  CHECK(2u == *set.find_as("c"));
  CHECK(set.find_as("d") == set.end());
}

TEMPLATE_TEST_CASE_METHOD(DenseSetTest, "DenseSetTest EqualityComparisonTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseSetTest) {
  TestType set1({1, 2, 3, 4});
  TestType set2({4, 3, 2, 1});
  TestType set3({2, 3, 4, 5});

  CHECK(set1 == set2);
  CHECK(set1 != set3);
}

// Simple class that counts how many moves and copy happens when growing a map
struct CountCopyAndMove {
  static int Move;
  static int Copy;
  int Value;
  CountCopyAndMove(int Value) : Value(Value) {}

  CountCopyAndMove(const CountCopyAndMove &RHS) {
    Value = RHS.Value;
    Copy++;
  }
  CountCopyAndMove &operator=(const CountCopyAndMove &RHS) {
    Value = RHS.Value;
    Copy++;
    return *this;
  }
  CountCopyAndMove(CountCopyAndMove &&RHS) {
    Value = RHS.Value;
    Move++;
  }
  CountCopyAndMove &operator=(const CountCopyAndMove &&RHS) {
    Value = RHS.Value;
    Move++;
    return *this;
  }
};
int CountCopyAndMove::Copy = 0;
int CountCopyAndMove::Move = 0;
} // anonymous namespace

namespace wpi::util {
// Specialization required to insert a CountCopyAndMove into a DenseSet.
template <> struct DenseMapInfo<CountCopyAndMove> {
  static unsigned getHashValue(const CountCopyAndMove &Val) {
    return Val.Value;
  }
  static bool isEqual(const CountCopyAndMove &LHS,
                      const CountCopyAndMove &RHS) {
    return LHS.Value == RHS.Value;
  }
};
}

namespace {
// Make sure reserve actually gives us enough buckets to insert N items
// without increasing allocation size.
TEST_CASE("DenseSetCustomTest ReserveTest", "[wpiutil][llvm]") {
  // Test a few different size, 48 is *not* a random choice: we need a value
  // that is 2/3 of a power of two to stress the grow() condition, and the power
  // of two has to be at least 64 because of minimum size allocation in the
  // DenseMa. 66 is a value just above the 64 default init.
  for (auto Size : {1, 2, 48, 66}) {
    DenseSet<CountCopyAndMove> Set;
    Set.reserve(Size);
    unsigned MemorySize = Set.getMemorySize();
    CountCopyAndMove::Copy = 0;
    CountCopyAndMove::Move = 0;
    for (int i = 0; i < Size; ++i)
      Set.insert(CountCopyAndMove(i));
    // Check that we didn't grow
    CHECK(MemorySize == Set.getMemorySize());
    // Check that move was called the expected number of times
    CHECK(Size == CountCopyAndMove::Move);
    // Check that no copy occurred
    CHECK(0 == CountCopyAndMove::Copy);
  }
}
TEST_CASE("DenseSetCustomTest ConstTest", "[wpiutil][llvm]") {
  // Test that const pointers work okay for count and find, even when the
  // underlying map is a non-const pointer.
  DenseSet<int *> Map;
  int A;
  int *B = &A;
  const int *C = &A;
  Map.insert(B);
  CHECK(Map.count(B) == 1u);
  CHECK(Map.count(C) == 1u);
  CHECK(Map.contains(B));
  CHECK(Map.contains(C));
}

#if LLVM_ENABLE_ABI_BREAKING_CHECKS
TEST_CASE("DenseSetCustomTest EraseInvalidatesIterators", "[wpiutil][llvm]") {
  DenseSet<int> Set;
  Set.insert(1);
  Set.insert(2);
  auto It = Set.find(1);
  Set.erase(2);
  CHECK_DEATH((void)*It, "invalid iterator access");
}
#endif
}
