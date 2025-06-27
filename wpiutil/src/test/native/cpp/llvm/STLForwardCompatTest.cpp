//===- STLForwardCompatTest.cpp - Unit tests for STLForwardCompat ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/STLForwardCompat.h"
#include "CountCopyAndMove.h"
#include "gtest/gtest.h"

namespace {

template <typename T>
class STLForwardCompatRemoveCVRefTest : public ::testing::Test {};

using STLForwardCompatRemoveCVRefTestTypes = ::testing::Types<
    // clang-format off
    std::pair<int, int>,
    std::pair<int &, int>,
    std::pair<const int, int>,
    std::pair<volatile int, int>,
    std::pair<const volatile int &, int>,
    std::pair<int *, int *>,
    std::pair<int *const, int *>,
    std::pair<const int *, const int *>,
    std::pair<int *&, int *>
    // clang-format on
    >;

TYPED_TEST_SUITE(STLForwardCompatRemoveCVRefTest,
                 STLForwardCompatRemoveCVRefTestTypes, );

TYPED_TEST(STLForwardCompatRemoveCVRefTest, RemoveCVRef) {
  using From = typename TypeParam::first_type;
  using To = typename TypeParam::second_type;
  EXPECT_TRUE(
      (std::is_same<typename wpi::remove_cvref<From>::type, To>::value));
}

TYPED_TEST(STLForwardCompatRemoveCVRefTest, RemoveCVRefT) {
  using From = typename TypeParam::first_type;
  EXPECT_TRUE((std::is_same<typename wpi::remove_cvref<From>::type,
                            wpi::remove_cvref_t<From>>::value));
}

TEST(TransformTest, TransformStd) {
  std::optional<int> A;

  std::optional<int> B = wpi::transformOptional(A, [&](int N) { return N + 1; });
  EXPECT_FALSE(B.has_value());

  A = 3;
  std::optional<int> C = wpi::transformOptional(A, [&](int N) { return N + 1; });
  EXPECT_TRUE(C.has_value());
  EXPECT_EQ(4, *C);
}

TEST(TransformTest, MoveTransformStd) {
  using wpi::CountCopyAndMove;

  std::optional<CountCopyAndMove> A;

  CountCopyAndMove::ResetCounts();
  std::optional<int> B = wpi::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  EXPECT_FALSE(B.has_value());
  EXPECT_EQ(0, CountCopyAndMove::TotalCopies());
  EXPECT_EQ(0, CountCopyAndMove::MoveConstructions);
  EXPECT_EQ(0, CountCopyAndMove::MoveAssignments);
  EXPECT_EQ(0, CountCopyAndMove::Destructions);

  A = CountCopyAndMove(5);
  CountCopyAndMove::ResetCounts();
  std::optional<int> C = wpi::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  EXPECT_TRUE(C.has_value());
  EXPECT_EQ(7, *C);
  EXPECT_EQ(0, CountCopyAndMove::TotalCopies());
  EXPECT_EQ(0, CountCopyAndMove::MoveConstructions);
  EXPECT_EQ(0, CountCopyAndMove::MoveAssignments);
  EXPECT_EQ(0, CountCopyAndMove::Destructions);
}

TEST(TransformTest, TransformLlvm) {
  std::optional<int> A;

  std::optional<int> B =
      wpi::transformOptional(A, [&](int N) { return N + 1; });
  EXPECT_FALSE(B.has_value());

  A = 3;
  std::optional<int> C =
      wpi::transformOptional(A, [&](int N) { return N + 1; });
  EXPECT_TRUE(C.has_value());
  EXPECT_EQ(4, *C);
}

TEST(TransformTest, MoveTransformLlvm) {
  using wpi::CountCopyAndMove;

  std::optional<CountCopyAndMove> A;

  CountCopyAndMove::ResetCounts();
  std::optional<int> B = wpi::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  EXPECT_FALSE(B.has_value());
  EXPECT_EQ(0, CountCopyAndMove::TotalCopies());
  EXPECT_EQ(0, CountCopyAndMove::MoveConstructions);
  EXPECT_EQ(0, CountCopyAndMove::MoveAssignments);
  EXPECT_EQ(0, CountCopyAndMove::Destructions);

  A = CountCopyAndMove(5);
  CountCopyAndMove::ResetCounts();
  std::optional<int> C = wpi::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  EXPECT_TRUE(C.has_value());
  EXPECT_EQ(7, *C);
  EXPECT_EQ(0, CountCopyAndMove::TotalCopies());
  EXPECT_EQ(0, CountCopyAndMove::MoveConstructions);
  EXPECT_EQ(0, CountCopyAndMove::MoveAssignments);
  EXPECT_EQ(0, CountCopyAndMove::Destructions);
}

TEST(TransformTest, ToUnderlying) {
  enum E { A1 = 0, B1 = -1 };
  static_assert(wpi::to_underlying(A1) == 0);
  static_assert(wpi::to_underlying(B1) == -1);

  enum E2 : unsigned char { A2 = 0, B2 };
  static_assert(
      std::is_same_v<unsigned char, decltype(wpi::to_underlying(A2))>);
  static_assert(wpi::to_underlying(A2) == 0);
  static_assert(wpi::to_underlying(B2) == 1);

  enum class E3 { A3 = -1, B3 };
  static_assert(std::is_same_v<int, decltype(wpi::to_underlying(E3::A3))>);
  static_assert(wpi::to_underlying(E3::A3) == -1);
  static_assert(wpi::to_underlying(E3::B3) == 0);
}

} // namespace
