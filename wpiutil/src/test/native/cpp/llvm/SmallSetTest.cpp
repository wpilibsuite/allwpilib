//===- llvm/unittest/ADT/SmallSetTest.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// SmallSet unit tests.
//
//===----------------------------------------------------------------------===//

#include "wpi/util/SmallSet.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <algorithm>
#include <string>

using namespace wpi::util;

TEST_CASE("SmallSetTest ConstructorIteratorPair", "[wpiutil][llvm]") {
  std::initializer_list<int> L = {1, 2, 3, 4, 5};
  SmallSet<int, 4> S(std::begin(L), std::end(L));
  CHECK_THAT(S, Catch::Matchers::UnorderedRangeEquals(L));
}

TEST_CASE("SmallSet ConstructorInitializerList", "[wpiutil][llvm]") {
  std::initializer_list<int> L = {1, 2, 3, 4, 5};
  SmallSet<int, 4> S = {1, 2, 3, 4, 5};
  CHECK_THAT(S, Catch::Matchers::UnorderedRangeEquals(L));
}

TEST_CASE("SmallSet CopyConstructor", "[wpiutil][llvm]") {
  SmallSet<int, 4> S = {1, 2, 3};
  SmallSet<int, 4> T = S;

  CHECK_THAT(S, Catch::Matchers::RangeEquals(T));
}

TEST_CASE("SmallSet MoveConstructor", "[wpiutil][llvm]") {
  std::initializer_list<int> L = {1, 2, 3};
  SmallSet<int, 4> S = L;
  SmallSet<int, 4> T = std::move(S);

  CHECK_THAT(T, Catch::Matchers::UnorderedRangeEquals(L));
}

TEST_CASE("SmallSet CopyAssignment", "[wpiutil][llvm]") {
  SmallSet<int, 4> S = {1, 2, 3};
  SmallSet<int, 4> T;
  T = S;

  CHECK_THAT(S, Catch::Matchers::RangeEquals(T));
}

TEST_CASE("SmallSet MoveAssignment", "[wpiutil][llvm]") {
  std::initializer_list<int> L = {1, 2, 3};
  SmallSet<int, 4> S = L;
  SmallSet<int, 4> T;
  T = std::move(S);

  CHECK_THAT(T, Catch::Matchers::UnorderedRangeEquals(L));
}

TEST_CASE("SmallSetTest Insert", "[wpiutil][llvm]") {

  SmallSet<int, 4> s1;

  for (int i = 0; i < 4; i++) {
    auto InsertResult = s1.insert(i);
    CHECK((*InsertResult.first) == (i));
    CHECK((InsertResult.second) == (true));
  }

  for (int i = 0; i < 4; i++) {
    auto InsertResult = s1.insert(i);
    CHECK((*InsertResult.first) == (i));
    CHECK((InsertResult.second) == (false));
  }

  CHECK((4u) == (s1.size()));

  for (int i = 0; i < 4; i++)
    CHECK((1u) == (s1.count(i)));

  CHECK((0u) == (s1.count(4)));
}

TEST_CASE("SmallSetTest InsertPerfectFwd", "[wpiutil][llvm]") {
  struct Value {
    int Key;
    bool Moved;

    Value(int Key) : Key(Key), Moved(false) {}
    Value(const Value &) = default;
    Value(Value &&Other) : Key(Other.Key), Moved(false) { Other.Moved = true; }
    bool operator==(const Value &Other) const { return Key == Other.Key; }
    bool operator<(const Value &Other) const { return Key < Other.Key; }
  };

  {
    SmallSet<Value, 4> S;
    Value V1(1), V2(2);

    S.insert(V1);
    CHECK((V1.Moved) == (false));

    S.insert(std::move(V2));
    CHECK((V2.Moved) == (true));
  }
  {
    SmallSet<Value, 1> S;
    Value V1(1), V2(2);

    S.insert(V1);
    CHECK((V1.Moved) == (false));

    S.insert(std::move(V2));
    CHECK((V2.Moved) == (true));
  }
}

TEST_CASE("SmallSetTest CtorRange", "[wpiutil][llvm]") {
  constexpr unsigned Args[] = {3, 1, 2};
  SmallSet<int, 4> s1(wpi::util::from_range, Args);
  CHECK_THAT(s1, Catch::Matchers::UnorderedRangeEquals({1, 2, 3}));
}

TEST_CASE("SmallSetTest InsertRange", "[wpiutil][llvm]") {
  SmallSet<int, 4> s1;
  constexpr unsigned Args[] = {3, 1, 2};
  s1.insert_range(Args);
  CHECK_THAT(s1, Catch::Matchers::UnorderedRangeEquals({1, 2, 3}));
}

TEST_CASE("SmallSetTest Grow", "[wpiutil][llvm]") {
  SmallSet<int, 4> s1;

  for (int i = 0; i < 8; i++) {
    auto InsertResult = s1.insert(i);
    CHECK((*InsertResult.first) == (i));
    CHECK((InsertResult.second) == (true));
  }

  for (int i = 0; i < 8; i++) {
    auto InsertResult = s1.insert(i);
    CHECK((*InsertResult.first) == (i));
    CHECK((InsertResult.second) == (false));
  }

  CHECK((8u) == (s1.size()));

  for (int i = 0; i < 8; i++)
    CHECK((1u) == (s1.count(i)));

  CHECK((0u) == (s1.count(8)));
}

