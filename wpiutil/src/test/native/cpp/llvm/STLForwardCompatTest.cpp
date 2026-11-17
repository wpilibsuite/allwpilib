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

#include <iterator>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace wpi::util {
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
  CHECK(std::is_same < typename wpi::util::remove_cvref<From>::type, To>::value);
}

TEMPLATE_TEST_CASE_METHOD(STLForwardCompatRemoveCVRefTest, "STLForwardCompatRemoveCVRefTest RemoveCVRefT", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_STLForwardCompatRemoveCVRefTest) {
  using From = typename TestType::first_type;
  CHECK(std::is_same < typename wpi::util::remove_cvref<From>::type,
                            wpi::util::remove_cvref_t<From>>::value);
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
  CHECK(std::is_same_v < TestType, typename TypeIdentityTest<TestType>::TypeIdentity::type>);
}

TEST_CASE("TransformTest TransformStd", "[wpiutil][llvm]") {
  std::optional<int> A;

  std::optional<int> B = wpi::util::transformOptional(A, [&](int N) { return N + 1; });
  CHECK_FALSE(B.has_value());

  A = 3;
  std::optional<int> C = wpi::util::transformOptional(A, [&](int N) { return N + 1; });
  CHECK(C.has_value());
  CHECK(4 == *C);
}

TEST_CASE("TransformTest MoveTransformStd", "[wpiutil][llvm]") {
  using wpi::util::CountCopyAndMove;

  std::optional<CountCopyAndMove> A;

  CountCopyAndMove::ResetCounts();
  std::optional<int> B = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK_FALSE(B.has_value());
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(0 == CountCopyAndMove::MoveAssignments);
  CHECK(0 == CountCopyAndMove::Destructions);

  A = CountCopyAndMove(5);
  CountCopyAndMove::ResetCounts();
  std::optional<int> C = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK(C.has_value());
  CHECK(7 == *C);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(0 == CountCopyAndMove::MoveAssignments);
  CHECK(0 == CountCopyAndMove::Destructions);
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
  CHECK(4 == *C);
}

