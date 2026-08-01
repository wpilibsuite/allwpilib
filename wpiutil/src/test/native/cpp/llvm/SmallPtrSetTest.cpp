//===- llvm/unittest/ADT/SmallPtrSetTest.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// SmallPtrSet unit tests.
//
//===----------------------------------------------------------------------===//

#include "wpi/util/SmallPtrSet.hpp"
#include "wpi/util/PointerIntPair.hpp"
#include "wpi/util/PointerLikeTypeTraits.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <algorithm>

using namespace wpi::util;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("SmallPtrSetTest Assignment", "[wpiutil][llvm]") {
  int buf[8];
  for (int i = 0; i < 8; ++i)
    buf[i] = 0;

  SmallPtrSet<int *, 4> s1 = {&buf[0], &buf[1]};
  SmallPtrSet<int *, 4> s2;
  (s2 = s1).insert(&buf[2]);

  // Self assign as well.
  (s2 = static_cast<SmallPtrSet<int *, 4> &>(s2)).insert(&buf[3]);

  s1 = s2;
  CHECK((4U) == (s1.size()));
  for (int i = 0; i < 8; ++i)
    if (i < 4)
      CHECK(s1.count(&buf[i]));
    else
      CHECK_FALSE(s1.count(&buf[i]));

  // Assign and insert with initializer lists, and ones that contain both
  // duplicates and out-of-order elements.
  (s2 = {&buf[6], &buf[7], &buf[6]}).insert({&buf[5], &buf[4]});
  for (int i = 0; i < 8; ++i)
    if (i < 4)
      CHECK_FALSE(s2.count(&buf[i]));
    else
      CHECK(s2.count(&buf[i]));
}

TEST_CASE("SmallPtrSetTest GrowthTest", "[wpiutil][llvm]") {
  int i;
  int buf[8];
  for(i=0; i<8; ++i) buf[i]=0;


  SmallPtrSet<int *, 4> s;
  using iter = SmallPtrSet<int *, 4>::iterator;

  s.insert(&buf[0]);
  s.insert(&buf[1]);
  s.insert(&buf[2]);
  s.insert(&buf[3]);
  CHECK((4U) == (s.size()));

  i = 0;
  for(iter I=s.begin(), E=s.end(); I!=E; ++I, ++i)
      (**I)++;
  CHECK((4) == (i));
  for(i=0; i<8; ++i)
      CHECK((i < 4 ? 1 : 0) == buf[i]);

  s.insert(&buf[4]);
  s.insert(&buf[5]);
  s.insert(&buf[6]);
  s.insert(&buf[7]);

  i = 0;
  for(iter I=s.begin(), E=s.end(); I!=E; ++I, ++i)
      (**I)++;
  CHECK((8) == (i));
  s.erase(&buf[4]);
  s.erase(&buf[5]);
  s.erase(&buf[6]);
  s.erase(&buf[7]);
  CHECK((4U) == (s.size()));

  i = 0;
  for(iter I=s.begin(), E=s.end(); I!=E; ++I, ++i)
      (**I)++;
  CHECK((4) == (i));
  for(i=0; i<8; ++i)
      CHECK((i < 4 ? 3 : 1) == buf[i]);

  s.clear();
  for(i=0; i<8; ++i) buf[i]=0;
  for(i=0; i<128; ++i) s.insert(&buf[i%8]); // test repeated entires
  CHECK((8U) == (s.size()));
  for(iter I=s.begin(), E=s.end(); I!=E; ++I, ++i)
      (**I)++;
  for(i=0; i<8; ++i)
      CHECK((1) == (buf[i]));
}

