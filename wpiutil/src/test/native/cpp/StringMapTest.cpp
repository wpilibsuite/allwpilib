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

#include "wpi/StringMap.h"  // NOLINT(build/include_order)

#include <string>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

using namespace wpi;

namespace {

// Test fixture
class StringMapTest : public testing::Test {
 protected:
  StringMap<uint32_t> testMap;

  static const char testKey[];
  static const uint32_t testValue;
  static const char* testKeyFirst;
  static size_t testKeyLength;
  static const std::string testKeyStr;

  void assertEmptyMap() {
    // Size tests
    EXPECT_EQ(0u, testMap.size());
    EXPECT_TRUE(testMap.empty());

    // Iterator tests
    EXPECT_TRUE(testMap.begin() == testMap.end());

    // Lookup tests
    EXPECT_FALSE(testMap.contains(testKey));
    EXPECT_EQ(0u, testMap.count(testKey));
    EXPECT_EQ(0u, testMap.count(std::string_view(testKeyFirst, testKeyLength)));
    EXPECT_EQ(0u, testMap.count(testKeyStr));
    EXPECT_TRUE(testMap.find(testKey) == testMap.end());
    EXPECT_TRUE(testMap.find(std::string_view(testKeyFirst, testKeyLength)) ==
                testMap.end());
    EXPECT_TRUE(testMap.find(testKeyStr) == testMap.end());
  }

