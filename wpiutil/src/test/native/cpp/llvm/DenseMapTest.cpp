//===- llvm/unittest/ADT/DenseMapMap.cpp - DenseMap unit tests --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#include "wpi/util/DenseMap.hpp"
#include "CountCopyAndMove.hpp"
#include "wpi/util/DenseMapInfo.hpp"
#include "wpi/util/DenseMapInfoVariant.hpp"
#include "wpi/util/STLForwardCompat.hpp"
#include "wpi/util/SmallSet.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <map>
#include <optional>
#include <set>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

using namespace wpi::util;

namespace {
uint32_t getTestKey(int i, uint32_t *) { return i; }
uint32_t getTestValue(int i, uint32_t *) { return 42 + i; }

uint32_t *getTestKey(int i, uint32_t **) {
  static uint32_t dummy_arr1[8192];
  assert(i < 8192 && "Only support 8192 dummy keys.");
  return &dummy_arr1[i];
}
uint32_t *getTestValue(int i, uint32_t **) {
  static uint32_t dummy_arr1[8192];
  assert(i < 8192 && "Only support 8192 dummy keys.");
  return &dummy_arr1[i];
}

enum class EnumClass { Val };

EnumClass getTestKey(int i, EnumClass *) {
  // We can't possibly support 100 values for the swap test, so just return an
  // invalid EnumClass for testing.
  return static_cast<EnumClass>(i);
}

/// A test class that tries to check that construction and destruction
/// occur correctly.
class CtorTester {
  static std::set<CtorTester *> Constructed;
  int Value;

public:
  explicit CtorTester(int Value = 0) : Value(Value) {
    CHECK(Constructed.insert(this).second);
  }
  CtorTester(uint32_t Value) : Value(Value) {
    CHECK(Constructed.insert(this).second);
  }
  CtorTester(const CtorTester &Arg) : Value(Arg.Value) {
    CHECK(Constructed.insert(this).second);
  }
  CtorTester &operator=(const CtorTester &) = default;
  ~CtorTester() {
    CHECK(1u == Constructed.erase(this));
  }
  operator uint32_t() const { return Value; }

  int getValue() const { return Value; }
  bool operator==(const CtorTester &RHS) const { return Value == RHS.Value; }

  // Return the number of live CtorTester objects, excluding the empty and
  // tombstone keys.
  static size_t getNumConstructed() {
    return std::count_if(Constructed.begin(), Constructed.end(),
                         [](const CtorTester *Obj) {
                           int V = Obj->getValue();
                           return V != -1 && V != -2;
                         });
  }
};

std::set<CtorTester *> CtorTester::Constructed;

struct CtorTesterMapInfo {
  static inline CtorTester getEmptyKey() { return CtorTester(-1); }
  static inline CtorTester getTombstoneKey() { return CtorTester(-2); }
  static unsigned getHashValue(const CtorTester &Val) {
    return Val.getValue() * 37u;
  }
  static bool isEqual(const CtorTester &LHS, const CtorTester &RHS) {
    return LHS == RHS;
  }
};

CtorTester getTestKey(int i, CtorTester *) { return CtorTester(i); }
CtorTester getTestValue(int i, CtorTester *) { return CtorTester(42 + i); }

std::optional<uint32_t> getTestKey(int i, std::optional<uint32_t> *) {
  return i;
}

// Test fixture, with helper functions implemented by forwarding to global
// function overloads selected by component types of the type parameter. This
// allows all of the map implementations to be tested with shared
// implementations of helper routines.
template <typename T>
class DenseMapTest {
protected:
  T Map;

  static typename T::key_type *const dummy_key_ptr;
  static typename T::mapped_type *const dummy_value_ptr;

  typename T::key_type getKey(int i = 0) {
    return getTestKey(i, dummy_key_ptr);
  }
  typename T::mapped_type getValue(int i = 0) {
    return getTestValue(i, dummy_value_ptr);
  }
};

template <typename T>
typename T::key_type *const DenseMapTest<T>::dummy_key_ptr = nullptr;
template <typename T>
typename T::mapped_type *const DenseMapTest<T>::dummy_value_ptr = nullptr;

#define WPIUTIL_TEST_TYPES_DenseMapTest \
  (DenseMap<uint32_t, uint32_t>), (DenseMap<uint32_t*, uint32_t*>), \
      (DenseMap<CtorTester, CtorTester, CtorTesterMapInfo>), \
      (DenseMap<EnumClass, uint32_t>), \
      (DenseMap<std::optional<uint32_t>, uint32_t>), \
      (SmallDenseMap<uint32_t, uint32_t>), \
      (SmallDenseMap<uint32_t*, uint32_t*>), \
      (SmallDenseMap<CtorTester, CtorTester, 4, CtorTesterMapInfo>), \
      (SmallDenseMap<EnumClass, uint32_t>), \
      (SmallDenseMap<std::optional<uint32_t>, uint32_t>)

// Empty map tests
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest EmptyIntMapTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  // Size tests
  CHECK(0u == this->Map.size());
  CHECK(this-> Map.empty());

  // Iterator tests
  CHECK(this-> Map.begin() == this->Map.end());