TEST_CASE("SmallSetTest Erase", "[wpiutil][llvm]") {
  SmallSet<int, 4> s1;

  for (int i = 0; i < 8; i++)
    s1.insert(i);

  CHECK((8u) == (s1.size()));

  // Remove elements one by one and check if all other elements are still there.
  for (int i = 0; i < 8; i++) {
    CHECK((1u) == (s1.count(i)));
    CHECK(s1.erase(i));
    CHECK((0u) == (s1.count(i)));
    CHECK((8u - i - 1) == (s1.size()));
    for (int j = i + 1; j < 8; j++)
      CHECK((1u) == (s1.count(j)));
  }

  CHECK((0u) == (s1.count(8)));
}

TEST_CASE("SmallSetTest IteratorInt", "[wpiutil][llvm]") {
  SmallSet<int, 4> s1;

  // Test the 'small' case.
  for (int i = 0; i < 3; i++)
    s1.insert(i);

  std::vector<int> V(s1.begin(), s1.end());
  // Make sure the elements are in the expected order.
  std::sort(V.begin(), V.end());
  for (int i = 0; i < 3; i++)
    CHECK((i) == (V[i]));

  // Test the 'big' case by adding a few more elements to switch to std::set
  // internally.
  for (int i = 3; i < 6; i++)
    s1.insert(i);

  V.assign(s1.begin(), s1.end());
  // Make sure the elements are in the expected order.
  std::sort(V.begin(), V.end());
  for (int i = 0; i < 6; i++)
    CHECK((i) == (V[i]));
}

TEST_CASE("SmallSetTest IteratorString", "[wpiutil][llvm]") {
  // Test SmallSetIterator for SmallSet with a type with non-trivial
  // ctors/dtors.
  SmallSet<std::string, 2> s1;

  s1.insert("str 1");
  s1.insert("str 2");
  s1.insert("str 1");

  std::vector<std::string> V(s1.begin(), s1.end());
  std::sort(V.begin(), V.end());
  CHECK((2u) == (s1.size()));
  CHECK(("str 1") == (V[0]));
  CHECK(("str 2") == (V[1]));

  s1.insert("str 4");
  s1.insert("str 0");
  s1.insert("str 4");

  V.assign(s1.begin(), s1.end());
  // Make sure the elements are in the expected order.
  std::sort(V.begin(), V.end());
  CHECK((4u) == (s1.size()));
  CHECK(("str 0") == (V[0]));
  CHECK(("str 1") == (V[1]));
  CHECK(("str 2") == (V[2]));
  CHECK(("str 4") == (V[3]));
}

TEST_CASE("SmallSetTest IteratorIncMoveCopy", "[wpiutil][llvm]") {
  // Test SmallSetIterator for SmallSet with a type with non-trivial
  // ctors/dtors.
  SmallSet<std::string, 2> s1;

  s1.insert("str 1");
  s1.insert("str 2");

  auto Iter = s1.begin();
  CHECK(("str 1") == (*Iter));
  ++Iter;
  CHECK(("str 2") == (*Iter));

  s1.insert("str 4");
  s1.insert("str 0");
  auto Iter2 = s1.begin();
  Iter = std::move(Iter2);
  CHECK(("str 0") == (*Iter));
}

TEST_CASE("SmallSetTest EqualityComparisonTest", "[wpiutil][llvm]") {
  SmallSet<int, 8> s1small;
  SmallSet<int, 10> s2small;
  SmallSet<int, 3> s3large;
  SmallSet<int, 8> s4large;

  for (int i = 1; i < 5; i++) {
    s1small.insert(i);
    s2small.insert(5 - i);
    s3large.insert(i);
  }
  for (int i = 1; i < 11; i++)
    s4large.insert(i);

  CHECK((s1small) == (s1small));
  CHECK((s3large) == (s3large));

  CHECK((s1small) == (s2small));
  CHECK((s1small) == (s3large));
  CHECK((s2small) == (s3large));

  CHECK((s1small) != (s4large));
  CHECK((s4large) != (s3large));
}

TEST_CASE("SmallSetTest Contains", "[wpiutil][llvm]") {
  SmallSet<int, 2> Set;
  CHECK_FALSE(Set.contains(0));
  CHECK_FALSE(Set.contains(1));

  Set.insert(0);
  Set.insert(1);
  CHECK(Set.contains(0));
  CHECK(Set.contains(1));

  Set.insert(1);
  CHECK(Set.contains(0));
  CHECK(Set.contains(1));

  Set.erase(1);
  CHECK(Set.contains(0));
  CHECK_FALSE(Set.contains(1));

  Set.insert(1);
  Set.insert(2);
  CHECK(Set.contains(0));
  CHECK(Set.contains(1));
  CHECK(Set.contains(2));
}
