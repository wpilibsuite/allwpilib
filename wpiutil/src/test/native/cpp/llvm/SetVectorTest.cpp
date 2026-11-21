//===- llvm/unittest/ADT/SetVector.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// SetVector unit tests.
//
//===----------------------------------------------------------------------===//

#include "wpi/util/SetVector.hpp"
#include "wpi/util/SmallPtrSet.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <algorithm>
#include <vector>

using namespace wpi::util;

TEST_CASE("SetVector EraseTest", "[wpiutil][llvm]") {
  SetVector<int> S;
  S.insert(0);
  S.insert(1);
  S.insert(2);

  auto I = S.erase(std::next(S.begin()));

  // Test that the returned iterator is the expected one-after-erase
  // and the size/contents is the expected sequence {0, 2}.
  CHECK(std::next(S.begin()) == I);
  CHECK(2u == S.size());
  CHECK(0 == *S.begin());
  CHECK(2 == *std::next(S.begin()));
}

TEST_CASE("SetVector ContainsTest", "[wpiutil][llvm]") {
  SetVector<int> S;
  S.insert(0);
  S.insert(1);
  S.insert(2);

  CHECK(S.contains(0));
  CHECK(S.contains(1));
  CHECK(S.contains(2));
  CHECK_FALSE(S.contains(-1));

  S.insert(2);
  CHECK(S.contains(2));

  S.remove(2);
  CHECK_FALSE(S.contains(2));
}

TEST_CASE("SetVector ConstPtrKeyTest", "[wpiutil][llvm]") {
  SetVector<int *> S, T;
  int i, j, k, m, n;

  S.insert(&i);
  S.insert(&j);
  S.insert(&k);

  CHECK(S.contains(&i));
  CHECK(S.contains(&j));
  CHECK(S.contains(&k));

  CHECK(S.contains((const int *)&i));
  CHECK(S.contains((const int *)&j));
  CHECK(S.contains((const int *)&k));

  CHECK(S.contains(S[0]));
  CHECK(S.contains(S[1]));
  CHECK(S.contains(S[2]));

  S.remove(&k);
  CHECK_FALSE(S.contains(&k));
  CHECK_FALSE(S.contains((const int *)&k));

  T.insert(&j);
  T.insert(&m);
  T.insert(&n);

  CHECK(S.set_union(T));
  CHECK(S.contains(&m));
  CHECK(S.contains((const int *)&m));

  S.set_subtract(T);
  CHECK_FALSE(S.contains(&j));
  CHECK_FALSE(S.contains((const int *)&j));
}

TEST_CASE("SetVector CtorRange", "[wpiutil][llvm]") {
  constexpr unsigned Args[] = {3, 1, 2};
  SetVector<unsigned> Set(wpi::util::from_range, Args);
  CHECK_THAT(Set, Catch::Matchers::RangeEquals(std::vector<unsigned>{3, 1, 2}));
}

TEST_CASE("SetVector InsertRange", "[wpiutil][llvm]") {
  SetVector<unsigned> Set;
  constexpr unsigned Args[] = {3, 1, 2};
  Set.insert_range(Args);
  CHECK_THAT(Set, Catch::Matchers::RangeEquals(std::vector<unsigned>{3, 1, 2}));
}

TEST_CASE("SmallSetVector CtorRange", "[wpiutil][llvm]") {
  constexpr unsigned Args[] = {3, 1, 2};
  SmallSetVector<unsigned, 4> Set(wpi::util::from_range, Args);
  CHECK_THAT(Set, Catch::Matchers::RangeEquals(std::vector<unsigned>{3, 1, 2}));
}

TEST_CASE("SmallSetVector InsertionOrderAfterRemovalAndReinsertion", "[wpiutil][llvm]") {
  for (int Size : {3, 32}) {
    SmallSetVector<int, 4> Set;
    std::vector<int> Expected;
    for (int I = 0; I < Size; ++I) {
      CHECK(Set.insert(I));
      CHECK_FALSE(Set.insert(I));
      Expected.push_back(I);
    }
    CHECK_THAT(Set, Catch::Matchers::RangeEquals(Expected));
    CHECK(Set.size() == Expected.size());
    CHECK(Set.remove(1));
    CHECK_FALSE(Set.remove(1));
    CHECK_FALSE(Set.contains(1));
    std::erase(Expected, 1);
    CHECK_THAT(Set, Catch::Matchers::RangeEquals(Expected));
    CHECK(Set.insert(1));
    Expected.push_back(1);
    CHECK_THAT(Set, Catch::Matchers::RangeEquals(Expected));
  }
}
