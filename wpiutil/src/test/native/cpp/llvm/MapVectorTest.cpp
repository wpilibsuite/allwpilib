//===- unittest/ADT/MapVectorTest.cpp - MapVector unit tests ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/MapVector.hpp"
#include "wpi/util/iterator_range.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <memory>
#include <utility>
#include <vector>

using namespace wpi::util;

namespace {
struct CountCopyAndMove {
  CountCopyAndMove() = default;
  CountCopyAndMove(const CountCopyAndMove &) { copy = 1; }
  CountCopyAndMove(CountCopyAndMove &&) { move = 1; }
  void operator=(const CountCopyAndMove &) { ++copy; }
  void operator=(CountCopyAndMove &&) { ++move; }
  int copy = 0;
  int move = 0;
};

struct A : CountCopyAndMove {
  A(int v) : v(v) {}
  int v;
};
} // namespace

namespace wpi::util {
template <> struct DenseMapInfo<A> {
  static unsigned getHashValue(const A &Val) { return (unsigned)(Val.v * 37U); }
  static bool isEqual(const A &LHS, const A &RHS) { return LHS.v == RHS.v; }
};
} // namespace wpi::util

namespace {
TEST_CASE("MapVectorTest swap", "[wpiutil][llvm]") {
  MapVector<int, int> MV1, MV2;
  std::pair<MapVector<int, int>::iterator, bool> R;

  R = MV1.insert(std::make_pair(1, 2));
  REQUIRE(R.first == MV1.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK(R.second);

  CHECK_FALSE(MV1.empty());
  CHECK(MV2.empty());
  MV2.swap(MV1);
  CHECK(MV1.empty());
  CHECK_FALSE(MV2.empty());

  auto I = MV1.find(1);
  REQUIRE(MV1.end() == I);

  I = MV2.find(1);
  REQUIRE(I == MV2.begin());
  CHECK(I->first == 1);
  CHECK(I->second == 2);
}

TEST_CASE("MapVectorTest insert_pop", "[wpiutil][llvm]") {
  MapVector<int, int> MV;
  std::pair<MapVector<int, int>::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK(R.second);

  R = MV.insert(std::make_pair(1, 3));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK_FALSE(R.second);

  R = MV.insert(std::make_pair(4, 5));
  REQUIRE(R.first != MV.end());
  CHECK(R.first->first == 4);
  CHECK(R.first->second == 5);
  CHECK(R.second);

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 5);

  MV.pop_back();
  CHECK(MV.size() == 1u);
  CHECK(MV[1] == 2);

  R = MV.insert(std::make_pair(4, 7));
  REQUIRE(R.first != MV.end());
  CHECK(R.first->first == 4);
  CHECK(R.first->second == 7);
  CHECK(R.second);

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 7);
}

TEST_CASE("MapVectorTest try_emplace", "[wpiutil][llvm]") {
  struct AAndU {
    A a;
    std::unique_ptr<int> b;
    AAndU(A a, std::unique_ptr<int> b) : a(a), b(std::move(b)) {}
  };
  MapVector<A, AAndU> mv;

  A zero(0);
  auto try0 = mv.try_emplace(zero, zero, nullptr);
  CHECK(try0.second);
  CHECK(0 == try0.first->second.a.v);
  CHECK(1 == try0.first->second.a.copy);
  CHECK(0 == try0.first->second.a.move);

  auto try1 = mv.try_emplace(zero, zero, nullptr);
  CHECK_FALSE(try1.second);
  CHECK(0 == try1.first->second.a.v);
  CHECK(1 == try1.first->second.a.copy);
  CHECK(0 == try1.first->second.a.move);

  CHECK(try0.first == try1.first);
  CHECK(1 == try1.first->first.copy);
  CHECK(0 == try1.first->first.move);

  A two(2);
  auto try2 = mv.try_emplace(2, std::move(two), std::make_unique<int>(2));
  CHECK(try2.second);
  CHECK(2 == try2.first->second.a.v);
  CHECK(0 == try2.first->second.a.move);

  std::unique_ptr<int> p(new int(3));
  auto try3 = mv.try_emplace(std::move(two), 3, std::move(p));
  CHECK_FALSE(try3.second);
  CHECK(2 == try3.first->second.a.v);
  CHECK(1 == try3.first->second.a.copy);
  CHECK(0 == try3.first->second.a.move);

  CHECK(try2.first == try3.first);
  CHECK(0 == try3.first->first.copy);
  CHECK(1 == try3.first->first.move);
  CHECK(nullptr != p);
}

