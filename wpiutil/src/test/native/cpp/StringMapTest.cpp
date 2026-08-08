// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

//===- llvm/unittest/ADT/StringMapMap.cpp - StringMap unit tests ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/StringMap.hpp"

#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using namespace wpi::util;

namespace {

// Test fixture
class StringMapTest {
 protected:
  StringMap<uint32_t> testMap;

  static const char testKey[];
  static const uint32_t testValue;
  static const char* testKeyFirst;
  static size_t testKeyLength;
  static const std::string testKeyStr;

  void assertEmptyMap() {
    // Size tests
    CHECK(0u == testMap.size());
    CHECK(testMap.empty());

    // Iterator tests
    CHECK(testMap.begin() == testMap.end());

    // Lookup tests
    CHECK_FALSE(testMap.contains(testKey));
    CHECK(0u == testMap.count(testKey));
    CHECK(0u == testMap.count(std::string_view(testKeyFirst, testKeyLength)));
    CHECK(0u == testMap.count(testKeyStr));
    CHECK(testMap.find(testKey) == testMap.end());
    CHECK(testMap.find(std::string_view(testKeyFirst, testKeyLength)) ==
          testMap.end());
    CHECK(testMap.find(testKeyStr) == testMap.end());
  }

  void assertSingleItemMap() {
    // Size tests
    CHECK(1u == testMap.size());
    CHECK_FALSE(testMap.begin() == testMap.end());
    CHECK_FALSE(testMap.empty());

    // Iterator tests
    StringMap<uint32_t>::iterator it = testMap.begin();
    CHECK(std::string_view{testKey} == std::string_view{it->first.data()});
    CHECK(testValue == it->second);
    ++it;
    CHECK(it == testMap.end());

    // Lookup tests
    CHECK(testMap.contains(testKey));
    CHECK(1u == testMap.count(testKey));
    CHECK(1u == testMap.count(std::string_view(testKeyFirst, testKeyLength)));
    CHECK(1u == testMap.count(testKeyStr));
    CHECK(testMap.find(testKey) == testMap.begin());
    CHECK(testMap.find(std::string_view(testKeyFirst, testKeyLength)) ==
          testMap.begin());
    CHECK(testMap.find(testKeyStr) == testMap.begin());
  }
};

const char StringMapTest::testKey[] = "key";
const uint32_t StringMapTest::testValue = 1u;
const char* StringMapTest::testKeyFirst = testKey;
size_t StringMapTest::testKeyLength = sizeof(testKey) - 1;
const std::string StringMapTest::testKeyStr(testKey);

struct CountCopyAndMove {
  CountCopyAndMove() = default;
  CountCopyAndMove(const CountCopyAndMove&) { copy = 1; }
  CountCopyAndMove(CountCopyAndMove&&) { move = 1; }
  void operator=(const CountCopyAndMove&) { ++copy; }
  void operator=(CountCopyAndMove&&) { ++move; }
  int copy = 0;
  int move = 0;
};

// Empty map tests.
TEST_CASE_METHOD(StringMapTest, "StringMapTest EmptyMap", "[wpiutil]") {
  assertEmptyMap();
}

// Constant map tests.
TEST_CASE_METHOD(StringMapTest, "StringMapTest ConstEmptyMap", "[wpiutil]") {
  const StringMap<uint32_t>& constTestMap = testMap;

  // Size tests
  CHECK(0u == constTestMap.size());
  CHECK(constTestMap.empty());

  // Iterator tests
  CHECK(constTestMap.begin() == constTestMap.end());

  // Lookup tests
  CHECK(0u == constTestMap.count(testKey));
  CHECK(0u ==
        constTestMap.count(std::string_view(testKeyFirst, testKeyLength)));
  CHECK(0u == constTestMap.count(testKeyStr));
  CHECK(constTestMap.find(testKey) == constTestMap.end());
  CHECK(constTestMap.find(std::string_view(testKeyFirst, testKeyLength)) ==
        constTestMap.end());
  CHECK(constTestMap.find(testKeyStr) == constTestMap.end());
}

// initializer_list ctor test; also implicitly tests initializer_list and
// iterator overloads of insert().
TEST_CASE_METHOD(StringMapTest, "StringMapTest InitializerListCtor",
                 "[wpiutil]") {
  testMap = StringMap<uint32_t>({{"key", 1}});
  assertSingleItemMap();
}

// A map with a single entry.
TEST_CASE_METHOD(StringMapTest, "StringMapTest SingleEntryMap", "[wpiutil]") {
  testMap[testKey] = testValue;
  assertSingleItemMap();
}

// Test clear() method.
TEST_CASE_METHOD(StringMapTest, "StringMapTest Clear", "[wpiutil]") {
  testMap[testKey] = testValue;
  testMap.clear();
  assertEmptyMap();
}

// Test erase(iterator) method.
TEST_CASE_METHOD(StringMapTest, "StringMapTest EraseIterator", "[wpiutil]") {
  testMap[testKey] = testValue;
  testMap.erase(testMap.begin());
  assertEmptyMap();
}

// Test erase(value) method.
TEST_CASE_METHOD(StringMapTest, "StringMapTest EraseValue", "[wpiutil]") {
  testMap[testKey] = testValue;
  testMap.erase(testKey);
  assertEmptyMap();
}

// Test inserting two values and erasing one.
TEST_CASE_METHOD(StringMapTest, "StringMapTest InsertAndErase", "[wpiutil]") {
  testMap[testKey] = testValue;
  testMap["otherKey"] = 2;
  testMap.erase("otherKey");
  assertSingleItemMap();
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest SmallFullMap", "[wpiutil]") {
  wpi::util::StringMap<int> Map;

  Map["eins"] = 1;
  Map["zwei"] = 2;
  Map["drei"] = 3;
  Map.erase("drei");
  Map.erase("eins");
  Map["veir"] = 4;
  Map["funf"] = 5;

  CHECK(3u == Map.size());
  CHECK_FALSE(Map.contains("eins"));
  CHECK(2 == Map["zwei"]);
  CHECK_FALSE(Map.contains("drei"));
  CHECK(4 == Map["veir"]);
  CHECK(5 == Map["funf"]);
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest CopyCtor", "[wpiutil]") {
  wpi::util::StringMap<int> Map;

  Map["eins"] = 1;
  Map["zwei"] = 2;
  Map["drei"] = 3;
  Map.erase("drei");
  Map.erase("eins");
  Map["veir"] = 4;
  Map["funf"] = 5;

  CHECK(3u == Map.size());
  CHECK_FALSE(Map.contains("eins"));
  CHECK(2 == Map["zwei"]);
  CHECK_FALSE(Map.contains("drei"));
  CHECK(4 == Map["veir"]);
  CHECK(5 == Map["funf"]);

  wpi::util::StringMap<int> Map2(Map);
  CHECK(3u == Map2.size());
  CHECK_FALSE(Map2.contains("eins"));
  CHECK(2 == Map2["zwei"]);
  CHECK_FALSE(Map2.contains("drei"));
  CHECK(4 == Map2["veir"]);
  CHECK(5 == Map2["funf"]);
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest At", "[wpiutil]") {
  wpi::util::StringMap<int> Map;

  // keys both found and not found on non-empty map
  Map["a"] = 1;
  Map["b"] = 2;
  Map["c"] = 3;
  CHECK(1 == Map.at("a"));
  CHECK(2 == Map.at("b"));
  CHECK(3 == Map.at("c"));
}

// A more complex iteration test.
TEST_CASE_METHOD(StringMapTest, "StringMapTest Iteration", "[wpiutil]") {
  bool visited[100];

  // Insert 100 numbers into the map
  for (int i = 0; i < 100; ++i) {
    std::stringstream ss;
    ss << "key_" << i;
    testMap[ss.str()] = i;
    visited[i] = false;
  }

  // Iterate over all numbers and mark each one found.
  for (StringMap<uint32_t>::iterator it = testMap.begin(); it != testMap.end();
       ++it) {
    std::stringstream ss;
    ss << "key_" << it->second;
    REQUIRE(std::string_view{ss.str().c_str()} ==
            std::string_view{it->first.data()});
    visited[it->second] = true;
  }

  // Ensure every number was visited.
  for (int i = 0; i < 100; ++i) {
    UNSCOPED_INFO("Entry #" << i << " was never visited");
    REQUIRE(visited[i]);
  }
}

// Test insert() method.
TEST_CASE_METHOD(StringMapTest, "StringMapTest Insert", "[wpiutil]") {
  UNSCOPED_INFO("InsertTest");
  testMap.insert(std::pair{std::string_view(testKeyFirst, testKeyLength), 1u});
  assertSingleItemMap();
}

// Test insert(pair<K, V>) method
TEST_CASE_METHOD(StringMapTest, "StringMapTest InsertPair", "[wpiutil]") {
  bool Inserted;
  StringMap<uint32_t>::iterator NewIt;
  std::tie(NewIt, Inserted) =
      testMap.insert(std::pair{testKeyFirst, testValue});
  CHECK(1u == testMap.size());
  CHECK(testValue == testMap[testKeyFirst]);
  CHECK(testKeyFirst == NewIt->first);
  CHECK(testValue == NewIt->second);
  CHECK(Inserted);

  StringMap<uint32_t>::iterator ExistingIt;
  std::tie(ExistingIt, Inserted) =
      testMap.insert(std::pair{testKeyFirst, testValue + 1});
  CHECK(1u == testMap.size());
  CHECK(testValue == testMap[testKeyFirst]);
  CHECK_FALSE(Inserted);
  CHECK(NewIt == ExistingIt);
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest InsertOrAssign", "[wpiutil]") {
  struct A : CountCopyAndMove {
    explicit A(int v) : v(v) {}
    int v;
  };
  StringMap<A> t;

  auto try1 = t.insert_or_assign("A", A(1));
  CHECK(try1.second);
  CHECK(1 == try1.first->second.v);
  CHECK(1 == try1.first->second.move);

  auto try2 = t.insert_or_assign("A", A(2));
  CHECK_FALSE(try2.second);
  CHECK(2 == try2.first->second.v);
  CHECK(2 == try1.first->second.move);

  CHECK(try1.first == try2.first);
  CHECK(0 == try1.first->second.copy);
}

// Create a non-default constructable value
struct StringMapTestStruct {
  explicit StringMapTestStruct(int i) : i(i) {}
  StringMapTestStruct() = delete;
  int i;
};

TEST_CASE_METHOD(StringMapTest, "StringMapTest NonDefaultConstructable",
                 "[wpiutil]") {
  StringMap<StringMapTestStruct> t;
  t.insert(std::pair{"Test", StringMapTestStruct(123)});
  StringMap<StringMapTestStruct>::iterator iter = t.find("Test");
  REQUIRE(iter != t.end());
  REQUIRE(iter->second.i == 123);
}

struct Immovable {
  Immovable() {}
  Immovable(Immovable&&) = delete;  // will disable the other special members
};

struct MoveOnly {
  int i;
  explicit MoveOnly(int i) : i(i) {}
  explicit MoveOnly(const Immovable&) : i(0) {}
  MoveOnly(MoveOnly&& RHS) : i(RHS.i) {}
  MoveOnly& operator=(MoveOnly&& RHS) {
    i = RHS.i;
    return *this;
  }

