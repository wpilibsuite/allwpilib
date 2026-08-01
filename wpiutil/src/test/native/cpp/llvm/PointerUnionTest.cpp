//===- llvm/unittest/ADT/PointerUnionTest.cpp - Optional unit tests -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/PointerUnion.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
using namespace wpi::util;

namespace {

using PU = PointerUnion<int *, float *>;
using PU3 = PointerUnion<int *, float *, long long *>;
using PU4 = PointerUnion<int *, float *, long long *, double *>;

struct PointerUnionTest {
  float f;
  int i;
  double d;
  long long l;

  PU a, b, c, n;
  PU3 i3, f3, l3;
  PU4 i4, f4, l4, d4;
  PU4 i4null, f4null, l4null, d4null;

  PointerUnionTest()
      : f(3.14f), i(42), d(3.14), l(42), a(&f), b(&i), c(&i), n(), i3(&i),
        f3(&f), l3(&l), i4(&i), f4(&f), l4(&l), d4(&d), i4null((int *)nullptr),
        f4null((float *)nullptr), l4null((long long *)nullptr),
        d4null((double *)nullptr) {}
};

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest Comparison", "[wpiutil][llvm]") {
  CHECK(a == a);
  CHECK_FALSE(a != a);
  CHECK(a != b);
  CHECK_FALSE(a == b);
  CHECK(b == c);
  CHECK_FALSE(b != c);
  CHECK(b != n);
  CHECK_FALSE(b == n);
  CHECK(i3 == i3);
  CHECK_FALSE(i3 != i3);
  CHECK(i3 != f3);
  CHECK(f3 != l3);
  CHECK(i4 == i4);
  CHECK_FALSE(i4 != i4);
  CHECK(i4 != f4);
  CHECK(i4 != l4);
  CHECK(f4 != l4);
  CHECK(l4 != d4);
  CHECK(i4null != f4null);
  CHECK(i4null != l4null);
  CHECK(i4null != d4null);
}

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest Null", "[wpiutil][llvm]") {
  CHECK_FALSE(a.isNull());
  CHECK_FALSE(b.isNull());
  CHECK(n.isNull());
  CHECK_FALSE(!a);
  CHECK_FALSE(!b);
  CHECK(!n);
  // workaround an issue with EXPECT macros and explicit bool
  CHECK((bool)a);
  CHECK((bool)b);
  CHECK_FALSE(n);

  CHECK((n) != (b));
  CHECK((b) == (c));
  b = nullptr;
  CHECK((n) == (b));
  CHECK((b) != (c));
  CHECK_FALSE(i3.isNull());
  CHECK_FALSE(f3.isNull());
  CHECK_FALSE(l3.isNull());
  CHECK_FALSE(i4.isNull());
  CHECK_FALSE(f4.isNull());
  CHECK_FALSE(l4.isNull());
  CHECK_FALSE(d4.isNull());
  CHECK(i4null.isNull());
  CHECK(f4null.isNull());
  CHECK(l4null.isNull());
  CHECK(d4null.isNull());
}

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest Is", "[wpiutil][llvm]") {
  CHECK_FALSE(isa<int *>(a));
  CHECK(isa<float *>(a));
  CHECK(isa<int *>(b));
  CHECK_FALSE(isa<float *>(b));
  CHECK(isa<int *>(n));
  CHECK_FALSE(isa<float *>(n));
  CHECK(isa<int *>(i3));
  CHECK(isa<float *>(f3));
  CHECK(isa<long long *>(l3));
  CHECK(isa<int *>(i4));
  CHECK(isa<float *>(f4));
  CHECK(isa<long long *>(l4));
  CHECK(isa<double *>(d4));
  CHECK(isa<int *>(i4null));
  CHECK(isa<float *>(f4null));
  CHECK(isa<long long *>(l4null));
  CHECK(isa<double *>(d4null));
}

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest Get", "[wpiutil][llvm]") {
  CHECK((cast<float *>(a)) == (&f));
  CHECK((cast<int *>(b)) == (&i));
  CHECK((cast<int *>(n)) == ((int *)nullptr));
}

template<int I> struct alignas(8) Aligned {};