  // Lookup tests
  CHECK_FALSE(this-> Map.count(this->getKey()));
  CHECK_FALSE(this-> Map.contains(this->getKey()));
  CHECK(this-> Map.find(this->getKey()) == this->Map.end());
  CHECK(typename TestType::mapped_type() == this->Map.lookup(this->getKey()));
}

// Constant map tests
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest ConstEmptyMapTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  const TestType &ConstMap = this->Map;
  CHECK(0u == ConstMap.size());
  CHECK(ConstMap.empty());
  CHECK(ConstMap.begin() == ConstMap.end());
}

// A map with a single entry
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest SingleEntryMapTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();

  // Size tests
  CHECK(1u == this->Map.size());
  CHECK_FALSE(this-> Map.begin() == this->Map.end());
  CHECK_FALSE(this-> Map.empty());

  // Iterator tests
  typename TestType::iterator it = this->Map.begin();
  CHECK(this-> getKey() == it->first);
  CHECK(this-> getValue() == it->second);
  ++it;
  CHECK(it == this->Map.end());

  // Lookup tests
  CHECK(this-> Map.count(this->getKey()));
  CHECK(this-> Map.contains(this->getKey()));
  CHECK(this-> Map.find(this->getKey()) == this->Map.begin());
  CHECK(this-> getValue() == this->Map.lookup(this->getKey()));
  CHECK(this-> getValue() == this->Map[this->getKey()]);
}

TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest AtTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey(0)] = this->getValue(0);
  this->Map[this->getKey(1)] = this->getValue(1);
  this->Map[this->getKey(2)] = this->getValue(2);
  CHECK(this-> getValue(0) == this->Map.at(this->getKey(0)));
  CHECK(this-> getValue(1) == this->Map.at(this->getKey(1)));
  CHECK(this-> getValue(2) == this->Map.at(this->getKey(2)));

  this->Map.at(this->getKey(0)) = this->getValue(1);
  CHECK(this-> getValue(1) == this->Map.at(this->getKey(0)));

  const auto &ConstMap = this->Map;
  CHECK(this-> getValue(1) == ConstMap.at(this->getKey(0)));
  CHECK(this-> getValue(1) == ConstMap.at(this->getKey(1)));
  CHECK(this-> getValue(2) == ConstMap.at(this->getKey(2)));
}

// Test clear() method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest ClearTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();
  this->Map.clear();

  CHECK(0u == this->Map.size());
  CHECK(this-> Map.empty());
  CHECK(this-> Map.begin() == this->Map.end());
}

// Test erase(iterator) method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest EraseTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();
  this->Map.erase(this->Map.begin());

  CHECK(0u == this->Map.size());
  CHECK(this-> Map.empty());
  CHECK(this-> Map.begin() == this->Map.end());
}

// Test erase(value) method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest EraseTest2", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();
  this->Map.erase(this->getKey());

  CHECK(0u == this->Map.size());
  CHECK(this-> Map.empty());
  CHECK(this-> Map.begin() == this->Map.end());
}

// Test insert() method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest InsertTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map.insert(std::make_pair(this->getKey(), this->getValue()));
  CHECK(1u == this->Map.size());
  CHECK(this-> getValue() == this->Map[this->getKey()]);
}

// Test copy constructor method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest CopyConstructorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();
  TestType copyMap(this->Map);

  CHECK(1u == copyMap.size());
  CHECK(this-> getValue() == copyMap[this->getKey()]);
}

// Test copy constructor method where SmallDenseMap isn't small.
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest CopyConstructorNotSmallTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  for (int Key = 0; Key < 5; ++Key)
    this->Map[this->getKey(Key)] = this->getValue(Key);
  TestType copyMap(this->Map);

  CHECK(5u == copyMap.size());
  for (int Key = 0; Key < 5; ++Key)
    CHECK(this-> getValue(Key) == copyMap[this->getKey(Key)]);
}

// Test range constructors.
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest RangeConstructorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  using KeyAndValue =
      std::pair<typename TestType::key_type, typename TestType::mapped_type>;
  KeyAndValue PlainArray[] = {{this->getKey(0), this->getValue(0)},
                              {this->getKey(1), this->getValue(1)}};

  TestType MapFromRange(wpi::util::from_range, PlainArray);
  CHECK(2u == MapFromRange.size());
  CHECK(this-> getValue(0) == MapFromRange[this->getKey(0)]);
  CHECK(this-> getValue(1) == MapFromRange[this->getKey(1)]);

  TestType MapFromInitList({{this->getKey(0), this->getValue(1)},
                             {this->getKey(1), this->getValue(2)}});
  CHECK(2u == MapFromInitList.size());
  CHECK(this-> getValue(1) == MapFromInitList[this->getKey(0)]);
  CHECK(this-> getValue(2) == MapFromInitList[this->getKey(1)]);
}

// Test copying from a default-constructed map.
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest CopyConstructorFromDefaultTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  TestType copyMap(this->Map);

  CHECK(copyMap.empty());
}

// Test copying from an empty map where SmallDenseMap isn't small.
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest CopyConstructorFromEmptyTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  for (int Key = 0; Key < 5; ++Key)
    this->Map[this->getKey(Key)] = this->getValue(Key);
  this->Map.clear();
  TestType copyMap(this->Map);

  CHECK(copyMap.empty());
}