 private:
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;
};

TEST_CASE_METHOD(StringMapTest, "StringMapTest MoveConstruct", "[wpiutil]") {
  StringMap<int> A;
  A["x"] = 42;
  StringMap<int> B = std::move(A);
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
  REQUIRE(A.size() == 0u);
  REQUIRE(B.size() == 1u);
  REQUIRE(B["x"] == 42);
  REQUIRE(B.count("y") == 0u);
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest MoveAssignment", "[wpiutil]") {
  StringMap<int> A;
  A["x"] = 42;
  StringMap<int> B;
  B["y"] = 117;
  A = std::move(B);
  REQUIRE(A.size() == 1u);
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
  REQUIRE(B.size() == 0u);
  REQUIRE(A["y"] == 117);
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
  REQUIRE(B.count("x") == 0u);
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest EqualEmpty", "[wpiutil]") {
  StringMap<int> A;
  StringMap<int> B;
  REQUIRE(A == B);
  REQUIRE_FALSE(A != B);
  REQUIRE(A == A);  // self check
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest EqualWithValues", "[wpiutil]") {
  StringMap<int> A;
  A["A"] = 1;
  A["B"] = 2;
  A["C"] = 3;
  A["D"] = 3;

  StringMap<int> B;
  B["A"] = 1;
  B["B"] = 2;
  B["C"] = 3;
  B["D"] = 3;

  REQUIRE(A == B);
  REQUIRE(B == A);
  REQUIRE_FALSE(A != B);
  REQUIRE_FALSE(B != A);
  REQUIRE(A == A);  // self check
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest NotEqualMissingKeys",
                 "[wpiutil]") {
  StringMap<int> A;
  A["A"] = 1;
  A["B"] = 2;

  StringMap<int> B;
  B["A"] = 1;
  B["B"] = 2;
  B["C"] = 3;
  B["D"] = 3;

  REQUIRE_FALSE(A == B);
  REQUIRE_FALSE(B == A);
  REQUIRE(A != B);
  REQUIRE(B != A);
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest NotEqualWithDifferentValues",
                 "[wpiutil]") {
  StringMap<int> A;
  A["A"] = 1;
  A["B"] = 2;
  A["C"] = 100;
  A["D"] = 3;

  StringMap<int> B;
  B["A"] = 1;
  B["B"] = 2;
  B["C"] = 3;
  B["D"] = 3;

  REQUIRE_FALSE(A == B);
  REQUIRE_FALSE(B == A);
  REQUIRE(A != B);
  REQUIRE(B != A);
}

struct Countable {
  int& InstanceCount;
  int Number;
  Countable(int Number, int& InstanceCount)
      : InstanceCount(InstanceCount), Number(Number) {
    ++InstanceCount;
  }
  Countable(Countable&& C) : InstanceCount(C.InstanceCount), Number(C.Number) {
    ++InstanceCount;
    C.Number = -1;
  }
  Countable(const Countable& C)
      : InstanceCount(C.InstanceCount), Number(C.Number) {
    ++InstanceCount;
  }
  Countable& operator=(Countable C) {
    Number = C.Number;
    return *this;
  }
  ~Countable() { --InstanceCount; }
};

TEST_CASE_METHOD(StringMapTest, "StringMapTest MoveDtor", "[wpiutil]") {
  int InstanceCount = 0;
  StringMap<Countable> A;
  A.insert(std::pair{"x", Countable(42, InstanceCount)});
  REQUIRE(InstanceCount == 1);
  auto I = A.find("x");
  REQUIRE(I != A.end());
  REQUIRE(I->second.Number == 42);

  StringMap<Countable> B;
  B = std::move(A);
  REQUIRE(InstanceCount == 1);
  REQUIRE(A.empty());
  I = B.find("x");
  REQUIRE(I != B.end());
  REQUIRE(I->second.Number == 42);

  B = StringMap<Countable>();
  REQUIRE(InstanceCount == 0);
  REQUIRE(B.empty());
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest StructuredBindings",
                 "[wpiutil]") {
  StringMap<int> A;
  A["a"] = 42;

  for (auto& [Key, Value] : A) {
    CHECK("a" == Key);
    CHECK(42 == Value);
  }
}

TEST_CASE_METHOD(StringMapTest, "StringMapTest StructuredBindingsMoveOnly",
                 "[wpiutil]") {
  StringMap<MoveOnly> A;
  A.insert(std::pair{"a", MoveOnly(42)});

  for (auto&& [Key, Value] : A) {
    CHECK("a" == Key);
    CHECK(42 == Value.i);
  }
}

namespace {
// Simple class that counts how many moves and copy happens when growing a map
struct CountCtorCopyAndMove {
  static unsigned Ctor;
  static unsigned Move;
  static unsigned Copy;
  int Data = 0;
  explicit CountCtorCopyAndMove(int Data) : Data(Data) { Ctor++; }
  CountCtorCopyAndMove() { Ctor++; }