TEST_CASE("SmallPtrSetTest CopyAndMoveTest", "[wpiutil][llvm]") {
  int buf[8];
  for (int i = 0; i < 8; ++i)
    buf[i] = 0;

  SmallPtrSet<int *, 4> s1;
  s1.insert(&buf[0]);
  s1.insert(&buf[1]);
  s1.insert(&buf[2]);
  s1.insert(&buf[3]);
  CHECK((4U) == (s1.size()));
  for (int i = 0; i < 8; ++i)
    if (i < 4)
      CHECK(s1.count(&buf[i]));
    else
      CHECK_FALSE(s1.count(&buf[i]));

  SmallPtrSet<int *, 4> s2(s1);
  CHECK((4U) == (s2.size()));
  for (int i = 0; i < 8; ++i)
    if (i < 4)
      CHECK(s2.count(&buf[i]));
    else
      CHECK_FALSE(s2.count(&buf[i]));

  s1 = s2;
  CHECK((4U) == (s1.size()));
  CHECK((4U) == (s2.size()));
  for (int i = 0; i < 8; ++i)
    if (i < 4)
      CHECK(s1.count(&buf[i]));
    else
      CHECK_FALSE(s1.count(&buf[i]));

  SmallPtrSet<int *, 4> s3(std::move(s1));
  CHECK((4U) == (s3.size()));
  CHECK(s1.empty());
  for (int i = 0; i < 8; ++i)
    if (i < 4)
      CHECK(s3.count(&buf[i]));
    else
      CHECK_FALSE(s3.count(&buf[i]));

  // Move assign into the moved-from object. Also test move of a non-small
  // container.
  s3.insert(&buf[4]);
  s3.insert(&buf[5]);
  s3.insert(&buf[6]);
  s3.insert(&buf[7]);
  s1 = std::move(s3);
  CHECK((8U) == (s1.size()));
  CHECK(s3.empty());
  for (int i = 0; i < 8; ++i)
    CHECK(s1.count(&buf[i]));

  // Copy assign into a moved-from object.
  s3 = s1;
  CHECK((8U) == (s3.size()));
  CHECK((8U) == (s1.size()));
  for (int i = 0; i < 8; ++i)
    CHECK(s3.count(&buf[i]));
}

TEST_CASE("SmallPtrSetTest SwapTest", "[wpiutil][llvm]") {
  int buf[10];

  SmallPtrSet<int *, 2> a;
  SmallPtrSet<int *, 2> b;

  a.insert(&buf[0]);
  a.insert(&buf[1]);
  b.insert(&buf[2]);

  CHECK((2U) == (a.size()));
  CHECK((1U) == (b.size()));
  CHECK(a.count(&buf[0]));
  CHECK(a.count(&buf[1]));
  CHECK_FALSE(a.count(&buf[2]));
  CHECK_FALSE(a.count(&buf[3]));
  CHECK_FALSE(b.count(&buf[0]));
  CHECK_FALSE(b.count(&buf[1]));
  CHECK(b.count(&buf[2]));
  CHECK_FALSE(b.count(&buf[3]));

  std::swap(a, b);

  CHECK((1U) == (a.size()));
  CHECK((2U) == (b.size()));
  CHECK_FALSE(a.count(&buf[0]));
  CHECK_FALSE(a.count(&buf[1]));
  CHECK(a.count(&buf[2]));
  CHECK_FALSE(a.count(&buf[3]));
  CHECK(b.count(&buf[0]));
  CHECK(b.count(&buf[1]));
  CHECK_FALSE(b.count(&buf[2]));
  CHECK_FALSE(b.count(&buf[3]));

  b.insert(&buf[3]);
  std::swap(a, b);

  CHECK((3U) == (a.size()));
  CHECK((1U) == (b.size()));
  CHECK(a.count(&buf[0]));
  CHECK(a.count(&buf[1]));
  CHECK_FALSE(a.count(&buf[2]));
  CHECK(a.count(&buf[3]));
  CHECK_FALSE(b.count(&buf[0]));
  CHECK_FALSE(b.count(&buf[1]));
  CHECK(b.count(&buf[2]));
  CHECK_FALSE(b.count(&buf[3]));

  std::swap(a, b);

  CHECK((1U) == (a.size()));
  CHECK((3U) == (b.size()));
  CHECK_FALSE(a.count(&buf[0]));
  CHECK_FALSE(a.count(&buf[1]));
  CHECK(a.count(&buf[2]));
  CHECK_FALSE(a.count(&buf[3]));
  CHECK(b.count(&buf[0]));
  CHECK(b.count(&buf[1]));
  CHECK_FALSE(b.count(&buf[2]));
  CHECK(b.count(&buf[3]));

  a.insert(&buf[4]);
  a.insert(&buf[5]);
  a.insert(&buf[6]);

  std::swap(b, a);

  CHECK((3U) == (a.size()));
  CHECK((4U) == (b.size()));
  CHECK(b.count(&buf[2]));
  CHECK(b.count(&buf[4]));
  CHECK(b.count(&buf[5]));
  CHECK(b.count(&buf[6]));
  CHECK(a.count(&buf[0]));
  CHECK(a.count(&buf[1]));
  CHECK(a.count(&buf[3]));
}