  void assertSingleItemMap() {
    // Size tests
    EXPECT_EQ(1u, testMap.size());
    EXPECT_FALSE(testMap.begin() == testMap.end());
    EXPECT_FALSE(testMap.empty());

    // Iterator tests
    StringMap<uint32_t>::iterator it = testMap.begin();
    EXPECT_STREQ(testKey, it->first.data());
    EXPECT_EQ(testValue, it->second);
    ++it;
    EXPECT_TRUE(it == testMap.end());

    // Lookup tests
    EXPECT_TRUE(testMap.contains(testKey));
    EXPECT_EQ(1u, testMap.count(testKey));
    EXPECT_EQ(1u, testMap.count(std::string_view(testKeyFirst, testKeyLength)));
    EXPECT_EQ(1u, testMap.count(testKeyStr));
    EXPECT_TRUE(testMap.find(testKey) == testMap.begin());
    EXPECT_TRUE(testMap.find(std::string_view(testKeyFirst, testKeyLength)) ==
                testMap.begin());
    EXPECT_TRUE(testMap.find(testKeyStr) == testMap.begin());
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
TEST_F(StringMapTest, EmptyMap) {
  assertEmptyMap();
}

// Constant map tests.
TEST_F(StringMapTest, ConstEmptyMap) {
  const StringMap<uint32_t>& constTestMap = testMap;

  // Size tests
  EXPECT_EQ(0u, constTestMap.size());
  EXPECT_TRUE(constTestMap.empty());

  // Iterator tests
  EXPECT_TRUE(constTestMap.begin() == constTestMap.end());

  // Lookup tests
  EXPECT_EQ(0u, constTestMap.count(testKey));
  EXPECT_EQ(0u,
            constTestMap.count(std::string_view(testKeyFirst, testKeyLength)));
  EXPECT_EQ(0u, constTestMap.count(testKeyStr));
  EXPECT_TRUE(constTestMap.find(testKey) == constTestMap.end());
  EXPECT_TRUE(constTestMap.find(std::string_view(
                  testKeyFirst, testKeyLength)) == constTestMap.end());
  EXPECT_TRUE(constTestMap.find(testKeyStr) == constTestMap.end());
}

// initializer_list ctor test; also implicitly tests initializer_list and
// iterator overloads of insert().
TEST_F(StringMapTest, InitializerListCtor) {
  testMap = StringMap<uint32_t>({{"key", 1}});
  assertSingleItemMap();
}

// A map with a single entry.
TEST_F(StringMapTest, SingleEntryMap) {
  testMap[testKey] = testValue;
  assertSingleItemMap();
}

// Test clear() method.
TEST_F(StringMapTest, Clear) {
  testMap[testKey] = testValue;
  testMap.clear();
  assertEmptyMap();
}

// Test erase(iterator) method.
TEST_F(StringMapTest, EraseIterator) {
  testMap[testKey] = testValue;
  testMap.erase(testMap.begin());
  assertEmptyMap();
}

// Test erase(value) method.
TEST_F(StringMapTest, EraseValue) {
  testMap[testKey] = testValue;
  testMap.erase(testKey);
  assertEmptyMap();
}

// Test inserting two values and erasing one.
TEST_F(StringMapTest, InsertAndErase) {
  testMap[testKey] = testValue;
  testMap["otherKey"] = 2;
  testMap.erase("otherKey");
  assertSingleItemMap();
}

TEST_F(StringMapTest, SmallFullMap) {
  wpi::StringMap<int> Map;

  Map["eins"] = 1;
  Map["zwei"] = 2;
  Map["drei"] = 3;
  Map.erase("drei");
  Map.erase("eins");
  Map["veir"] = 4;
  Map["funf"] = 5;

  EXPECT_EQ(3u, Map.size());
  EXPECT_FALSE(Map.contains("eins"));
  EXPECT_EQ(2, Map["zwei"]);
  EXPECT_FALSE(Map.contains("drei"));
  EXPECT_EQ(4, Map["veir"]);
  EXPECT_EQ(5, Map["funf"]);
}

TEST_F(StringMapTest, CopyCtor) {
  wpi::StringMap<int> Map;

  Map["eins"] = 1;
  Map["zwei"] = 2;
  Map["drei"] = 3;
  Map.erase("drei");
  Map.erase("eins");
  Map["veir"] = 4;
  Map["funf"] = 5;

  EXPECT_EQ(3u, Map.size());
  EXPECT_FALSE(Map.contains("eins"));
  EXPECT_EQ(2, Map["zwei"]);
  EXPECT_FALSE(Map.contains("drei"));
  EXPECT_EQ(4, Map["veir"]);
  EXPECT_EQ(5, Map["funf"]);

  wpi::StringMap<int> Map2(Map);
  EXPECT_EQ(3u, Map2.size());
  EXPECT_FALSE(Map2.contains("eins"));
  EXPECT_EQ(2, Map2["zwei"]);
  EXPECT_FALSE(Map2.contains("drei"));
  EXPECT_EQ(4, Map2["veir"]);
  EXPECT_EQ(5, Map2["funf"]);
}

TEST_F(StringMapTest, At) {
  wpi::StringMap<int> Map;

  // keys both found and not found on non-empty map
  Map["a"] = 1;
  Map["b"] = 2;
  Map["c"] = 3;
  EXPECT_EQ(1, Map.at("a"));
  EXPECT_EQ(2, Map.at("b"));
  EXPECT_EQ(3, Map.at("c"));
}

// A more complex iteration test.
TEST_F(StringMapTest, Iteration) {
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
    ASSERT_STREQ(ss.str().c_str(), it->first.data());
    visited[it->second] = true;
  }

  // Ensure every number was visited.
  for (int i = 0; i < 100; ++i) {
    ASSERT_TRUE(visited[i]) << "Entry #" << i << " was never visited";
  }
}

// Test insert() method.
TEST_F(StringMapTest, Insert) {
  SCOPED_TRACE("InsertTest");
  testMap.insert(std::pair{std::string_view(testKeyFirst, testKeyLength), 1u});
  assertSingleItemMap();
}

// Test insert(pair<K, V>) method
TEST_F(StringMapTest, InsertPair) {
  bool Inserted;
  StringMap<uint32_t>::iterator NewIt;
  std::tie(NewIt, Inserted) =
      testMap.insert(std::pair{testKeyFirst, testValue});
  EXPECT_EQ(1u, testMap.size());
  EXPECT_EQ(testValue, testMap[testKeyFirst]);
  EXPECT_EQ(testKeyFirst, NewIt->first);
  EXPECT_EQ(testValue, NewIt->second);
  EXPECT_TRUE(Inserted);

  StringMap<uint32_t>::iterator ExistingIt;
  std::tie(ExistingIt, Inserted) =
      testMap.insert(std::pair{testKeyFirst, testValue + 1});
  EXPECT_EQ(1u, testMap.size());
  EXPECT_EQ(testValue, testMap[testKeyFirst]);
  EXPECT_FALSE(Inserted);
  EXPECT_EQ(NewIt, ExistingIt);
}

TEST_F(StringMapTest, InsertOrAssign) {
  struct A : CountCopyAndMove {
    explicit A(int v) : v(v) {}
    int v;
  };
  StringMap<A> t;

  auto try1 = t.insert_or_assign("A", A(1));
  EXPECT_TRUE(try1.second);
  EXPECT_EQ(1, try1.first->second.v);
  EXPECT_EQ(1, try1.first->second.move);

  auto try2 = t.insert_or_assign("A", A(2));
  EXPECT_FALSE(try2.second);
  EXPECT_EQ(2, try2.first->second.v);
  EXPECT_EQ(2, try1.first->second.move);

  EXPECT_EQ(try1.first, try2.first);
  EXPECT_EQ(0, try1.first->second.copy);
}

// Create a non-default constructable value
struct StringMapTestStruct {
  explicit StringMapTestStruct(int i) : i(i) {}
  StringMapTestStruct() = delete;
  int i;
};

TEST_F(StringMapTest, NonDefaultConstructable) {
  StringMap<StringMapTestStruct> t;
  t.insert(std::pair{"Test", StringMapTestStruct(123)});
  StringMap<StringMapTestStruct>::iterator iter = t.find("Test");
  ASSERT_NE(iter, t.end());
  ASSERT_EQ(iter->second.i, 123);
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

TEST_F(StringMapTest, MoveConstruct) {
  StringMap<int> A;
  A["x"] = 42;
  StringMap<int> B = std::move(A);
  ASSERT_EQ(A.size(), 0u);
  ASSERT_EQ(B.size(), 1u);
  ASSERT_EQ(B["x"], 42);
  ASSERT_EQ(B.count("y"), 0u);
}

TEST_F(StringMapTest, MoveAssignment) {
  StringMap<int> A;
  A["x"] = 42;
  StringMap<int> B;
  B["y"] = 117;
  A = std::move(B);
  ASSERT_EQ(A.size(), 1u);
  ASSERT_EQ(B.size(), 0u);
  ASSERT_EQ(A["y"], 117);
  ASSERT_EQ(B.count("x"), 0u);
}

TEST_F(StringMapTest, EqualEmpty) {
  StringMap<int> A;
  StringMap<int> B;
  ASSERT_TRUE(A == B);
  ASSERT_FALSE(A != B);
  ASSERT_TRUE(A == A);  // self check
}

TEST_F(StringMapTest, EqualWithValues) {
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

  ASSERT_TRUE(A == B);
  ASSERT_TRUE(B == A);
  ASSERT_FALSE(A != B);
  ASSERT_FALSE(B != A);
  ASSERT_TRUE(A == A);  // self check
}

TEST_F(StringMapTest, NotEqualMissingKeys) {
  StringMap<int> A;
  A["A"] = 1;
  A["B"] = 2;

  StringMap<int> B;
  B["A"] = 1;
  B["B"] = 2;
  B["C"] = 3;
  B["D"] = 3;

  ASSERT_FALSE(A == B);
  ASSERT_FALSE(B == A);
  ASSERT_TRUE(A != B);
  ASSERT_TRUE(B != A);
}

TEST_F(StringMapTest, NotEqualWithDifferentValues) {
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

  ASSERT_FALSE(A == B);
  ASSERT_FALSE(B == A);
  ASSERT_TRUE(A != B);
  ASSERT_TRUE(B != A);
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

TEST_F(StringMapTest, MoveDtor) {
  int InstanceCount = 0;
  StringMap<Countable> A;
  A.insert(std::pair{"x", Countable(42, InstanceCount)});
  ASSERT_EQ(InstanceCount, 1);
  auto I = A.find("x");
  ASSERT_NE(I, A.end());
  ASSERT_EQ(I->second.Number, 42);

  StringMap<Countable> B;
  B = std::move(A);
  ASSERT_EQ(InstanceCount, 1);
  ASSERT_TRUE(A.empty());
  I = B.find("x");
  ASSERT_NE(I, B.end());
  ASSERT_EQ(I->second.Number, 42);

  B = StringMap<Countable>();
  ASSERT_EQ(InstanceCount, 0);
  ASSERT_TRUE(B.empty());
}

TEST_F(StringMapTest, StructuredBindings) {
  StringMap<int> A;
  A["a"] = 42;

  for (auto& [Key, Value] : A) {
    EXPECT_EQ("a", Key);
    EXPECT_EQ(42, Value);
  }
}

TEST_F(StringMapTest, StructuredBindingsMoveOnly) {
  StringMap<MoveOnly> A;
  A.insert(std::pair{"a", MoveOnly(42)});

  for (auto&& [Key, Value] : A) {
    EXPECT_EQ("a", Key);
    EXPECT_EQ(42, Value.i);
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

TEST(StringMapCustomTest, BracketOperatorCtor) {
  StringMap<CountCtorCopyAndMove> Map;
  CountCtorCopyAndMove::Ctor = 0;
  Map["abcd"];
  EXPECT_EQ(1u, CountCtorCopyAndMove::Ctor);
  // Test that operator[] does not create a value when it is already in the map
  CountCtorCopyAndMove::Ctor = 0;
  Map["abcd"];
  EXPECT_EQ(0u, CountCtorCopyAndMove::Ctor);
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
TEST(StringMapCustomTest, Emplace) {
  StringMap<NonMoveableNonCopyableType> Map;
  Map.try_emplace("abcd", 42);
  EXPECT_EQ(1u, Map.count("abcd"));
  EXPECT_EQ(42, Map["abcd"].Data);
}

}  // namespace