TEST_CASE("MapVectorTest insert_or_assign", "[wpiutil][llvm]") {
  MapVector<A, A> mv;

  A zero(0);
  auto try0 = mv.insert_or_assign(zero, zero);
  CHECK(try0.second);
  CHECK(0 == try0.first->second.v);
  CHECK(1 == try0.first->second.copy);
  CHECK(0 == try0.first->second.move);

  auto try1 = mv.insert_or_assign(zero, zero);
  CHECK_FALSE(try1.second);
  CHECK(0 == try1.first->second.v);
  CHECK(2 == try1.first->second.copy);
  CHECK(0 == try1.first->second.move);

  CHECK(try0.first == try1.first);
  CHECK(1 == try1.first->first.copy);
  CHECK(0 == try1.first->first.move);

  A two(2);
  auto try2 = mv.try_emplace(2, std::move(two));
  CHECK(try2.second);
  CHECK(2 == try2.first->second.v);
  CHECK(1 == try2.first->second.move);

  auto try3 = mv.insert_or_assign(std::move(two), 3);
  CHECK_FALSE(try3.second);
  CHECK(3 == try3.first->second.v);
  CHECK(0 == try3.first->second.copy);
  CHECK(2 == try3.first->second.move);

  CHECK(try2.first == try3.first);
  CHECK(0 == try3.first->first.copy);
  CHECK(1 == try3.first->first.move);
}

TEST_CASE("MapVectorTest erase", "[wpiutil][llvm]") {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 2));
  MV.insert(std::make_pair(3, 4));
  MV.insert(std::make_pair(5, 6));
  REQUIRE(MV.size() == 3u);

  REQUIRE(MV.contains(1));
  MV.erase(MV.find(1));
  REQUIRE(MV.size() == 2u);
  REQUIRE_FALSE(MV.contains(1));
  REQUIRE(MV.find(1) == MV.end());
  REQUIRE(MV[3] == 4);
  REQUIRE(MV[5] == 6);

  REQUIRE(MV.erase(3) == 1u);
  REQUIRE(MV.size() == 1u);
  REQUIRE(MV.find(3) == MV.end());
  REQUIRE(MV[5] == 6);

  REQUIRE(MV.erase(79) == 0u);
  REQUIRE(MV.size() == 1u);
}

TEST_CASE("MapVectorTest remove_if", "[wpiutil][llvm]") {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  REQUIRE(MV.size() == 6u);

  MV.remove_if([](const std::pair<int, int> &Val) { return Val.second % 2; });
  REQUIRE(MV.size() == 3u);
  REQUIRE(MV.find(1) == MV.end());
  REQUIRE(MV.find(3) == MV.end());
  REQUIRE(MV.find(5) == MV.end());
  REQUIRE(MV[2] == 12);
  REQUIRE(MV[4] == 14);
  REQUIRE(MV[6] == 16);
}

TEST_CASE("MapVectorTest iteration_test", "[wpiutil][llvm]") {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  REQUIRE(MV.size() == 6u);

  int count = 1;
  for (auto P : make_range(MV.begin(), MV.end())) {
    REQUIRE(P.first == count);
    count++;
  }

  count = 6;
  for (auto P : make_range(MV.rbegin(), MV.rend())) {
    REQUIRE(P.first == count);
    count--;
  }
}

TEST_CASE("MapVectorTest NonCopyable", "[wpiutil][llvm]") {
  MapVector<int, std::unique_ptr<int>> MV;
  MV.insert(std::make_pair(1, std::make_unique<int>(1)));
  MV.insert(std::make_pair(2, std::make_unique<int>(2)));

  REQUIRE(MV.count(1) == 1u);
  REQUIRE(*MV.find(2)->second == 2);
}

TEST_CASE("MapVectorTest GetArrayRef", "[wpiutil][llvm]") {
  MapVector<int, int> MV;

  // The underlying vector is empty to begin with.
  CHECK(MV.getArrayRef().empty());

  // Test inserted element.
  MV.insert(std::make_pair(100, 99));
  CHECK_THAT(MV.getArrayRef(), Catch::Matchers::RangeEquals(std::vector{std::pair(100, 99)}));

  // Inserting a different element for an existing key won't change the
  // underlying vector.
  auto [Iter, Inserted] = MV.try_emplace(100, 98);
  CHECK_FALSE(Inserted);
  CHECK(Iter->second == 99);
  CHECK_THAT(MV.getArrayRef(), Catch::Matchers::RangeEquals(std::vector{std::pair(100, 99)}));

  // Inserting a new element. Tests that elements are in order in the underlying
  // array.
  MV.insert(std::make_pair(99, 98));
  CHECK_THAT(MV.getArrayRef(), Catch::Matchers::RangeEquals(std::vector{std::pair(100, 99), std::pair(99, 98)}));
}