// Test assignment operator method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest AssignmentTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();
  TestType copyMap = this->Map;

  CHECK(1u == copyMap.size());
  CHECK(this-> getValue() == copyMap[this->getKey()]);

  // test self-assignment.
  copyMap = static_cast<TestType &>(copyMap);
  CHECK(1u == copyMap.size());
  CHECK(this-> getValue() == copyMap[this->getKey()]);
}

TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest AssignmentTestNotSmall", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  for (int Key = 0; Key < 5; ++Key)
    this->Map[this->getKey(Key)] = this->getValue(Key);
  TestType copyMap = this->Map;

  CHECK(5u == copyMap.size());
  for (int Key = 0; Key < 5; ++Key)
    CHECK(this-> getValue(Key) == copyMap[this->getKey(Key)]);

  // test self-assignment.
  copyMap = static_cast<TestType &>(copyMap);
  CHECK(5u == copyMap.size());
  for (int Key = 0; Key < 5; ++Key)
    CHECK(this-> getValue(Key) == copyMap[this->getKey(Key)]);
}

// Test swap method
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest SwapTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  this->Map[this->getKey()] = this->getValue();
  TestType otherMap;

  this->Map.swap(otherMap);
  CHECK(0u == this->Map.size());
  CHECK(this-> Map.empty());
  CHECK(1u == otherMap.size());
  CHECK(this-> getValue() == otherMap[this->getKey()]);

  this->Map.swap(otherMap);
  CHECK(0u == otherMap.size());
  CHECK(otherMap.empty());
  CHECK(1u == this->Map.size());
  CHECK(this-> getValue() == this->Map[this->getKey()]);

  // Make this more interesting by inserting 100 numbers into the map.
  for (int i = 0; i < 100; ++i)
    this->Map[this->getKey(i)] = this->getValue(i);

  this->Map.swap(otherMap);
  CHECK(0u == this->Map.size());
  CHECK(this-> Map.empty());
  CHECK(100u == otherMap.size());
  for (int i = 0; i < 100; ++i)
    CHECK(this-> getValue(i) == otherMap[this->getKey(i)]);

  this->Map.swap(otherMap);
  CHECK(0u == otherMap.size());
  CHECK(otherMap.empty());
  CHECK(100u == this->Map.size());
  for (int i = 0; i < 100; ++i)
    CHECK(this-> getValue(i) == this->Map[this->getKey(i)]);
}

// A more complex iteration test
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest IterationTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  bool visited[100];
  std::map<typename TestType::key_type, unsigned> visitedIndex;

  // Insert 100 numbers into the map
  for (int i = 0; i < 100; ++i) {
    visited[i] = false;
    visitedIndex[this->getKey(i)] = i;

    this->Map[this->getKey(i)] = this->getValue(i);
  }

  // Iterate over all numbers and mark each one found.
  for (typename TestType::iterator it = this->Map.begin();
       it != this->Map.end(); ++it)
    visited[visitedIndex[it->first]] = true;

  // Ensure every number was visited.
  for (int i = 0; i < 100; ++i) {
    UNSCOPED_INFO("Entry #" << i << " was never visited");
    REQUIRE(visited[i]);
  }
}

// const_iterator test
TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest ConstIteratorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  // Check conversion from iterator to const_iterator.
  typename TestType::iterator it = this->Map.begin();
  typename TestType::const_iterator cit(it);
  CHECK(it == cit);

  // Check copying of const_iterators.
  typename TestType::const_iterator cit2(cit);
  CHECK(cit == cit2);
}

// TYPED_TEST below cycles through different types.  We define UniversalSmallSet
// here so that we'll use SmallSet or SmallPtrSet depending on whether the
// element type is a pointer.
template <typename T, unsigned N>
using UniversalSmallSet =
    std::conditional_t<std::is_pointer_v<T>, SmallPtrSet<T, N>, SmallSet<T, N>>;

TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest KeysValuesIterator", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  UniversalSmallSet<typename TestType::key_type, 10> Keys;
  UniversalSmallSet<typename TestType::mapped_type, 10> Values;
  for (int I = 0; I < 10; ++I) {
    auto K = this->getKey(I);
    auto V = this->getValue(I);
    Keys.insert(K);
    Values.insert(V);
    this->Map[K] = V;
  }

  UniversalSmallSet<typename TestType::key_type, 10> ActualKeys;
  UniversalSmallSet<typename TestType::mapped_type, 10> ActualValues;
  for (auto K : this->Map.keys())
    ActualKeys.insert(K);
  for (auto V : this->Map.values())
    ActualValues.insert(V);

  CHECK(Keys == ActualKeys);
  CHECK(Values == ActualValues);
}