// Verify that dereferencing and iteration work.
TEST_CASE("SmallPtrSetTest dereferenceAndIterate", "[wpiutil][llvm]") {
  int Ints[] = {0, 1, 2, 3, 4, 5, 6, 7};
  SmallPtrSet<const int *, 4> S;
  for (int &I : Ints) {
    CHECK((&I) == (*S.insert(&I).first));
    CHECK((&I) == (*S.find(&I)));
  }

  // Iterate from each and count how many times each element is found.
  int Found[sizeof(Ints)/sizeof(int)] = {0};
  for (int &I : Ints)
    for (auto F = S.find(&I), E = S.end(); F != E; ++F)
      ++Found[*F - Ints];

  // Sort.  We should hit the first element just once and the final element N
  // times.
  std::sort(std::begin(Found), std::end(Found));
  for (auto F = std::begin(Found), E = std::end(Found); F != E; ++F)
    CHECK((F - Found + 1) == (*F));
}

// Verify that const pointers work for count and find even when the underlying
// SmallPtrSet is not for a const pointer type.
TEST_CASE("SmallPtrSetTest ConstTest", "[wpiutil][llvm]") {
  SmallPtrSet<int *, 8> IntSet;
  int A;
  int *B = &A;
  const int *C = &A;
  IntSet.insert(B);
  CHECK((IntSet.count(B)) == (1u));
  CHECK((IntSet.count(C)) == (1u));
  CHECK(IntSet.contains(B));
  CHECK(IntSet.contains(C));
}

// Verify that we automatically get the const version of PointerLikeTypeTraits
// filled in for us, even for a non-pointer type
using TestPair = PointerIntPair<int *, 1>;

TEST_CASE("SmallPtrSetTest ConstNonPtrTest", "[wpiutil][llvm]") {
  SmallPtrSet<TestPair, 8> IntSet;
  int A[1];
  TestPair Pair(&A[0], 1);
  IntSet.insert(Pair);
  CHECK((IntSet.count(Pair)) == (1u));
  CHECK(IntSet.contains(Pair));
}

// Test equality comparison.
TEST_CASE("SmallPtrSetTest EqualityComparison", "[wpiutil][llvm]") {
  int buf[3];
  for (int i = 0; i < 3; ++i)
    buf[i] = 0;

  SmallPtrSet<int *, 1> a;
  a.insert(&buf[0]);
  a.insert(&buf[1]);

  SmallPtrSet<int *, 2> b;
  b.insert(&buf[1]);
  b.insert(&buf[0]);

  SmallPtrSet<int *, 3> c;
  c.insert(&buf[1]);
  c.insert(&buf[2]);

  SmallPtrSet<int *, 4> d;
  d.insert(&buf[0]);

  SmallPtrSet<int *, 5> e;
  e.insert(&buf[0]);
  e.insert(&buf[1]);
  e.insert(&buf[2]);

  CHECK((a) == (b));
  CHECK((b) == (a));
  CHECK((b) != (c));
  CHECK((c) != (a));
  CHECK((d) != (a));
  CHECK((a) != (d));
  CHECK((a) != (e));
  CHECK((e) != (a));
  CHECK((c) != (e));
  CHECK((e) != (d));
}

TEST_CASE("SmallPtrSetTest Contains", "[wpiutil][llvm]") {
  SmallPtrSet<int *, 2> Set;
  int buf[4] = {0, 11, 22, 11};
  CHECK_FALSE(Set.contains(&buf[0]));
  CHECK_FALSE(Set.contains(&buf[1]));

  Set.insert(&buf[0]);
  Set.insert(&buf[1]);
  CHECK(Set.contains(&buf[0]));
  CHECK(Set.contains(&buf[1]));
  CHECK_FALSE(Set.contains(&buf[3]));

  Set.insert(&buf[1]);
  CHECK(Set.contains(&buf[0]));
  CHECK(Set.contains(&buf[1]));
  CHECK_FALSE(Set.contains(&buf[3]));

  Set.erase(&buf[1]);
  CHECK(Set.contains(&buf[0]));
  CHECK_FALSE(Set.contains(&buf[1]));

  Set.insert(&buf[1]);
  Set.insert(&buf[2]);
  CHECK(Set.contains(&buf[0]));
  CHECK(Set.contains(&buf[1]));
  CHECK(Set.contains(&buf[2]));
}

TEST_CASE("SmallPtrSetTest InsertIterator", "[wpiutil][llvm]") {
  SmallPtrSet<int *, 5> Set;
  int Vals[5] = {11, 22, 33, 44, 55};
  int *Buf[5] = {&Vals[0], &Vals[1], &Vals[2], &Vals[3], &Vals[4]};

  for (int *Ptr : Buf)
    Set.insert(Set.begin(), Ptr);

  // Ensure that all of the values were copied into the set.
  for (const auto *Ptr : Buf)
    CHECK(Set.contains(Ptr));
}