TEST_CASE("MapVectorTest AtTest", "[wpiutil][llvm]") {
  MapVector<int, int> MV;
  MV[0] = 10;
  MV[1] = 11;
  CHECK(MV.at(0) == 10);
  CHECK(MV.at(1) == 11);

  MV.at(1) = 12;
  CHECK(MV.at(1) == 12);

  const auto &ConstMV = MV;
  CHECK(ConstMV.at(0) == 10);
  CHECK(ConstMV.at(1) == 12);
}

TEST_CASE("MapVectorTest KeysValuesIterator", "[wpiutil][llvm]") {
  MapVector<int, int> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));

  CHECK_THAT(MV.keys(), Catch::Matchers::RangeEquals(std::vector{1, 2, 3, 4, 5, 6}));
  CHECK_THAT(MV.values(), Catch::Matchers::RangeEquals(std::vector{11, 12, 13, 14, 15, 16}));

  const MapVector<int, int> &ConstMV = MV;
  CHECK_THAT(ConstMV.keys(), Catch::Matchers::RangeEquals(std::vector{1, 2, 3, 4, 5, 6}));
  CHECK_THAT(ConstMV.values(), Catch::Matchers::RangeEquals(std::vector{11, 12, 13, 14, 15, 16}));
}

TEMPLATE_TEST_CASE("MapVectorMappedTypeTest DifferentDenseMap", "[wpiutil][llvm]",
                   int, long, long long, unsigned, unsigned long,
                   unsigned long long) {
  // Test that using a map with a mapped type other than 'unsigned' compiles
  // and works.
  using IntType = TestType;
  using MapVectorType = MapVector<int, int, DenseMap<int, IntType>>;

  MapVectorType MV;
  std::pair<typename MapVectorType::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK(R.second);

  const std::pair<int, int> Elem(1, 3);
  R = MV.insert(Elem);
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK_FALSE(R.second);

  int& value = MV[4];
  CHECK(value == 0);
  value = 5;

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 5);
}

TEST_CASE("SmallMapVectorSmallTest insert_pop", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 32> MV;
  std::pair<SmallMapVector<int, int, 32>::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK(R.second);

  R = MV.insert(std::make_pair(1, 3));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK_FALSE(R.second);

  R = MV.insert(std::make_pair(4, 5));
  REQUIRE(R.first != MV.end());
  CHECK(R.first->first == 4);
  CHECK(R.first->second == 5);
  CHECK(R.second);

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 5);

  MV.pop_back();
  CHECK(MV.size() == 1u);
  CHECK(MV[1] == 2);

  R = MV.insert(std::make_pair(4, 7));
  REQUIRE(R.first != MV.end());
  CHECK(R.first->first == 4);
  CHECK(R.first->second == 7);
  CHECK(R.second);

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 7);
}

TEST_CASE("SmallMapVectorSmallTest erase", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 32> MV;

  MV.insert(std::make_pair(1, 2));
  MV.insert(std::make_pair(3, 4));
  MV.insert(std::make_pair(5, 6));
  REQUIRE(MV.size() == 3u);

  MV.erase(MV.find(1));
  REQUIRE(MV.size() == 2u);
  REQUIRE(MV.find(1) == MV.end());
  REQUIRE(MV[3] == 4);
  REQUIRE(MV[5] == 6);

  REQUIRE(MV.erase(3) == 1u);
  REQUIRE(MV.size() == 1u);
  REQUIRE(MV.find(3) == MV.end());
  REQUIRE(MV[5] == 6);

  REQUIRE(MV.erase(79) == 0u);
  REQUIRE(MV.size() == 1u);
}

TEST_CASE("SmallMapVectorSmallTest remove_if", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 32> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  REQUIRE(MV.size() == 6u);

  MV.remove_if([](const std::pair<int, int> &Val) { return Val.second % 2; });
  REQUIRE(MV.size() == 3u);
  REQUIRE(MV.find(1) == MV.end());
  REQUIRE(MV.find(3) == MV.end());
  REQUIRE(MV.find(5) == MV.end());
  REQUIRE(MV[2] == 12);
  REQUIRE(MV[4] == 14);
  REQUIRE(MV[6] == 16);
}