TEMPLATE_TEST_CASE_METHOD(DenseMapTest, "DenseMapTest ConstKeysValuesIterator", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DenseMapTest) {
  UniversalSmallSet<typename TestType::key_type, 10> Keys;
  UniversalSmallSet<typename TestType::mapped_type, 10> Values;
  for (int I = 0; I < 10; ++I) {
    auto K = this->getKey(I);
    auto V = this->getValue(I);
    Keys.insert(K);
    Values.insert(V);
    this->Map[K] = V;
  }

  const TestType &ConstMap = this->Map;
  UniversalSmallSet<typename TestType::key_type, 10> ActualKeys;
  UniversalSmallSet<typename TestType::mapped_type, 10> ActualValues;
  for (auto K : ConstMap.keys())
    ActualKeys.insert(K);
  for (auto V : ConstMap.values())
    ActualValues.insert(V);

  CHECK(Keys == ActualKeys);
  CHECK(Values == ActualValues);
}

// Test initializer list construction.
TEST_CASE("DenseMapCustomTest InitializerList", "[wpiutil][llvm]") {
  DenseMap<int, int> M({{0, 0}, {0, 1}, {1, 2}});
  CHECK(2u == M.size());
  CHECK(1u == M.count(0));
  CHECK(0 == M[0]);
  CHECK(1u == M.count(1));
  CHECK(2 == M[1]);
}

// Test initializer list construction.
TEST_CASE("DenseMapCustomTest EqualityComparison", "[wpiutil][llvm]") {
  DenseMap<int, int> M1({{0, 0}, {1, 2}});
  DenseMap<int, int> M2({{0, 0}, {1, 2}});
  DenseMap<int, int> M3({{0, 0}, {1, 3}});

  CHECK(M1 == M2);
  CHECK(M1 != M3);
}

TEST_CASE("DenseMapCustomTest InsertRange", "[wpiutil][llvm]") {
  DenseMap<int, int> M;

  std::pair<int, int> InputVals[3] = {{0, 0}, {0, 1}, {1, 2}};
  M.insert_range(InputVals);

  CHECK(M.size() == 2u);
  CHECK_THAT(M, Catch::Matchers::UnorderedRangeEquals(
                    {std::pair{0, 0}, std::pair{1, 2}}));
}

TEST_CASE("SmallDenseMapCustomTest InsertRange", "[wpiutil][llvm]") {
  SmallDenseMap<int, int> M;

  std::pair<int, int> InputVals[3] = {{0, 0}, {0, 1}, {1, 2}};
  M.insert_range(InputVals);

  CHECK(M.size() == 2u);
  CHECK_THAT(M, Catch::Matchers::UnorderedRangeEquals(
                    {std::pair{0, 0}, std::pair{1, 2}}));
}

// Test for the default minimum size of a DenseMap
TEST_CASE("DenseMapCustomTest DefaultMinReservedSizeTest", "[wpiutil][llvm]") {
  // IF THIS VALUE CHANGE, please update InitialSizeTest, InitFromIterator, and
  // ReserveTest as well!
  const int ExpectedInitialBucketCount = 64;
  // Formula from DenseMap::getMinBucketToReserveForEntries()
  const int ExpectedMaxInitialEntries = ExpectedInitialBucketCount * 3 / 4 - 1;

  DenseMap<int, CountCopyAndMove> Map;
  // Will allocate 64 buckets
  Map.reserve(1);
  unsigned MemorySize = Map.getMemorySize();
  CountCopyAndMove::ResetCounts();

  for (int i = 0; i < ExpectedMaxInitialEntries; ++i)
    Map.insert(std::pair<int, CountCopyAndMove>(std::piecewise_construct,
                                                std::forward_as_tuple(i),
                                                std::forward_as_tuple()));
  // Check that we didn't grow
  CHECK(MemorySize == Map.getMemorySize());
  // Check that move was called the expected number of times
  CHECK(ExpectedMaxInitialEntries == CountCopyAndMove::TotalMoves());
  // Check that no copy occurred
  CHECK(0 == CountCopyAndMove::TotalCopies());

  // Adding one extra element should grow the map
  Map.insert(std::pair<int, CountCopyAndMove>(
      std::piecewise_construct,
      std::forward_as_tuple(ExpectedMaxInitialEntries),
      std::forward_as_tuple()));
  // Check that we grew
  CHECK(MemorySize != Map.getMemorySize());
  // Check that move was called the expected number of times
  //  This relies on move-construction elision, and cannot be reliably tested.
  //   CHECK(ExpectedMaxInitialEntries + 2 == CountCopyAndMove::Move);
  // Check that no copy occurred
  CHECK(0 == CountCopyAndMove::TotalCopies());
}

// Make sure creating the map with an initial size of N actually gives us enough
// buckets to insert N items without increasing allocation size.
TEST_CASE("DenseMapCustomTest InitialSizeTest", "[wpiutil][llvm]") {
  // Test a few different sizes, 48 is *not* a random choice: we need a value
  // that is 2/3 of a power of two to stress the grow() condition, and the power
  // of two has to be at least 64 because of minimum size allocation in the
  // DenseMap (see DefaultMinReservedSizeTest). 66 is a value just above the
  // 64 default init.
  for (auto Size : {1, 2, 48, 66}) {
    DenseMap<int, CountCopyAndMove> Map(Size);
    unsigned MemorySize = Map.getMemorySize();
    CountCopyAndMove::ResetCounts();

    for (int i = 0; i < Size; ++i)
      Map.insert(std::pair<int, CountCopyAndMove>(std::piecewise_construct,
                                                  std::forward_as_tuple(i),
                                                  std::forward_as_tuple()));
    // Check that we didn't grow
    CHECK(MemorySize == Map.getMemorySize());
    // Check that move was called the expected number of times
    CHECK(Size == CountCopyAndMove::TotalMoves());
    // Check that no copy occurred
    CHECK(0 == CountCopyAndMove::TotalCopies());
  }
}