  CountCtorCopyAndMove(const CountCtorCopyAndMove&) { Copy++; }
  CountCtorCopyAndMove& operator=(const CountCtorCopyAndMove&) {
    Copy++;
    return *this;
  }
  CountCtorCopyAndMove(CountCtorCopyAndMove&&) { Move++; }
  CountCtorCopyAndMove& operator=(const CountCtorCopyAndMove&&) {
    Move++;
    return *this;
  }
};
unsigned CountCtorCopyAndMove::Copy = 0;
unsigned CountCtorCopyAndMove::Move = 0;
unsigned CountCtorCopyAndMove::Ctor = 0;

}  // namespace

TEST_CASE("StringMapCustomTest BracketOperatorCtor", "[wpiutil]") {
  StringMap<CountCtorCopyAndMove> Map;
  CountCtorCopyAndMove::Ctor = 0;
  Map["abcd"];
  CHECK(1u == CountCtorCopyAndMove::Ctor);
  // Test that operator[] does not create a value when it is already in the map
  CountCtorCopyAndMove::Ctor = 0;
  Map["abcd"];
  CHECK(0u == CountCtorCopyAndMove::Ctor);
}

namespace {
struct NonMoveableNonCopyableType {
  int Data = 0;
  NonMoveableNonCopyableType() = default;
  explicit NonMoveableNonCopyableType(int Data) : Data(Data) {}
  NonMoveableNonCopyableType(const NonMoveableNonCopyableType&) = delete;
  NonMoveableNonCopyableType(NonMoveableNonCopyableType&&) = delete;
};
}  // namespace

// Test that we can "emplace" an element in the map without involving map/move
TEST_CASE("StringMapCustomTest Emplace", "[wpiutil]") {
  StringMap<NonMoveableNonCopyableType> Map;
  Map.try_emplace("abcd", 42);
  CHECK(1u == Map.count("abcd"));
  CHECK(42 == Map["abcd"].Data);
}

}  // namespace