TEST_CASE("SmallMapVectorSmallTest iteration_test", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 32> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  REQUIRE(MV.size() == 6u);

  int count = 1;
  for (auto P : make_range(MV.begin(), MV.end())) {
    REQUIRE(P.first == count);
    count++;
  }

  count = 6;
  for (auto P : make_range(MV.rbegin(), MV.rend())) {
    REQUIRE(P.first == count);
    count--;
  }
}

TEST_CASE("SmallMapVectorSmallTest NonCopyable", "[wpiutil][llvm]") {
  SmallMapVector<int, std::unique_ptr<int>, 8> MV;
  MV.insert(std::make_pair(1, std::make_unique<int>(1)));
  MV.insert(std::make_pair(2, std::make_unique<int>(2)));

  REQUIRE(MV.count(1) == 1u);
  REQUIRE(*MV.find(2)->second == 2);
}

TEST_CASE("SmallMapVectorLargeTest insert_pop", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 1> MV;
  std::pair<SmallMapVector<int, int, 1>::iterator, bool> R;

  R = MV.insert(std::make_pair(1, 2));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK(R.second);

  R = MV.insert(std::make_pair(1, 3));
  REQUIRE(R.first == MV.begin());
  CHECK(R.first->first == 1);
  CHECK(R.first->second == 2);
  CHECK_FALSE(R.second);

  R = MV.insert(std::make_pair(4, 5));
  REQUIRE(R.first != MV.end());
  CHECK(R.first->first == 4);
  CHECK(R.first->second == 5);
  CHECK(R.second);

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 5);

  MV.pop_back();
  CHECK(MV.size() == 1u);
  CHECK(MV[1] == 2);

  R = MV.insert(std::make_pair(4, 7));
  REQUIRE(R.first != MV.end());
  CHECK(R.first->first == 4);
  CHECK(R.first->second == 7);
  CHECK(R.second);

  CHECK(MV.size() == 2u);
  CHECK(MV[1] == 2);
  CHECK(MV[4] == 7);
}

TEST_CASE("SmallMapVectorLargeTest erase", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 1> MV;

  MV.insert(std::make_pair(1, 2));
  MV.insert(std::make_pair(3, 4));
  MV.insert(std::make_pair(5, 6));
  REQUIRE(MV.size() == 3u);

  MV.erase(MV.find(1));
  REQUIRE(MV.size() == 2u);
  REQUIRE(MV.find(1) == MV.end());
  REQUIRE(MV[3] == 4);
  REQUIRE(MV[5] == 6);

  REQUIRE(MV.erase(3) == 1u);
  REQUIRE(MV.size() == 1u);
  REQUIRE(MV.find(3) == MV.end());
  REQUIRE(MV[5] == 6);

  REQUIRE(MV.erase(79) == 0u);
  REQUIRE(MV.size() == 1u);
}

TEST_CASE("SmallMapVectorLargeTest remove_if", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 1> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  REQUIRE(MV.size() == 6u);

  MV.remove_if([](const std::pair<int, int> &Val) { return Val.second % 2; });
  REQUIRE(MV.size() == 3u);
  REQUIRE(MV.find(1) == MV.end());
  REQUIRE(MV.find(3) == MV.end());
  REQUIRE(MV.find(5) == MV.end());
  REQUIRE(MV[2] == 12);
  REQUIRE(MV[4] == 14);
  REQUIRE(MV[6] == 16);
}

TEST_CASE("SmallMapVectorLargeTest iteration_test", "[wpiutil][llvm]") {
  SmallMapVector<int, int, 1> MV;

  MV.insert(std::make_pair(1, 11));
  MV.insert(std::make_pair(2, 12));
  MV.insert(std::make_pair(3, 13));
  MV.insert(std::make_pair(4, 14));
  MV.insert(std::make_pair(5, 15));
  MV.insert(std::make_pair(6, 16));
  REQUIRE(MV.size() == 6u);

  int count = 1;
  for (auto P : make_range(MV.begin(), MV.end())) {
    REQUIRE(P.first == count);
    count++;
  }

  count = 6;
  for (auto P : make_range(MV.rbegin(), MV.rend())) {
    REQUIRE(P.first == count);
    count--;
  }
}
} // namespace