using PU8 =
    PointerUnion<Aligned<0> *, Aligned<1> *, Aligned<2> *, Aligned<3> *,
                 Aligned<4> *, Aligned<5> *, Aligned<6> *, Aligned<7> *>;

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest ManyElements", "[wpiutil][llvm]") {
  Aligned<0> a0;
  Aligned<7> a7;

  PU8 a = &a0;
  CHECK(isa<Aligned<0> *>(a));
  CHECK_FALSE(isa<Aligned<1> *>(a));
  CHECK_FALSE(isa<Aligned<2> *>(a));
  CHECK_FALSE(isa<Aligned<3> *>(a));
  CHECK_FALSE(isa<Aligned<4> *>(a));
  CHECK_FALSE(isa<Aligned<5> *>(a));
  CHECK_FALSE(isa<Aligned<6> *>(a));
  CHECK_FALSE(isa<Aligned<7> *>(a));
  CHECK((dyn_cast_if_present<Aligned<0> *>(a)) == (&a0));
  CHECK((*a.getAddrOfPtr1()) == (&a0));

  a = &a7;
  CHECK_FALSE(isa<Aligned<0> *>(a));
  CHECK_FALSE(isa<Aligned<1> *>(a));
  CHECK_FALSE(isa<Aligned<2> *>(a));
  CHECK_FALSE(isa<Aligned<3> *>(a));
  CHECK_FALSE(isa<Aligned<4> *>(a));
  CHECK_FALSE(isa<Aligned<5> *>(a));
  CHECK_FALSE(isa<Aligned<6> *>(a));
  CHECK(isa<Aligned<7> *>(a));
  CHECK((dyn_cast_if_present<Aligned<7> *>(a)) == (&a7));

  CHECK(a == PU8(&a7));
  CHECK(a != PU8(&a0));
}

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest GetAddrOfPtr1", "[wpiutil][llvm]") {
  CHECK((void *)b.getAddrOfPtr1() == (void *)&b);
  CHECK((void *)n.getAddrOfPtr1() == (void *)&n);
}