// Make sure creating the map with a iterator range does not trigger grow()
TEST_CASE("DenseMapCustomTest InitFromIterator", "[wpiutil][llvm]") {
  std::vector<std::pair<int, CountCopyAndMove>> Values;
  // The size is a random value greater than 64 (hardcoded DenseMap min init)
  const int Count = 65;
  Values.reserve(Count);
  for (int i = 0; i < Count; i++)
    Values.emplace_back(i, CountCopyAndMove(i));

  CountCopyAndMove::ResetCounts();
  DenseMap<int, CountCopyAndMove> Map(Values.begin(), Values.end());
  // Check that no move occurred
  CHECK(0 == CountCopyAndMove::TotalMoves());
  // Check that copy was called the expected number of times
  CHECK(Count == CountCopyAndMove::TotalCopies());
}

// Make sure reserve actually gives us enough buckets to insert N items
// without increasing allocation size.
TEST_CASE("DenseMapCustomTest ReserveTest", "[wpiutil][llvm]") {
  // Test a few different size, 48 is *not* a random choice: we need a value
  // that is 2/3 of a power of two to stress the grow() condition, and the power
  // of two has to be at least 64 because of minimum size allocation in the
  // DenseMap (see DefaultMinReservedSizeTest). 66 is a value just above the
  // 64 default init.
  for (auto Size : {1, 2, 48, 66}) {
    DenseMap<int, CountCopyAndMove> Map;
    Map.reserve(Size);
    unsigned MemorySize = Map.getMemorySize();
    CountCopyAndMove::ResetCounts();
    for (int i = 0; i < Size; ++i)
      Map.insert(std::pair<int, CountCopyAndMove>(std::piecewise_construct,
                                                  std::forward_as_tuple(i),
                                                  std::forward_as_tuple()));
    // Check that we didn't grow
    CHECK(MemorySize == Map.getMemorySize());
    // Check that move was called the expected number of times
    CHECK(Size == CountCopyAndMove::TotalMoves());
    // Check that no copy occurred
    CHECK(0 == CountCopyAndMove::TotalCopies());
  }
}

TEST_CASE("DenseMapCustomTest InsertOrAssignTest", "[wpiutil][llvm]") {
  DenseMap<int, CountCopyAndMove> Map;

  CountCopyAndMove val1(1);
  CountCopyAndMove::ResetCounts();
  auto try0 = Map.insert_or_assign(0, val1);
  CHECK(try0.second);
  CHECK(0 == CountCopyAndMove::TotalMoves());
  CHECK(1 == CountCopyAndMove::CopyConstructions);
  CHECK(0 == CountCopyAndMove::CopyAssignments);

  CountCopyAndMove::ResetCounts();
  auto try1 = Map.insert_or_assign(0, val1);
  CHECK_FALSE(try1.second);
  CHECK(0 == CountCopyAndMove::TotalMoves());
  CHECK(0 == CountCopyAndMove::CopyConstructions);
  CHECK(1 == CountCopyAndMove::CopyAssignments);

  int key2 = 2;
  CountCopyAndMove val2(2);
  CountCopyAndMove::ResetCounts();
  auto try2 = Map.insert_or_assign(key2, std::move(val2));
  CHECK(try2.second);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(1 == CountCopyAndMove::MoveConstructions);
  CHECK(0 == CountCopyAndMove::MoveAssignments);

  CountCopyAndMove val3(3);
  CountCopyAndMove::ResetCounts();
  auto try3 = Map.insert_or_assign(key2, std::move(val3));
  CHECK_FALSE(try3.second);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(1 == CountCopyAndMove::MoveAssignments);
}

TEST_CASE("DenseMapCustomTest EmplaceOrAssign", "[wpiutil][llvm]") {
  DenseMap<int, CountCopyAndMove> Map;

  CountCopyAndMove::ResetCounts();
  auto Try0 = Map.emplace_or_assign(3, 3);
  CHECK(Try0.second);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::TotalMoves());
  CHECK(1 == CountCopyAndMove::ValueConstructions);

  CountCopyAndMove::ResetCounts();
  auto Try1 = Map.emplace_or_assign(3, 4);
  CHECK_FALSE(Try1.second);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(1 == CountCopyAndMove::ValueConstructions);
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(1 == CountCopyAndMove::MoveAssignments);

  int Key = 5;
  CountCopyAndMove::ResetCounts();
  auto Try2 = Map.emplace_or_assign(Key, 3);
  CHECK(Try2.second);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::TotalMoves());
  CHECK(1 == CountCopyAndMove::ValueConstructions);

  CountCopyAndMove::ResetCounts();
  auto Try3 = Map.emplace_or_assign(Key, 4);
  CHECK_FALSE(Try3.second);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(1 == CountCopyAndMove::ValueConstructions);
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(1 == CountCopyAndMove::MoveAssignments);
}