TEST_CASE("TransformTest MoveTransformLlvm", "[wpiutil][llvm]") {
  using wpi::util::CountCopyAndMove;

  std::optional<CountCopyAndMove> A;

  CountCopyAndMove::ResetCounts();
  std::optional<int> B = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK_FALSE(B.has_value());
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(0 == CountCopyAndMove::MoveAssignments);
  CHECK(0 == CountCopyAndMove::Destructions);

  A = CountCopyAndMove(5);
  CountCopyAndMove::ResetCounts();
  std::optional<int> C = wpi::util::transformOptional(
      std::move(A), [&](const CountCopyAndMove &M) { return M.val + 2; });
  CHECK(C.has_value());
  CHECK(7 == *C);
  CHECK(0 == CountCopyAndMove::TotalCopies());
  CHECK(0 == CountCopyAndMove::MoveConstructions);
  CHECK(0 == CountCopyAndMove::MoveAssignments);
  CHECK(0 == CountCopyAndMove::Destructions);
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

// Checks that pointer to member is a valid argument
TEST_CASE("TransformTest TransformToMember", "[wpiutil][llvm]") {
  struct StructA {
    int x = 1;
    int y = 2;
  };
  std::optional<StructA> Opt{StructA{}};
  auto OptX = transformOptional(Opt, &StructA::x);
  REQUIRE(OptX.has_value());
  CHECK(*OptX == 1);

  auto OptY = transformOptional(Opt, &StructA::y);
  REQUIRE(OptY.has_value());
  CHECK(*OptY == 2);

  Opt = std::nullopt;
  OptX = transformOptional(Opt, &StructA::x);
  CHECK_FALSE(OptX.has_value());
}

// Checks that pointer to member function is a valid argument
TEST_CASE("TransformTest TransformToMemberFunc", "[wpiutil][llvm]") {
  struct Foo {
    int bar() const { return 42; }
  };
  std::optional<Foo> Opt{Foo{}};
  auto OptRes = transformOptional(Opt, &Foo::bar);
  REQUIRE(OptRes.has_value());
  CHECK(*OptRes == 42);
}

// Checks that callable is forwarded properly
TEST_CASE("TransformTest TransformRvalueRef", "[wpiutil][llvm]") {
  struct Foo {
    int operator()(int x) && { return 42; }
    int operator()(int x) & { return 43; }
  };
  std::optional<int> Opt = 1;
  Foo foo;
  auto Res = transformOptional(Opt, foo);
  CHECK(Res.value_or(0) == 43);
  Res = transformOptional(Opt, std::move(foo));
  CHECK(Res.value_or(0) == 42);
}

// Checks constexpr
TEST_CASE("TransformTest TransfornConstexpr", "[wpiutil][llvm]") {
  constexpr std::optional<int> Opt1 = 42;
  auto PlusOne = [](int x) { return x + 1; };
  constexpr auto Res1 = transformOptional(Opt1, PlusOne);
  CHECK(Res1.value_or(0) == 43);
  constexpr std::optional<int> Opt2;
  constexpr auto Res2 = transformOptional(Opt2, PlusOne);
  CHECK_FALSE(Res2.has_value());
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
  CHECK(&X == &Y);

  // Test with a const lvalue.
  const int CX = 10;
  const int &CY = identity(CX);
  CHECK(&CX == &CY);

  // Test with an rvalue.
  CHECK(identity(123) == 123);

  // Test perfect forwarding.
  static_assert(std::is_same_v<int &, decltype(identity(X))>);
  static_assert(std::is_same_v<const int &, decltype(identity(CX))>);
  static_assert(std::is_same_v<int &&, decltype(identity(int(5)))>);
}

//===----------------------------------------------------------------------===//
// wpi::util::invoke tests
//===----------------------------------------------------------------------===//

TEST_CASE("STLForwardCompatTest InvokePerfectForwarding", "[wpiutil][llvm]") {
  auto CheckArgs = [](auto &&A, auto &&B, auto &&C) {
    static_assert(std::is_same_v<decltype(A), int &>);
    static_assert(std::is_same_v<decltype(B), const int &>);
    static_assert(std::is_same_v<decltype(C), int &&>);
    return A + B + C;
  };

  int X = 1;
  const int Y = 2;
  CHECK(wpi::util::invoke(CheckArgs, X, Y, 3) == 6);
  // Make sure the result produced by std::invoke is identical --
  // the only meaningful difference is that std::invoke became
  // constexpr in C++20.
  CHECK(std::invoke(CheckArgs, X, Y, 3) == 6);
}

namespace {
struct InvokeTest {
  int Value;
  constexpr int scale(int Factor) const { return Value * Factor; }
  constexpr int &getRef() { return Value; }
};
} // namespace

TEST_CASE("STLForwardCompatTest InvokeMemberPointers", "[wpiutil][llvm]") {
  InvokeTest Obj{10};

  // Member function pointer.
  CHECK(wpi::util::invoke(&InvokeTest::scale, Obj, 3) == 30);
  CHECK(wpi::util::invoke(&InvokeTest::scale, &Obj, 3) == 30);

  // Member data pointer.
  CHECK(wpi::util::invoke(&InvokeTest::Value, Obj) == 10);
  CHECK(wpi::util::invoke(&InvokeTest::Value, &Obj) == 10);

  // Member function returning reference - args are forwarded, not copied.
  wpi::util::invoke(&InvokeTest::getRef, Obj) = 20;
  CHECK(Obj.Value == 20); // Lvalue forwarded by reference.
  wpi::util::invoke(&InvokeTest::getRef, &Obj) = 30;
  CHECK(Obj.Value == 30); // Pointer also works.
}

TEST_CASE("STLForwardCompatTest InvokeConstexpr", "[wpiutil][llvm]") {
  // Regular function.
  static constexpr int A =
      wpi::util::invoke([](int X, int Y) { return X + Y; }, 1, 2);
  static_assert(A == 3);

  // Member data pointer.
  static constexpr int B = wpi::util::invoke(&InvokeTest::Value, InvokeTest{42});
  static_assert(B == 42);

  // Member function pointer.
  static constexpr int C = wpi::util::invoke(&InvokeTest::scale, InvokeTest{5}, 3);
  static_assert(C == 15);
}

TEST_CASE("STLForwardCompatTest BindFrontReferences", "[wpiutil][llvm]") {
  // All bound arguments are forwarded (for ints, this is a copy) into the
  // wrapper. Call arguments are forwarded with their original value category.
  int A = 1;
  const int B = 2;
  int C = 3;
  int D = 4;
  const int E = 5;
  int F = 6;

  auto TestTypes = [](auto &&AArg, auto &&BArg, auto &&CArg, auto &&DArg,
                      auto &&EArg, auto &&FArg) {
    // Bound args: all stored as values, passed as lvalue refs.
    CHECK(AArg == 1);
    static_assert(std::is_same_v<decltype(AArg), int &>);
    CHECK(BArg == 2);
    static_assert(std::is_same_v<decltype(BArg), int &>); // Const decayed away.
    CHECK(CArg == 3);
    static_assert(std::is_same_v<decltype(CArg), int &>);
    // Call args: forwarded with original value category.
    CHECK(DArg == 4);
    static_assert(std::is_same_v<decltype(DArg), int &>);
    CHECK(EArg == 5);
    static_assert(std::is_same_v<decltype(EArg), const int &>);
    CHECK(FArg == 6);
    static_assert(std::is_same_v<decltype(FArg), int &&>);

    ++AArg;
    ++DArg;
  };

  wpi::util::bind_front(TestTypes, A, B, std::move(C))(D, E, std::move(F));
  CHECK(A == 1); // A was copied, original unchanged.
  CHECK(D == 5); // D was passed by reference and incremented.
}

TEST_CASE("STLForwardCompatTest BindBackReferences", "[wpiutil][llvm]") {
  // With std::decay_t, all bound arguments are copied into the wrapper.
  // Call arguments are forwarded with their original value category.
  int A = 1;
  const int B = 2;
  int C = 3;
  int D = 4;
  const int E = 5;
  int F = 6;

  auto TestTypes = [](auto &&AArg, auto &&BArg, auto &&CArg, auto &&DArg,
                      auto &&EArg, auto &&FArg) {
    // Call args: forwarded with original value category.
    CHECK(AArg == 1);
    static_assert(std::is_same_v<decltype(AArg), int &>);
    CHECK(BArg == 2);
    static_assert(std::is_same_v<decltype(BArg), const int &>);
    CHECK(CArg == 3);
    static_assert(std::is_same_v<decltype(CArg), int &&>);
    // Bound args: all stored as values, passed as lvalue refs.
    CHECK(DArg == 4);
    static_assert(std::is_same_v<decltype(DArg), int &>);
    CHECK(EArg == 5);
    static_assert(std::is_same_v<decltype(EArg), int &>); // Const decayed away.
    CHECK(FArg == 6);
    static_assert(std::is_same_v<decltype(FArg), int &>);

    ++AArg;
    ++DArg;
  };

  wpi::util::bind_back(TestTypes, D, E, std::move(F))(A, B, std::move(C));
  CHECK(A == 2); // A was passed by reference and incremented.
  CHECK(D == 4); // D was copied, original unchanged.
}

// Check that bound args are copied once during bind, then passed by reference.
TEST_CASE("STLForwardCompatTest BindBoundArgsForwarding", "[wpiutil][llvm]") {
  auto Fn = [](CountCopyAndMove &A) -> int { return A.val; };

  CountCopyAndMove::ResetCounts();
  CountCopyAndMove Arg(42);
  CHECK(CountCopyAndMove::TotalCopies() == 0);
  CHECK(CountCopyAndMove::TotalMoves() == 0);

  // Creating the wrapper should copy the bound args once.
  auto Bound = wpi::util::bind_front(Fn, Arg);
  CHECK(CountCopyAndMove::TotalCopies() == 1);
  CHECK(CountCopyAndMove::TotalMoves() == 0);

  // Calling should not copy -- bound args are passed by reference.
  CHECK(Bound() == 42);
  CHECK(CountCopyAndMove::TotalCopies() == 1);
  CHECK(CountCopyAndMove::TotalMoves() == 0);
}

// Check that call args are forwarded without copies.
TEST_CASE("STLForwardCompatTest BindCallArgsForwarding", "[wpiutil][llvm]") {
  auto Fn = [](int, CountCopyAndMove &&A) -> int { return A.val; };

  CountCopyAndMove::ResetCounts();
  auto Bound = wpi::util::bind_front(Fn, 1);
  CHECK(CountCopyAndMove::TotalCopies() == 0);
  CHECK(CountCopyAndMove::TotalMoves() == 0);

  // Call arg should be forwarded as rvalue, no copies.
  CHECK(Bound(CountCopyAndMove(42)) == 42);
  CHECK(CountCopyAndMove::TotalCopies() == 0);
  CHECK(CountCopyAndMove::TotalMoves() == 0);
}

// Check that the callable itself is moved, not copied excessively.
TEST_CASE("STLForwardCompatTest BindCallableForwarding", "[wpiutil][llvm]") {
  CountCopyAndMove::ResetCounts();
  CountCopyAndMove Capture(42);
  CHECK(CountCopyAndMove::TotalCopies() == 0);

  // Lambda captures by value -- this copy is outside bind's control.
  auto Fn = [Capture]() { return Capture.val; };
  CHECK(CountCopyAndMove::TotalCopies() == 1);
  CHECK(CountCopyAndMove::TotalMoves() == 0);

  // Moving lambda into bind should move, not copy.
  auto Bound = wpi::util::bind_front(std::move(Fn));
  CHECK(CountCopyAndMove::TotalCopies() == 1);
  CHECK(CountCopyAndMove::TotalMoves() == 1);

  // Calling should not copy the callable.
  CHECK(Bound() == 42);
  CHECK(CountCopyAndMove::TotalCopies() == 1);
  CHECK(CountCopyAndMove::TotalMoves() == 1);

  // The bind object should be copyable.
  auto BoundCopy = Bound;
  CHECK(CountCopyAndMove::TotalCopies() == 2);

  // The bind object should be movable.
  auto BoundMove = std::move(BoundCopy);
  CHECK(CountCopyAndMove::TotalCopies() == 2);
  CHECK(CountCopyAndMove::TotalMoves() == 2);
}

// Check that moving bound args works correctly.
TEST_CASE("STLForwardCompatTest BindMoveBoundArgs", "[wpiutil][llvm]") {
  auto Fn = [](CountCopyAndMove &A) -> int { return A.val; };

  CountCopyAndMove::ResetCounts();
  CountCopyAndMove Arg(42);

  // Moving into bind should move, not copy.
  auto Bound = wpi::util::bind_front(Fn, std::move(Arg));
  CHECK(CountCopyAndMove::TotalCopies() == 0);
  CHECK(CountCopyAndMove::TotalMoves() == 1);

  CHECK(Bound() == 42);
  CHECK(CountCopyAndMove::TotalCopies() == 0);
  CHECK(CountCopyAndMove::TotalMoves() == 1);
}

TEST_CASE("STLForwardCompatTest BindFrontMutableStorage", "[wpiutil][llvm]") {
  // With std::decay_t, A is copied into the wrapper. The stored copy can be
  // mutated across calls, but the original A is unchanged.
  int A = 1;

  auto TestMutation = [](int &AArg, int &BArg, auto ExtraCheckFn) {
    ++AArg;
    ++BArg;
    ExtraCheckFn(AArg, BArg);
  };

  auto BoundA = wpi::util::bind_front(TestMutation, A, 42);
  BoundA([](int AVal, int BVal) {
    CHECK(AVal == 2); // Stored copy incremented from 1.
    CHECK(BVal == 43);
  });
  CHECK(A == 1); // Original unchanged.

  BoundA([](int AVal, int BVal) {
    CHECK(AVal == 3); // Stored copy incremented again.
    CHECK(BVal == 44);
  });
  CHECK(A == 1); // Original still unchanged.
}

TEST_CASE("STLForwardCompatTest BindBackMutableStorage", "[wpiutil][llvm]") {
  // With std::decay_t, A is copied into the wrapper. The stored copy can be
  // mutated across calls, but the original A is unchanged.
  int A = 1;

  auto TestMutation = [](auto ExtraCheckFn, int &AArg, int &BArg) {
    ++AArg;
    ++BArg;
    ExtraCheckFn(AArg, BArg);
  };

  auto BoundA = wpi::util::bind_back(TestMutation, A, 42);
  BoundA([](int AVal, int BVal) {
    CHECK(AVal == 2); // Stored copy incremented from 1.
    CHECK(BVal == 43);
  });
  CHECK(A == 1); // Original unchanged.

  BoundA([](int AVal, int BVal) {
    CHECK(AVal == 3); // Stored copy incremented again.
    CHECK(BVal == 44);
  });
  CHECK(A == 1); // Original still unchanged.
}

// Free function for compile-time bind tests.
static int subtract(int A, int B) { return A - B; }

TEST_CASE("STLForwardCompatTest BindFrontConstexprCallable", "[wpiutil][llvm]") {
  // Test compile-time callable with wpi::util::bind_front.
  auto TimesFive = wpi::util::bind_front<subtract>(5);
  CHECK(TimesFive(3) == 2);

  // Test compile-time callable with wpi::util::bind_back.
  auto FiveTimesX = wpi::util::bind_back<subtract>(5);
  CHECK(FiveTimesX(3) == -2);
}

TEST_CASE("STLForwardCompatTest BindFrontBackNoBoundArgs", "[wpiutil][llvm]") {
  auto Fn1 = wpi::util::bind_front([](int A, int B) { return A + B; });
  CHECK(Fn1(3, 4) == 7);
  auto Fn2 = wpi::util::bind_back([](int A, int B) { return A + B; });
  CHECK(Fn2(3, 4) == 7);
}

TEST_CASE("STLForwardCompatTest BindFrontBindBackConstexpr", "[wpiutil][llvm]") {
  static constexpr auto Fn1 =
      wpi::util::bind_front([](int A, int B) { return A + B; }, 1);
  static_assert(Fn1(3) == 4);
  static constexpr auto Fn2 =
      wpi::util::bind_back([](int A, int B) { return A + B; }, 1);
  static_assert(Fn2(3) == 4);
}

// Test that reference return types are preserved (with `decltype(auto)`).
TEST_CASE("STLForwardCompatTest BindPreservesReferenceReturn", "[wpiutil][llvm]") {
  int X = 10;
  auto GetRef = [&X]() -> int & { return X; };

  auto BoundFront = wpi::util::bind_front(GetRef);
  static_assert(std::is_same_v<decltype(BoundFront()), int &>);
  BoundFront() = 20;
  CHECK(X == 20);

  const auto BoundFrontConst = wpi::util::bind_front(GetRef);
  static_assert(std::is_same_v<decltype(BoundFrontConst()), int &>);

  auto BoundBack = wpi::util::bind_back(GetRef);
  static_assert(std::is_same_v<decltype(BoundBack()), int &>);
  BoundBack() = 30;
  CHECK(X == 30);

  const auto BoundBackConst = wpi::util::bind_back(GetRef);
  static_assert(std::is_same_v<decltype(BoundBackConst()), int &>);
}

// Use std::ref/std::cref to bind references (bound args are decay-copied).
TEST_CASE("STLForwardCompatTest BindWithReferenceWrapper", "[wpiutil][llvm]") {
  int X = 1;
  auto Increment = wpi::util::bind_front([](int &Val) { ++Val; }, std::ref(X));
  Increment();
  CHECK(X == 2);
  Increment();
  CHECK(X == 3);
}

// The callable itself can have mutable state.
TEST_CASE("STLForwardCompatTest BindMutableCallable", "[wpiutil][llvm]") {
  auto Counter = wpi::util::bind_front([N = 0]() mutable { return ++N; });
  CHECK(Counter() == 1);
  CHECK(Counter() == 2);
  CHECK(Counter() == 3);
}

namespace {
struct MemberTest {
  int Value;
  int scale(int Factor) const { return Value * Factor; }
};
} // namespace

TEST_CASE("STLForwardCompatTest BindMembers", "[wpiutil][llvm]") {
  // Member function pointer support via std::apply (with std::invoke used
  // internally).
  MemberTest Obj{10};
  auto ScaleObj = wpi::util::bind_front(&MemberTest::scale, Obj);
  CHECK(ScaleObj(3) == 30);
  auto ScaleBy5 = wpi::util::bind_back(&MemberTest::scale, 5);
  CHECK(ScaleBy5(Obj) == 50);

  // Member data pointer support via std::apply (with std::invoke used
  // internally).
  auto GetValue = wpi::util::bind_front(&MemberTest::Value);
  CHECK(GetValue(Obj) == 10);

  // Make sure we can use member data pointers for constexpr callables.
  static constexpr int MemberVal =
      wpi::util::bind_front(&MemberTest::Value)(MemberTest{10});
  CHECK(MemberVal == 10);
}

TEST_CASE("STLForwardCompatTest BindFrontBindBack", "[wpiutil][llvm]") {
  std::vector<int> V;
  auto MulAdd = [](int A, int B, int C) { return A * (B + C) == 12; };
  auto MulAdd1 = [](const int &A, const int &B, const int &C) {
    return A * (B + C) == 12;
  };
  auto Mul0 = wpi::util::bind_back(MulAdd, 4, 2);
  auto MulL = wpi::util::bind_front(MulAdd1, 2, 4);
  auto Mul20 = wpi::util::bind_back(MulAdd, 4);
  auto Mul21 = wpi::util::bind_front(MulAdd1, 2);
  CHECK(std::ranges::all_of(V, Mul0));
  CHECK(std::ranges::all_of(V, MulL));

  V.push_back(2);
  CHECK(std::ranges::all_of(V, Mul0));
  CHECK(std::ranges::all_of(V, MulL));

  V.push_back(2);
  V.push_back(2);
  CHECK(std::ranges::all_of(V, Mul0));
  CHECK(std::ranges::all_of(V, MulL));

  auto Spec0 = wpi::util::bind_front(Mul20, 2);
  auto Spec1 = wpi::util::bind_back(Mul21, 4);
  CHECK(std::ranges::all_of(V, Spec0));
  CHECK(std::ranges::all_of(V, Spec1));

  V.push_back(3);
  CHECK_FALSE(std::ranges::all_of(V, Mul0));
  CHECK_FALSE(std::ranges::all_of(V, MulL));
  CHECK_FALSE(std::ranges::all_of(V, Spec0));
  CHECK_FALSE(std::ranges::all_of(V, Spec1));
  CHECK(std::ranges::any_of(V, Spec0));
  CHECK(std::ranges::any_of(V, Spec1));
}

// Compile-time tests for wpi::util::is_sorted_constexpr
static constexpr int CEmptyHelper[]{-1};
static_assert(wpi::util::is_sorted_constexpr(std::begin(CEmptyHelper),
                                        std::begin(CEmptyHelper)),
              "Empty range should be sorted");

static constexpr int CSingle[]{42};
static_assert(wpi::util::is_sorted_constexpr(std::begin(CSingle), std::end(CSingle)),
              "Single element range should be sorted");
static_assert(wpi::util::is_sorted_constexpr(std::begin(CSingle), std::end(CSingle),
                                        std::greater<>()),
              "Single element range should be sorted with std::greater");

static constexpr int CSorted[]{1, 2, 2, 3, 5};
static_assert(wpi::util::is_sorted_constexpr(std::begin(CSorted), std::end(CSorted)),
              "Non-descending order with duplicates should be sorted");
static_assert(wpi::util::is_sorted_constexpr(std::begin(CSorted), std::end(CSorted),
                                        std::less<>()),
              "Explicit std::less non-descending order should be sorted");
static_assert(!wpi::util::is_sorted_constexpr(std::begin(CSorted), std::end(CSorted),
                                         std::greater<>()),
              "Non-descending order should not be sorted by std::greater");

static constexpr int CUnsorted[]{1, 3, 2, 4, 5};
static_assert(!wpi::util::is_sorted_constexpr(std::begin(CUnsorted),
                                         std::end(CUnsorted)),
              "Unsorted range should not be sorted");

static constexpr int CDesc[]{9, 7, 7, 3, 0};
static_assert(wpi::util::is_sorted_constexpr(std::begin(CDesc), std::end(CDesc),
                                        std::greater<>()),
              "Non-ascending order with std::greater should be sorted");
static_assert(wpi::util::is_sorted_constexpr(std::rbegin(CDesc), std::rend(CDesc)),
              "Reverse iterators should be supported.");

} // namespace
} // namespace wpi::util