TEST_CASE_METHOD(PointerUnionTest, "PointerUnionTest NewCastInfra", "[wpiutil][llvm]") {
  // test isa<>
  CHECK(isa<float *>(a));
  CHECK(isa<int *>(b));
  CHECK(isa<int *>(c));
  CHECK(isa<int *>(n));
  CHECK(isa<int *>(i3));
  CHECK(isa<float *>(f3));
  CHECK(isa<long long *>(l3));
  CHECK(isa<int *>(i4));
  CHECK(isa<float *>(f4));
  CHECK(isa<long long *>(l4));
  CHECK(isa<double *>(d4));
  CHECK(isa<int *>(i4null));
  CHECK(isa<float *>(f4null));
  CHECK(isa<long long *>(l4null));
  CHECK(isa<double *>(d4null));
  CHECK_FALSE(isa<int *>(a));
  CHECK_FALSE(isa<float *>(b));
  CHECK_FALSE(isa<float *>(c));
  CHECK_FALSE(isa<float *>(n));
  CHECK_FALSE(isa<float *>(i3));
  CHECK_FALSE(isa<long long *>(i3));
  CHECK_FALSE(isa<int *>(f3));
  CHECK_FALSE(isa<long long *>(f3));
  CHECK_FALSE(isa<int *>(l3));
  CHECK_FALSE(isa<float *>(l3));
  CHECK_FALSE(isa<float *>(i4));
  CHECK_FALSE(isa<long long *>(i4));
  CHECK_FALSE(isa<double *>(i4));
  CHECK_FALSE(isa<int *>(f4));
  CHECK_FALSE(isa<long long *>(f4));
  CHECK_FALSE(isa<double *>(f4));
  CHECK_FALSE(isa<int *>(l4));
  CHECK_FALSE(isa<float *>(l4));
  CHECK_FALSE(isa<double *>(l4));
  CHECK_FALSE(isa<int *>(d4));
  CHECK_FALSE(isa<float *>(d4));
  CHECK_FALSE(isa<long long *>(d4));
  CHECK_FALSE(isa<float *>(i4null));
  CHECK_FALSE(isa<long long *>(i4null));
  CHECK_FALSE(isa<double *>(i4null));
  CHECK_FALSE(isa<int *>(f4null));
  CHECK_FALSE(isa<long long *>(f4null));
  CHECK_FALSE(isa<double *>(f4null));
  CHECK_FALSE(isa<int *>(l4null));
  CHECK_FALSE(isa<float *>(l4null));
  CHECK_FALSE(isa<double *>(l4null));
  CHECK_FALSE(isa<int *>(d4null));
  CHECK_FALSE(isa<float *>(d4null));
  CHECK_FALSE(isa<long long *>(d4null));

  // test cast<>
  CHECK((cast<float *>(a)) == (&f));
  CHECK((cast<int *>(b)) == (&i));
  CHECK((cast<int *>(c)) == (&i));
  CHECK((cast<int *>(i3)) == (&i));
  CHECK((cast<float *>(f3)) == (&f));
  CHECK((cast<long long *>(l3)) == (&l));
  CHECK((cast<int *>(i4)) == (&i));
  CHECK((cast<float *>(f4)) == (&f));
  CHECK((cast<long long *>(l4)) == (&l));
  CHECK((cast<double *>(d4)) == (&d));

  // test dyn_cast
  CHECK((dyn_cast<int *>(a)) == (nullptr));
  CHECK((dyn_cast<float *>(a)) == (&f));
  CHECK((dyn_cast<int *>(b)) == (&i));
  CHECK((dyn_cast<float *>(b)) == (nullptr));
  CHECK((dyn_cast<int *>(c)) == (&i));
  CHECK((dyn_cast<float *>(c)) == (nullptr));
  CHECK((dyn_cast_if_present<int *>(n)) == (nullptr));
  CHECK((dyn_cast_if_present<float *>(n)) == (nullptr));
  CHECK((dyn_cast<int *>(i3)) == (&i));
  CHECK((dyn_cast<float *>(i3)) == (nullptr));
  CHECK((dyn_cast<long long *>(i3)) == (nullptr));
  CHECK((dyn_cast<int *>(f3)) == (nullptr));
  CHECK((dyn_cast<float *>(f3)) == (&f));
  CHECK((dyn_cast<long long *>(f3)) == (nullptr));
  CHECK((dyn_cast<int *>(l3)) == (nullptr));
  CHECK((dyn_cast<float *>(l3)) == (nullptr));
  CHECK((dyn_cast<long long *>(l3)) == (&l));
  CHECK((dyn_cast<int *>(i4)) == (&i));
  CHECK((dyn_cast<float *>(i4)) == (nullptr));
  CHECK((dyn_cast<long long *>(i4)) == (nullptr));
  CHECK((dyn_cast<double *>(i4)) == (nullptr));
  CHECK((dyn_cast<int *>(f4)) == (nullptr));
  CHECK((dyn_cast<float *>(f4)) == (&f));
  CHECK((dyn_cast<long long *>(f4)) == (nullptr));
  CHECK((dyn_cast<double *>(f4)) == (nullptr));
  CHECK((dyn_cast<int *>(l4)) == (nullptr));
  CHECK((dyn_cast<float *>(l4)) == (nullptr));
  CHECK((dyn_cast<long long *>(l4)) == (&l));
  CHECK((dyn_cast<double *>(l4)) == (nullptr));
  CHECK((dyn_cast<int *>(d4)) == (nullptr));
  CHECK((dyn_cast<float *>(d4)) == (nullptr));
  CHECK((dyn_cast<long long *>(d4)) == (nullptr));
  CHECK((dyn_cast<double *>(d4)) == (&d));
  CHECK((dyn_cast_if_present<int *>(i4null)) == (nullptr));
  CHECK((dyn_cast_if_present<float *>(i4null)) == (nullptr));
  CHECK((dyn_cast_if_present<long long *>(i4null)) == (nullptr));
  CHECK((dyn_cast_if_present<double *>(i4null)) == (nullptr));
  CHECK((dyn_cast_if_present<int *>(f4null)) == (nullptr));
  CHECK((dyn_cast_if_present<float *>(f4null)) == (nullptr));
  CHECK((dyn_cast_if_present<long long *>(f4null)) == (nullptr));
  CHECK((dyn_cast_if_present<double *>(f4null)) == (nullptr));
  CHECK((dyn_cast_if_present<int *>(l4null)) == (nullptr));
  CHECK((dyn_cast_if_present<float *>(l4null)) == (nullptr));
  CHECK((dyn_cast_if_present<long long *>(l4null)) == (nullptr));
  CHECK((dyn_cast_if_present<double *>(l4null)) == (nullptr));
  CHECK((dyn_cast_if_present<int *>(d4null)) == (nullptr));
  CHECK((dyn_cast_if_present<float *>(d4null)) == (nullptr));
  CHECK((dyn_cast_if_present<long long *>(d4null)) == (nullptr));
  CHECK((dyn_cast_if_present<double *>(d4null)) == (nullptr));

  // test for const
  const PU4 constd4(&d);
  CHECK(isa<double *>(constd4));
  CHECK_FALSE(isa<int *>(constd4));
  CHECK((cast<double *>(constd4)) == (&d));
  CHECK((dyn_cast<long long *>(constd4)) == (nullptr));

  auto *result1 = cast<double *>(constd4);
  static_assert(std::is_same_v<double *, decltype(result1)>,
                "type mismatch for cast with PointerUnion");

  PointerUnion<int *, const double *> constd2(&d);
  auto *result2 = cast<const double *>(constd2);
  CHECK((result2) == (&d));
  static_assert(std::is_same_v<const double *, decltype(result2)>,
                "type mismatch for cast with PointerUnion");
}

} // end anonymous namespace