struct NonDefaultConstructible {
  unsigned V;
  NonDefaultConstructible(unsigned V) : V(V) {};
  bool operator==(const NonDefaultConstructible &Other) const {
    return V == Other.V;
  }
};

TEST_CASE("DenseMapCustomTest LookupOr", "[wpiutil][llvm]") {
  DenseMap<int, NonDefaultConstructible> M;

  M.insert_or_assign(0, 3u);
  M.insert_or_assign(1, 2u);
  M.insert_or_assign(1, 0u);

  CHECK(M.lookup_or(0, 4u) == 3u);
  CHECK(M.lookup_or(1, 4u) == 0u);
  CHECK(M.lookup_or(2, 4u) == 4u);
}

TEST_CASE("DenseMapCustomTest LookupOrConstness", "[wpiutil][llvm]") {
  DenseMap<int, unsigned *> M;
  unsigned Default = 3u;
  unsigned *Ret = M.lookup_or(0, &Default);
  CHECK(Ret == &Default);
}

// Key traits that allows lookup with either an unsigned or char* key;
// In the latter case, "a" == 0, "b" == 1 and so on.
struct TestDenseMapInfo {
  static inline unsigned getEmptyKey() { return ~0; }
  static inline unsigned getTombstoneKey() { return ~0U - 1; }
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

// find_as() tests
TEST_CASE("DenseMapCustomTest FindAsTest", "[wpiutil][llvm]") {
  DenseMap<unsigned, unsigned, TestDenseMapInfo> map;
  map[0] = 1;
  map[1] = 2;
  map[2] = 3;

  // Size tests
  CHECK(3u == map.size());

  // Normal lookup tests
  CHECK(1u == map.count(1));
  CHECK(1u == map.find(0)->second);
  CHECK(2u == map.find(1)->second);
  CHECK(3u == map.find(2)->second);
  CHECK(map.find(3) == map.end());

  // find_as() tests
  CHECK(1u == map.find_as("a")->second);
  CHECK(2u == map.find_as("b")->second);
  CHECK(3u == map.find_as("c")->second);
  CHECK(map.find_as("d") == map.end());
}

TEST_CASE("DenseMapCustomTest SmallDenseMapFromRange", "[wpiutil][llvm]") {
  std::pair<int, std::string_view> PlainArray[] = {{0, "0"}, {1, "1"}, {2, "2"}};
  SmallDenseMap<int, std::string_view> M(wpi::util::from_range, PlainArray);
  CHECK(3u == M.size());
  CHECK_THAT(
      M, Catch::Matchers::UnorderedRangeEquals(
             std::vector<std::pair<int, std::string_view>>{{0, "0"}, {1, "1"}, {2, "2"}}));
}

TEST_CASE("DenseMapCustomTest SmallDenseMapInitializerList", "[wpiutil][llvm]") {
  SmallDenseMap<int, int> M = {{0, 0}, {0, 1}, {1, 2}};
  CHECK(2u == M.size());
  CHECK(1u == M.count(0));
  CHECK(0 == M[0]);
  CHECK(1u == M.count(1));
  CHECK(2 == M[1]);
}

struct ContiguousDenseMapInfo {
  static inline unsigned getEmptyKey() { return ~0; }
  static inline unsigned getTombstoneKey() { return ~0U - 1; }
  static unsigned getHashValue(const unsigned& Val) { return Val; }
  static bool isEqual(const unsigned& LHS, const unsigned& RHS) {
    return LHS == RHS;
  }
};

// Test that filling a small dense map with exactly the number of elements in
// the map grows to have enough space for an empty bucket.
TEST_CASE("DenseMapCustomTest SmallDenseMapGrowTest", "[wpiutil][llvm]") {
  SmallDenseMap<unsigned, unsigned, 32, ContiguousDenseMapInfo> map;
  // Add some number of elements, then delete a few to leave us some tombstones.
  // If we just filled the map with 32 elements we'd grow because of not enough
  // tombstones which masks the issue here.
  for (unsigned i = 0; i < 20; ++i)
    map[i] = i + 1;
  for (unsigned i = 0; i < 10; ++i)
    map.erase(i);
  for (unsigned i = 20; i < 32; ++i)
    map[i] = i + 1;

  // Size tests
  CHECK(22u == map.size());

  // Try to find an element which doesn't exist.  There was a bug in
  // SmallDenseMap which led to a map with num elements == small capacity not
  // having an empty bucket any more.  Finding an element not in the map would
  // therefore never terminate.
  CHECK(map.find(32) == map.end());
}

TEST_CASE("DenseMapCustomTest LargeSmallDenseMapCompaction", "[wpiutil][llvm]") {
  SmallDenseMap<unsigned, unsigned, 128, ContiguousDenseMapInfo> map;
  // Fill to < 3/4 load.
  for (unsigned i = 0; i < 95; ++i)
    map[i] = i;
  // And erase, leaving behind tombstones.
  for (unsigned i = 0; i < 95; ++i)
    map.erase(i);
  // Fill further, so that less than 1/8 are empty, but still below 3/4 load.
  for (unsigned i = 95; i < 128; ++i)
    map[i] = i;

  CHECK(33u == map.size());
  // Similar to the previous test, check for a non-existing element, as an
  // indirect check that tombstones have been removed.
  CHECK(map.find(0) == map.end());
}

TEST_CASE("DenseMapCustomTest SmallDenseMapWithNumBucketsNonPowerOf2", "[wpiutil][llvm]") {
  // Is not power of 2.
  const unsigned NumInitBuckets = 33;
  // Power of 2 less then NumInitBuckets.
  constexpr unsigned InlineBuckets = 4;
  // Constructor should not trigger assert.
  SmallDenseMap<int, int, InlineBuckets> map(NumInitBuckets);
}

TEST_CASE("DenseMapCustomTest TryEmplaceTest", "[wpiutil][llvm]") {
  DenseMap<int, std::unique_ptr<int>> Map;
  std::unique_ptr<int> P(new int(2));
  auto Try1 = Map.try_emplace(0, new int(1));
  CHECK(Try1.second);
  auto Try2 = Map.try_emplace(0, std::move(P));
  CHECK_FALSE(Try2.second);
  CHECK(Try1.first == Try2.first);
  CHECK(nullptr != P);
}

TEST_CASE("DenseMapCustomTest ConstTest", "[wpiutil][llvm]") {
  // Test that const pointers work okay for count and find, even when the
  // underlying map is a non-const pointer.
  DenseMap<int *, int> Map;
  int A;
  int *B = &A;
  const int *C = &A;
  Map.insert({B, 0});
  CHECK(Map.count(B) == 1u);
  CHECK(Map.count(C) == 1u);
  CHECK(Map.find(B) != Map.end());
  CHECK(Map.find(C) != Map.end());
}

struct IncompleteStruct;

TEST_CASE("DenseMapCustomTest OpaquePointerKey", "[wpiutil][llvm]") {
  // Test that we can use a pointer to an incomplete type as a DenseMap key.
  // This is an important build time optimization, since many classes have
  // DenseMap members.
  DenseMap<IncompleteStruct *, int> Map;
  int Keys[3] = {0, 0, 0};
  IncompleteStruct *K1 = reinterpret_cast<IncompleteStruct *>(&Keys[0]);
  IncompleteStruct *K2 = reinterpret_cast<IncompleteStruct *>(&Keys[1]);
  IncompleteStruct *K3 = reinterpret_cast<IncompleteStruct *>(&Keys[2]);
  Map.insert({K1, 1});
  Map.insert({K2, 2});
  Map.insert({K3, 3});
  CHECK(Map.count(K1) == 1u);
  CHECK(Map[K1] == 1);
  CHECK(Map[K2] == 2);
  CHECK(Map[K3] == 3);
  Map.clear();
  CHECK(Map.find(K1) == Map.end());
  CHECK(Map.find(K2) == Map.end());
  CHECK(Map.find(K3) == Map.end());
}
} // namespace

