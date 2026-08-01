//===- STLForwardCompatTest.cpp - Unit tests for STLForwardCompat ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/util/STLForwardCompat.hpp"
#include "CountCopyAndMove.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <optional>
#include <type_traits>
#include <utility>

namespace {

template <typename T>
class STLForwardCompatRemoveCVRefTest {};

#define WPIUTIL_TEST_TYPES_STLForwardCompatRemoveCVRefTest \
  (std::pair<int, int>), (std::pair<int&, int>), \
      (std::pair<const int, int>), (std::pair<volatile int, int>), \
      (std::pair<const volatile int&, int>), (std::pair<int*, int*>), \
      (std::pair<int* const, int*>), (std::pair<const int*, const int*>), \
      (std::pair<int*&, int*>)

TEMPLATE_TEST_CASE_METHOD(STLForwardCompatRemoveCVRefTest, "STLForwardCompatRemoveCVRefTest RemoveCVRef", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_STLForwardCompatRemoveCVRefTest) {
  using From = typename TestType::first_type;
  using To = typename TestType::second_type;
  CHECK((std::is_same<typename wpi::util::remove_cvref<From>::type, To>::value));
}

TEMPLATE_TEST_CASE_METHOD(STLForwardCompatRemoveCVRefTest, "STLForwardCompatRemoveCVRefTest RemoveCVRefT", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_STLForwardCompatRemoveCVRefTest) {
  using From = typename TestType::first_type;
  CHECK((std::is_same<typename wpi::util::remove_cvref<From>::type,
                            wpi::util::remove_cvref_t<From>>::value));
}

template <typename T> class TypeIdentityTest {
public:
  using TypeIdentity = wpi::util::type_identity<T>;
};

struct A {
  struct B {};
};
#define WPIUTIL_TEST_TYPES_TypeIdentityTest int, volatile int, A, const A::B

TEMPLATE_TEST_CASE_METHOD(TypeIdentityTest, "TypeIdentityTest Identity", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_TypeIdentityTest) {
  // TypeIdentityTest<TestType> is the instantiated TypeIdentityTest.
  CHECK((std::is_same_v<TestType, typename TypeIdentityTest<TestType>::TypeIdentity::type>));
}

TEST_CASE("TransformTest TransformStd", "[wpiutil][llvm]") {
  std::optional<int> A;

  std::optional<int> B = wpi::util::transformOptional(A, [&](int N) { return N + 1; });
  CHECK_FALSE(B.has_value());

  A = 3;
  std::optional<int> C = wpi::util::transformOptional(A, [&](int N) { return N + 1; });
  CHECK(C.has_value());
  CHECK((4) == (*C));
}

TEST_CASE("TransformTest MoveTransformStd", "[wpiutil][llvm]") {
  using wpi::util::CountCopyAndMove;

  std::optional<CountCopyAndMove> A;

  CountCopyAndMove::ResetCounts();
  std::optional<int> B = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK_FALSE(B.has_value());
  CHECK((0) == (CountCopyAndMove::TotalCopies()));
  CHECK((0) == (CountCopyAndMove::MoveConstructions));
  CHECK((0) == (CountCopyAndMove::MoveAssignments));
  CHECK((0) == (CountCopyAndMove::Destructions));

  A = CountCopyAndMove(5);
  CountCopyAndMove::ResetCounts();
  std::optional<int> C = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK(C.has_value());
  CHECK((7) == (*C));
  CHECK((0) == (CountCopyAndMove::TotalCopies()));
  CHECK((0) == (CountCopyAndMove::MoveConstructions));
  CHECK((0) == (CountCopyAndMove::MoveAssignments));
  CHECK((0) == (CountCopyAndMove::Destructions));
}

TEST_CASE("TransformTest TransformLlvm", "[wpiutil][llvm]") {
  std::optional<int> A;

  std::optional<int> B =
      wpi::util::transformOptional(A, [&](int N) { return N + 1; });
  CHECK_FALSE(B.has_value());

  A = 3;
  std::optional<int> C =
      wpi::util::transformOptional(A, [&](int N) { return N + 1; });
  CHECK(C.has_value());
  CHECK((4) == (*C));
}

TEST_CASE("TransformTest MoveTransformLlvm", "[wpiutil][llvm]") {
  using wpi::util::CountCopyAndMove;

  std::optional<CountCopyAndMove> A;

  CountCopyAndMove::ResetCounts();
  std::optional<int> B = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK_FALSE(B.has_value());
  CHECK((0) == (CountCopyAndMove::TotalCopies()));
  CHECK((0) == (CountCopyAndMove::MoveConstructions));
  CHECK((0) == (CountCopyAndMove::MoveAssignments));
  CHECK((0) == (CountCopyAndMove::Destructions));

  A = CountCopyAndMove(5);
  CountCopyAndMove::ResetCounts();
  std::optional<int> C = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK(C.has_value());
  CHECK((7) == (*C));
  CHECK((0) == (CountCopyAndMove::TotalCopies()));
  CHECK((0) == (CountCopyAndMove::MoveConstructions));
  CHECK((0) == (CountCopyAndMove::MoveAssignments));
  CHECK((0) == (CountCopyAndMove::Destructions));
}

TEST_CASE("TransformTest TransformCategory", "[wpiutil][llvm]") {
  struct StructA {
    int x;
  };
  struct StructB : StructA {
    StructB(StructA &&A) : StructA(std::move(A)) {}
  };

  std::optional<StructA> A{StructA{}};
  wpi::util::transformOptional(A, [](auto &&s) {
    CHECK_FALSE(std::is_rvalue_reference_v<decltype(s)>);
    return StructB{std::move(s)};
  });

  wpi::util::transformOptional(std::move(A), [](auto &&s) {
    CHECK(std::is_rvalue_reference_v<decltype(s)>);
    return StructB{std::move(s)};
  });
}

TEST_CASE("TransformTest ToUnderlying", "[wpiutil][llvm]") {
  enum E { A1 = 0, B1 = -1 };
  static_assert(wpi::util::to_underlying(A1) == 0);
  static_assert(wpi::util::to_underlying(B1) == -1);

  enum E2 : unsigned char { A2 = 0, B2 };
  static_assert(
      std::is_same_v<unsigned char, decltype(wpi::util::to_underlying(A2))>);
  static_assert(wpi::util::to_underlying(A2) == 0);
  static_assert(wpi::util::to_underlying(B2) == 1);

  enum class E3 { A3 = -1, B3 };
  static_assert(std::is_same_v<int, decltype(wpi::util::to_underlying(E3::A3))>);
  static_assert(wpi::util::to_underlying(E3::A3) == -1);
  static_assert(wpi::util::to_underlying(E3::B3) == 0);
}

TEST_CASE("STLForwardCompatTest IdentityCxx20", "[wpiutil][llvm]") {
  wpi::util::identity identity;

  // Test with an lvalue.
  int X = 42;
  int &Y = identity(X);
  CHECK((&X) == (&Y));

  // Test with a const lvalue.
  const int CX = 10;
  const int &CY = identity(CX);
  CHECK((&CX) == (&CY));

  // Test with an rvalue.
  CHECK((identity(123)) == (123));

  // Test perfect forwarding.
  static_assert(std::is_same_v<int &, decltype(identity(X))>);
  static_assert(std::is_same_v<const int &, decltype(identity(CX))>);
  static_assert(std::is_same_v<int &&, decltype(identity(int(5)))>);
}

} // namespace