TEST_CASE("SmallPtrSetTest RemoveIf", "[wpiutil][llvm]") {
  SmallPtrSet<int *, 5> Set;
  int Vals[6] = {0, 1, 2, 3, 4, 5};

  // Stay in small regime.
  Set.insert(&Vals[0]);
  Set.insert(&Vals[1]);
  Set.insert(&Vals[2]);
  Set.insert(&Vals[3]);
  Set.erase(&Vals[0]); // Leave a tombstone.

  // Remove odd elements.
  bool Removed = Set.remove_if([](int *Ptr) { return *Ptr % 2 != 0; });
  // We should only have element 2 left now.
  CHECK(Removed);
  CHECK((Set.size()) == (1u));
  CHECK(Set.contains(&Vals[2]));

  // Switch to big regime.
  Set.insert(&Vals[0]);
  Set.insert(&Vals[1]);
  Set.insert(&Vals[3]);
  Set.insert(&Vals[4]);
  Set.insert(&Vals[5]);
  Set.erase(&Vals[0]); // Leave a tombstone.

  // Remove odd elements.
  Removed = Set.remove_if([](int *Ptr) { return *Ptr % 2 != 0; });
  // We should only have elements 2 and 4 left now.
  CHECK(Removed);
  CHECK((Set.size()) == (2u));
  CHECK(Set.contains(&Vals[2]));
  CHECK(Set.contains(&Vals[4]));

  Removed = Set.remove_if([](int *Ptr) { return false; });
  CHECK_FALSE(Removed);
}

TEST_CASE("SmallPtrSetTest CtorRange", "[wpiutil][llvm]") {
  int V0 = 0;
  int V1 = 1;
  int V2 = 2;
  int *Args[] = {&V2, &V0, &V1};
  SmallPtrSet<int *, 4> Set(wpi::util::from_range, Args);
  CHECK_THAT(Set, UnorderedRangeEquals({&V0, &V1, &V2}));
}

TEST_CASE("SmallPtrSetTest InsertRange", "[wpiutil][llvm]") {
  int V0 = 0;
  int V1 = 1;
  int V2 = 2;
  SmallPtrSet<int *, 4> Set;
  int *Args[] = {&V2, &V0, &V1};
  Set.insert_range(Args);
  CHECK_THAT(Set, UnorderedRangeEquals({&V0, &V1, &V2}));
}

TEST_CASE("SmallPtrSetTest Reserve", "[wpiutil][llvm]") {
  // Check that we don't do anything silly when using reserve().
  SmallPtrSet<int *, 4> Set;
  int Vals[8] = {0, 1, 2, 3, 4, 5, 6, 7};

  Set.insert(&Vals[0]);

  // We shouldn't reallocate when this happens.
  Set.reserve(4);
  CHECK((Set.capacity()) == (4u));

  Set.insert(&Vals[1]);
  Set.insert(&Vals[2]);
  Set.insert(&Vals[3]);

  // We shouldn't reallocate this time either.
  Set.reserve(4);
  CHECK((Set.capacity()) == (4u));
  CHECK((Set.size()) == (4u));
  CHECK_THAT(Set,
             UnorderedRangeEquals({&Vals[0], &Vals[1], &Vals[2], &Vals[3]}));

  // Reserving further should lead to a reallocation. And matching the existing
  // insertion approach, we immediately allocate up to 128 elements.
  Set.reserve(5);
  CHECK((Set.capacity()) == (128u));
  CHECK((Set.size()) == (4u));
  CHECK_THAT(Set,
             UnorderedRangeEquals({&Vals[0], &Vals[1], &Vals[2], &Vals[3]}));

  // And we should be able to insert another two or three elements without
  // reallocating.
  Set.insert(&Vals[4]);
  Set.insert(&Vals[5]);

  // Calling a smaller reserve size should have no effect.
  Set.reserve(1);
  CHECK((Set.capacity()) == (128u));
  CHECK((Set.size()) == (6u));

  // Reserving zero should have no effect either.
  Set.reserve(0);
  CHECK((Set.capacity()) == (128u));
  CHECK((Set.size()) == (6u));
  CHECK_THAT(Set, UnorderedRangeEquals({&Vals[0], &Vals[1], &Vals[2],
                                        &Vals[3], &Vals[4], &Vals[5]}));

  // Reserving 192 should result in 256 buckets.
  Set.reserve(192);
  CHECK((Set.capacity()) == (256u));
}