namespace {
struct A {
  A(int value) : value(value) {}
  int value;
};
struct B : public A {
  using A::A;
};

struct AlwaysEqType {
  bool operator==(const AlwaysEqType &RHS) const { return true; }
};
} // namespace

namespace wpi::util {
template <typename T>
struct DenseMapInfo<T, std::enable_if_t<std::is_base_of_v<A, T>>> {
  static inline T getEmptyKey() { return {static_cast<int>(~0)}; }
  static inline T getTombstoneKey() { return {static_cast<int>(~0U - 1)}; }
  static unsigned getHashValue(const T &Val) { return Val.value; }
  static bool isEqual(const T &LHS, const T &RHS) {
    return LHS.value == RHS.value;
  }
};

template <> struct DenseMapInfo<AlwaysEqType> {
  using T = AlwaysEqType;
  static inline T getEmptyKey() { return {}; }
  static inline T getTombstoneKey() { return {}; }
  static unsigned getHashValue(const T &Val) { return 0; }
  static bool isEqual(const T &LHS, const T &RHS) {
    return false;
  }
};
} // namespace wpi::util

namespace {
TEST_CASE("DenseMapCustomTest SFINAEMapInfo", "[wpiutil][llvm]") {
  // Test that we can use a pointer to an incomplete type as a DenseMap key.
  // This is an important build time optimization, since many classes have
  // DenseMap members.
  DenseMap<B, int> Map;
  B Keys[3] = {{0}, {1}, {2}};
  Map.insert({Keys[0], 1});
  Map.insert({Keys[1], 2});
  Map.insert({Keys[2], 3});
  CHECK(Map.count(Keys[0]) == 1u);
  CHECK(Map[Keys[0]] == 1);
  CHECK(Map[Keys[1]] == 2);
  CHECK(Map[Keys[2]] == 3);
  Map.clear();
  CHECK(Map.find(Keys[0]) == Map.end());
  CHECK(Map.find(Keys[1]) == Map.end());
  CHECK(Map.find(Keys[2]) == Map.end());
}

TEST_CASE("DenseMapCustomTest VariantSupport", "[wpiutil][llvm]") {
  using variant = std::variant<int, int, AlwaysEqType>;
  DenseMap<variant, int> Map;
  variant Keys[] = {
      variant(std::in_place_index<0>, 1),
      variant(std::in_place_index<1>, 1),
      variant(std::in_place_index<2>),
  };
  Map.try_emplace(Keys[0], 0);
  Map.try_emplace(Keys[1], 1);
  CHECK(Map.size() == 2u);
  CHECK(DenseMapInfo<variant>::getHashValue(Keys[0]) != DenseMapInfo<variant>::getHashValue(Keys[1]));
  // Check that isEqual dispatches to isEqual of underlying type, and not to
  // operator==.
  CHECK_FALSE(DenseMapInfo < variant>::isEqual(Keys[2], Keys[2]));
}

TEST_CASE("DenseMapCustomTest InitSize", "[wpiutil][llvm]") {
  constexpr unsigned ElemSize = sizeof(std::pair<int *, int>);

  {
    DenseMap<int *, int> Map;
    CHECK(ElemSize * 0U == Map.getMemorySize());
  }
  {
    DenseMap<int *, int> Map(0);
    CHECK(ElemSize * 0U == Map.getMemorySize());
  }
  {
    DenseMap<int *, int> Map(1);
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    DenseMap<int *, int> Map(2);
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    DenseMap<int *, int> Map(3);
    CHECK(ElemSize * 8U == Map.getMemorySize());
  }
  {
    int A, B;
    DenseMap<int *, int> Map = {{&A, 1}, {&B, 2}};
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    int A, B, C;
    DenseMap<int *, int> Map = {{&A, 1}, {&B, 2}, {&C, 3}};
    CHECK(ElemSize * 8U == Map.getMemorySize());
  }
}

TEST_CASE("SmallDenseMapCustomTest InitSize", "[wpiutil][llvm]") {
  constexpr unsigned ElemSize = sizeof(std::pair<int *, int>);
  {
    SmallDenseMap<int *, int> Map;
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    SmallDenseMap<int *, int> Map(0);
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    SmallDenseMap<int *, int> Map(1);
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    SmallDenseMap<int *, int> Map(2);
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    SmallDenseMap<int *, int> Map(3);
    CHECK(ElemSize * 8U == Map.getMemorySize());
  }
  {
    int A, B;
    SmallDenseMap<int *, int> Map = {{&A, 1}, {&B, 2}};
    CHECK(ElemSize * 4U == Map.getMemorySize());
  }
  {
    int A, B, C;
    SmallDenseMap<int *, int> Map = {{&A, 1}, {&B, 2}, {&C, 3}};
    CHECK(ElemSize * 8U == Map.getMemorySize());
  }
}

TEST_CASE("DenseMapCustomTest KeyDtor", "[wpiutil][llvm]") {
  // This test relies on CtorTester being non-trivially destructible.
  static_assert(!std::is_trivially_destructible_v<CtorTester>,
                "CtorTester must not be trivially destructible");

  // Test that keys are destructed on scope exit.
  CHECK(0u == CtorTester::getNumConstructed());
  {
    DenseMap<CtorTester, int, CtorTesterMapInfo> Map;
    Map.try_emplace(CtorTester(0), 1);
    Map.try_emplace(CtorTester(1), 2);
    CHECK(2u == CtorTester::getNumConstructed());
  }
  CHECK(0u == CtorTester::getNumConstructed());

  // Test that keys are destructed on erase and shrink_and_clear.
  CHECK(0u == CtorTester::getNumConstructed());
  {
    DenseMap<CtorTester, int, CtorTesterMapInfo> Map;
    Map.try_emplace(CtorTester(0), 1);
    Map.try_emplace(CtorTester(1), 2);
    CHECK(2u == CtorTester::getNumConstructed());
    Map.erase(CtorTester(1));
    CHECK(1u == CtorTester::getNumConstructed());
    Map.shrink_and_clear();
    CHECK(0u == CtorTester::getNumConstructed());
  }
  CHECK(0u == CtorTester::getNumConstructed());
}

TEST_CASE("DenseMapCustomTest ValueDtor", "[wpiutil][llvm]") {
  // This test relies on CtorTester being non-trivially destructible.
  static_assert(!std::is_trivially_destructible_v<CtorTester>,
                "CtorTester must not be trivially destructible");

  // Test that values are destructed on scope exit.
  CHECK(0u == CtorTester::getNumConstructed());
  {
    DenseMap<int, CtorTester> Map;
    Map.try_emplace(0, CtorTester(1));
    Map.try_emplace(1, CtorTester(2));
    CHECK(2u == CtorTester::getNumConstructed());
  }
  CHECK(0u == CtorTester::getNumConstructed());

  // Test that values are destructed on erase and shrink_and_clear.
  CHECK(0u == CtorTester::getNumConstructed());
  {
    DenseMap<int, CtorTester> Map;
    Map.try_emplace(0, CtorTester(1));
    Map.try_emplace(1, CtorTester(2));
    CHECK(2u == CtorTester::getNumConstructed());
    Map.erase(1);
    CHECK(1u == CtorTester::getNumConstructed());
    Map.shrink_and_clear();
    CHECK(0u == CtorTester::getNumConstructed());
  }
  CHECK(0u == CtorTester::getNumConstructed());
}

} // namespace
