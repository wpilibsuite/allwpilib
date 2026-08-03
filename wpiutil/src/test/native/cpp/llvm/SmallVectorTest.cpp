//===- llvm/unittest/ADT/SmallVectorTest.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// SmallVector unit tests.
//
//===----------------------------------------------------------------------===//

#include "wpi/util/SmallVector.hpp"
#include "wpi/util/Compiler.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <array>
#include <csignal>
#include <cstdlib>
#include <list>
#include <regex>
#include <span>
#include <stdarg.h>
#include <string>

#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

using namespace wpi::util;

namespace {

#ifdef _WIN32
#define WPIUTIL_HAS_DEATH_TEST 0
#define CHECK_DEATH(statement, matcher) \
  SKIP("death tests are not supported by Catch2 on Windows")
#else
#define WPIUTIL_HAS_DEATH_TEST 1
template <typename F>
bool CheckDeathImpl(F&& func, std::string_view matcher) {
  int pipeFds[2];
  if (pipe(pipeFds) != 0) {
    return false;
  }

  pid_t pid = fork();
  if (pid == 0) {
    std::signal(SIGABRT, SIG_DFL);
    std::signal(SIGBUS, SIG_DFL);
    std::signal(SIGFPE, SIG_DFL);
    std::signal(SIGILL, SIG_DFL);
    std::signal(SIGSEGV, SIG_DFL);

    close(pipeFds[0]);
    dup2(pipeFds[1], STDERR_FILENO);
    close(pipeFds[1]);
    func();
    std::_Exit(0);
  }

  close(pipeFds[1]);
  std::string output;
  char buffer[512];
  ssize_t count;
  while ((count = read(pipeFds[0], buffer, sizeof(buffer))) > 0) {
    output.append(buffer, count);
  }
  close(pipeFds[0]);

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    return false;
  }

  bool died = WIFSIGNALED(status) || (WIFEXITED(status) && WEXITSTATUS(status));
  bool outputMatches =
      std::regex_search(output, std::regex{std::string{matcher}});
  if (!died || !outputMatches) {
    UNSCOPED_INFO("Death test output: " << output);
  }
  return died && outputMatches;
}

#define CHECK_DEATH(statement, matcher) \
  CHECK(CheckDeathImpl([&] { statement; }, matcher))
#endif

/// A helper class that counts the total number of constructor and
/// destructor calls.
class Constructable {
private:
  static int numConstructorCalls;
  static int numMoveConstructorCalls;
  static int numCopyConstructorCalls;
  static int numDestructorCalls;
  static int numAssignmentCalls;
  static int numMoveAssignmentCalls;
  static int numCopyAssignmentCalls;

  bool constructed;
  int value;

public:
  Constructable() : constructed(true), value(0) {
    ++numConstructorCalls;
  }

  Constructable(int val) : constructed(true), value(val) {
    ++numConstructorCalls;
  }

  Constructable(const Constructable & src) : constructed(true) {
    value = src.value;
    ++numConstructorCalls;
    ++numCopyConstructorCalls;
  }

  Constructable(Constructable && src) : constructed(true) {
    value = src.value;
    src.value = 0;
    ++numConstructorCalls;
    ++numMoveConstructorCalls;
  }

  ~Constructable() {
    CHECK(constructed);
    ++numDestructorCalls;
    constructed = false;
  }

  Constructable & operator=(const Constructable & src) {
    CHECK(constructed);
    value = src.value;
    ++numAssignmentCalls;
    ++numCopyAssignmentCalls;
    return *this;
  }

  Constructable & operator=(Constructable && src) {
    CHECK(constructed);
    value = src.value;
    src.value = 0;
    ++numAssignmentCalls;
    ++numMoveAssignmentCalls;
    return *this;
  }

  int getValue() const {
    return abs(value);
  }

  static void reset() {
    numConstructorCalls = 0;
    numMoveConstructorCalls = 0;
    numCopyConstructorCalls = 0;
    numDestructorCalls = 0;
    numAssignmentCalls = 0;
    numMoveAssignmentCalls = 0;
    numCopyAssignmentCalls = 0;
  }

  static int getNumConstructorCalls() {
    return numConstructorCalls;
  }

  static int getNumMoveConstructorCalls() {
    return numMoveConstructorCalls;
  }

  static int getNumCopyConstructorCalls() {
    return numCopyConstructorCalls;
  }

  static int getNumDestructorCalls() {
    return numDestructorCalls;
  }

  static int getNumAssignmentCalls() {
    return numAssignmentCalls;
  }

  static int getNumMoveAssignmentCalls() {
    return numMoveAssignmentCalls;
  }

  static int getNumCopyAssignmentCalls() {
    return numCopyAssignmentCalls;
  }

  friend bool operator==(const Constructable &c0, const Constructable &c1) {
    return c0.getValue() == c1.getValue();
  }

  [[maybe_unused]] friend bool operator!=(const Constructable &c0,
                                          const Constructable &c1) {
    return c0.getValue() != c1.getValue();
  }

  friend bool operator<(const Constructable &c0, const Constructable &c1) {
    return c0.getValue() < c1.getValue();
  }
  [[maybe_unused]] friend bool operator<=(const Constructable &c0,
                                          const Constructable &c1) {
    return c0.getValue() <= c1.getValue();
  }
  [[maybe_unused]] friend bool operator>(const Constructable &c0,
                                         const Constructable &c1) {
    return c0.getValue() > c1.getValue();
  }
  [[maybe_unused]] friend bool operator>=(const Constructable &c0,
                                          const Constructable &c1) {
    return c0.getValue() >= c1.getValue();
  }
};

int Constructable::numConstructorCalls;
int Constructable::numCopyConstructorCalls;
int Constructable::numMoveConstructorCalls;
int Constructable::numDestructorCalls;
int Constructable::numAssignmentCalls;
int Constructable::numCopyAssignmentCalls;
int Constructable::numMoveAssignmentCalls;

struct NonCopyable {
  NonCopyable() = default;
  NonCopyable(NonCopyable &&) {}
  NonCopyable &operator=(NonCopyable &&) { return *this; }
private:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
};

LLVM_ATTRIBUTE_USED void CompileTest() {
  SmallVector<NonCopyable, 0> V;
  V.resize(42);
}

TEST_CASE("SmallVectorTest ConstructNonCopyableTest", "[wpiutil][llvm]") {
  SmallVector<NonCopyable, 0> V(42);
  CHECK(V.size() == (size_t)42);
}

// Assert that v contains the specified values, in order.
template <typename VectorT>
void assertValuesInOrder(VectorT &v, size_t size, ...) {
  CHECK(size == v.size());

  va_list ap;
  va_start(ap, size);
  for (size_t i = 0; i < size; ++i) {
    int value = va_arg(ap, int);
    CHECK(value == v[i].getValue());
  }

  va_end(ap);
}

template <typename VectorT> void assertEmpty(VectorT &v) {
  // Size tests
  CHECK(0u == v.size());
  CHECK(v.empty());

  // Iterator tests
  CHECK(v.begin() == v.end());
}

// Generate a sequence of values to initialize the vector.
template <typename VectorT> void makeSequence(VectorT &v, int start, int end) {
  for (int i = start; i <= end; ++i) {
    v.push_back(Constructable(i));
  }
}

template <typename T, unsigned N>
constexpr static unsigned NumBuiltinElts(const SmallVector<T, N> &) {
  return N;
}

class SmallVectorTestBase {
protected:
  SmallVectorTestBase() { Constructable::reset(); }
};

// Test fixture class
template <typename VectorT>
class SmallVectorTest : public SmallVectorTestBase {
protected:
  VectorT theVector;
  VectorT otherVector;
};

#define WPIUTIL_TEST_TYPES_SmallVectorTest \
  (SmallVector<Constructable, 0>), (SmallVector<Constructable, 1>), \
      (SmallVector<Constructable, 2>), (SmallVector<Constructable, 4>), \
      (SmallVector<Constructable, 5>)

// Constructor test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ConstructorNonIterTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ConstructorTest");
  auto &V = this->theVector;
  V = SmallVector<Constructable, 2>(2, 2);
  assertValuesInOrder(V, 2u, 2, 2);
}

// Constructor test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ConstructorIterTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ConstructorTest");
  int arr[] = {1, 2, 3};
  auto &V = this->theVector;
  V = SmallVector<Constructable, 4>(std::begin(arr), std::end(arr));
  assertValuesInOrder(V, 3u, 1, 2, 3);
}

// Constructor test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ConstructorFromSpanSimpleTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ConstructorFromSpanSimpleTest");
  std::array<Constructable, 3> StdArray = {Constructable(1), Constructable(2),
                                           Constructable(3)};
  std::span<const Constructable> Array = StdArray;
  auto &V = this->theVector;
  V = SmallVector<Constructable, 4>(Array);
  assertValuesInOrder(V, 3u, 1, 2, 3);
  REQUIRE(NumBuiltinElts(TestType{}) == NumBuiltinElts(V));
}

// New vector test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest EmptyVectorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("EmptyVectorTest");
  auto &V = this->theVector;
  assertEmpty(V);
  CHECK(V.rbegin() == V.rend());
  CHECK(0 == Constructable::getNumConstructorCalls());
  CHECK(0 == Constructable::getNumDestructorCalls());
}

// Simple insertions and deletions.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest PushPopTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("PushPopTest");
  auto &V = this->theVector;
  // Track whether the vector will potentially have to grow.
  bool RequiresGrowth = V.capacity() < 3;

  // Push an element
  V.push_back(Constructable(1));

  // Size tests
  assertValuesInOrder(V, 1u, 1);
  CHECK_FALSE(V.begin() == V.end());
  CHECK_FALSE(V.empty());

  // Push another element
  V.push_back(Constructable(2));
  assertValuesInOrder(V, 2u, 1, 2);

  // Insert at beginning. Reserve space to avoid reference invalidation from
  // V[1].
  V.reserve(V.size() + 1);
  V.insert(V.begin(), V[1]);
  assertValuesInOrder(V, 3u, 2, 1, 2);

  // Pop one element
  V.pop_back();
  assertValuesInOrder(V, 2u, 2, 1);

  // Pop remaining elements
  V.pop_back_n(2);
  assertEmpty(V);

  // Check number of constructor calls. Should be 2 for each list element,
  // one for the argument to push_back, one for the argument to insert,
  // and one for the list element itself.
  if (!RequiresGrowth) {
    CHECK(5 == Constructable::getNumConstructorCalls());
    CHECK(5 == Constructable::getNumDestructorCalls());
  } else {
    // If we had to grow the vector, these only have a lower bound, but should
    // always be equal.
    CHECK(5 <= Constructable::getNumConstructorCalls());
    CHECK(Constructable::getNumConstructorCalls() == Constructable::getNumDestructorCalls());
  }
}

// Clear test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ClearTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ClearTest");
  auto &V = this->theVector;
  V.reserve(2);
  makeSequence(V, 1, 2);
  V.clear();

  assertEmpty(V);
  CHECK(4 == Constructable::getNumConstructorCalls());
  CHECK(4 == Constructable::getNumDestructorCalls());
}

// Resize smaller test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ResizeShrinkTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ResizeShrinkTest");
  auto &V = this->theVector;
  V.reserve(3);
  makeSequence(V, 1, 3);
  V.resize(1);

  assertValuesInOrder(V, 1u, 1);
  CHECK(6 == Constructable::getNumConstructorCalls());
  CHECK(5 == Constructable::getNumDestructorCalls());
}

// Truncate test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest TruncateTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("TruncateTest");
  auto &V = this->theVector;
  V.reserve(3);
  makeSequence(V, 1, 3);
  V.truncate(1);

  assertValuesInOrder(V, 1u, 1);
  CHECK(6 == Constructable::getNumConstructorCalls());
  CHECK(5 == Constructable::getNumDestructorCalls());

#if !defined(NDEBUG) && WPIUTIL_HAS_DEATH_TEST
  CHECK_DEATH(V.truncate(2), "Cannot increase size");
#endif
  V.truncate(1);
  assertValuesInOrder(V, 1u, 1);
  CHECK(6 == Constructable::getNumConstructorCalls());
  CHECK(5 == Constructable::getNumDestructorCalls());

  V.truncate(0);
  assertEmpty(V);
  CHECK(6 == Constructable::getNumConstructorCalls());
  CHECK(6 == Constructable::getNumDestructorCalls());
}

// Resize bigger test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ResizeGrowTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ResizeGrowTest");
  auto &V = this->theVector;
  V.resize(2);

  CHECK(2 == Constructable::getNumConstructorCalls());
  CHECK(0 == Constructable::getNumDestructorCalls());
  CHECK(2u == V.size());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ResizeWithElementsTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  auto &V = this->theVector;
  V.resize(2);

  Constructable::reset();

  V.resize(4);

  size_t Ctors = Constructable::getNumConstructorCalls();
  CHECK((Ctors == 2 || Ctors == 4));
  size_t MoveCtors = Constructable::getNumMoveConstructorCalls();
  CHECK((MoveCtors == 0 || MoveCtors == 2));
  size_t Dtors = Constructable::getNumDestructorCalls();
  CHECK((Dtors == 0 || Dtors == 2));
}

// Resize with fill value.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ResizeFillTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ResizeFillTest");
  auto &V = this->theVector;
  V.resize(3, Constructable(77));
  assertValuesInOrder(V, 3u, 77, 77, 77);
}

TEST_CASE("SmallVectorTest ResizeForOverwrite", "[wpiutil][llvm]") {
  {
    // Heap allocated storage.
    SmallVector<unsigned, 0> V;
    V.push_back(5U);
    V.pop_back();
    V.resize_for_overwrite(V.size() + 1U);
    CHECK(5U == V.back());
    V.pop_back();
    V.resize(V.size() + 1);
    CHECK(0U == V.back());
  }
  {
    // Inline storage.
    SmallVector<unsigned, 2> V;
    V.push_back(5U);
    V.pop_back();
    V.resize_for_overwrite(V.size() + 1U);
    CHECK(5U == V.back());
    V.pop_back();
    V.resize(V.size() + 1);
    CHECK(0U == V.back());
  }
}

// Overflow past fixed size.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest OverflowTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("OverflowTest");
  auto &V = this->theVector;
  // Push more elements than the fixed size.
  makeSequence(V, 1, 10);

  // Test size and values.
  CHECK(10u == V.size());
  for (int i = 0; i < 10; ++i) {
    CHECK(i + 1 == V[i].getValue());
  }

  // Now resize back to fixed size.
  V.resize(1);

  assertValuesInOrder(V, 1u, 1);
}

// Iteration tests.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest IterationTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  auto &V = this->theVector;
  makeSequence(V, 1, 2);

  // Forward Iteration
  typename TestType::iterator it = V.begin();
  CHECK(*it == V.front());
  CHECK(*it == V[0]);
  CHECK(1 == it->getValue());
  ++it;
  CHECK(*it == V[1]);
  CHECK(*it == V.back());
  CHECK(2 == it->getValue());
  ++it;
  CHECK(it == V.end());
  --it;
  CHECK(*it == V[1]);
  CHECK(2 == it->getValue());
  --it;
  CHECK(*it == V[0]);
  CHECK(1 == it->getValue());

  // Reverse Iteration
  typename TestType::reverse_iterator rit = V.rbegin();
  CHECK(*rit == V[1]);
  CHECK(2 == rit->getValue());
  ++rit;
  CHECK(*rit == V[0]);
  CHECK(1 == rit->getValue());
  ++rit;
  CHECK(rit == V.rend());
  --rit;
  CHECK(*rit == V[0]);
  CHECK(1 == rit->getValue());
  --rit;
  CHECK(*rit == V[1]);
  CHECK(2 == rit->getValue());
}

// Swap test.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest SwapTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("SwapTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  makeSequence(V, 1, 2);
  std::swap(V, U);

  assertEmpty(V);
  assertValuesInOrder(U, 2u, 1, 2);
}

// Append test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AppendTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AppendTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  makeSequence(U, 2, 3);

  V.push_back(Constructable(1));
  V.append(U.begin(), U.end());

  assertValuesInOrder(V, 3u, 1, 2, 3);
}

// Append repeated test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AppendRepeatedTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AppendRepeatedTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.append(2, Constructable(77));
  assertValuesInOrder(V, 3u, 1, 77, 77);
}

// Append test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AppendNonIterTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AppendRepeatedTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.append(2, 7);
  assertValuesInOrder(V, 3u, 1, 7, 7);
}

struct output_iterator {
  using iterator_category = std::output_iterator_tag;
  using value_type = int;
  using difference_type = int;
  using pointer = value_type *;
  using reference = value_type &;
  operator int() { return 2; }
  operator Constructable() { return 7; }
};

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AppendRepeatedNonForwardIterator", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AppendRepeatedTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.append(output_iterator(), output_iterator());
  assertValuesInOrder(V, 3u, 1, 7, 7);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AppendSmallVector", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AppendSmallVector");
  auto &V = this->theVector;
  SmallVector<Constructable, 3> otherVector = {7, 7};
  V.push_back(Constructable(1));
  V.append(otherVector);
  assertValuesInOrder(V, 3u, 1, 7, 7);
}

// Assign test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AssignTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AssignTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.assign(2, Constructable(77));
  assertValuesInOrder(V, 2u, 77, 77);
}

// Assign test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AssignRangeTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AssignTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  int arr[] = {1, 2, 3};
  V.assign(std::begin(arr), std::end(arr));
  assertValuesInOrder(V, 3u, 1, 2, 3);
}

// Assign test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AssignNonIterTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AssignTest");
  auto &V = this->theVector;
  V.push_back(Constructable(1));
  V.assign(2, 7);
  assertValuesInOrder(V, 2u, 7, 7);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AssignSmallVector", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AssignSmallVector");
  auto &V = this->theVector;
  SmallVector<Constructable, 3> otherVector = {7, 7};
  V.push_back(Constructable(1));
  V.assign(otherVector);
  assertValuesInOrder(V, 2u, 7, 7);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest AssignSpan", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("AssignSpan");
  auto &V = this->theVector;
  Constructable Other[] = {7, 8, 9};
  V.push_back(Constructable(1));
  V.assign(std::span<const Constructable>(Other));
  assertValuesInOrder(V, 3u, 7, 8, 9);
}

// Move-assign test
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest MoveAssignTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("MoveAssignTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  // Set up our vector with a single element, but enough capacity for 4.
  V.reserve(4);
  V.push_back(Constructable(1));

  // Set up the other vector with 2 elements.
  U.push_back(Constructable(2));
  U.push_back(Constructable(3));

  // Move-assign from the other vector.
  V = std::move(U);

  // Make sure we have the right result.
  assertValuesInOrder(V, 2u, 2, 3);

  // Make sure the # of constructor/destructor calls line up. There
  // are two live objects after clearing the other vector.
  U.clear();
  CHECK(Constructable::getNumConstructorCalls()-2 == Constructable::getNumDestructorCalls());

  // There shouldn't be any live objects any more.
  V.clear();
  CHECK(Constructable::getNumConstructorCalls() == Constructable::getNumDestructorCalls());
}

// Erase a single element
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest EraseTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("EraseTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  const auto &theConstVector = V;
  V.erase(theConstVector.begin());
  assertValuesInOrder(V, 2u, 2, 3);
}

// Erase a range of elements
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest EraseRangeTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("EraseRangeTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  const auto &theConstVector = V;
  V.erase(theConstVector.begin(), theConstVector.begin() + 2);
  assertValuesInOrder(V, 1u, 3);
}

// Insert a single element.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  typename TestType::iterator I = V.insert(V.begin() + 1, Constructable(77));
  CHECK(V.begin() + 1 == I);
  assertValuesInOrder(V, 4u, 1, 77, 2, 3);
}

// Insert a copy of a single element.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertCopy", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);
  Constructable C(77);
  typename TestType::iterator I = V.insert(V.begin() + 1, C);
  CHECK(V.begin() + 1 == I);
  assertValuesInOrder(V, 4u, 1, 77, 2, 3);
}

// Insert repeated elements.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertRepeatedTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 4);
  Constructable::reset();
  auto I = V.insert(V.begin() + 1, 2, Constructable(16));
  // Move construct the top element into newly allocated space, and optionally
  // reallocate the whole buffer, move constructing into it.
  // FIXME: This is inefficient, we shouldn't move things into newly allocated
  // space, then move them up/around, there should only be 2 or 4 move
  // constructions here.
  CHECK((Constructable::getNumMoveConstructorCalls() == 2 ||
         Constructable::getNumMoveConstructorCalls() == 6));
  // Move assign the next two to shift them up and make a gap.
  CHECK(1 == Constructable::getNumMoveAssignmentCalls());
  // Copy construct the two new elements from the parameter.
  CHECK(2 == Constructable::getNumCopyAssignmentCalls());
  // All without any copy construction.
  CHECK(0 == Constructable::getNumCopyConstructorCalls());
  CHECK(V.begin() + 1 == I);
  assertValuesInOrder(V, 6u, 1, 16, 16, 2, 3, 4);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertRepeatedNonIterTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 4);
  Constructable::reset();
  auto I = V.insert(V.begin() + 1, 2, 7);
  CHECK(V.begin() + 1 == I);
  assertValuesInOrder(V, 6u, 1, 7, 7, 2, 3, 4);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertRepeatedAtEndTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 4);
  Constructable::reset();
  auto I = V.insert(V.end(), 2, Constructable(16));
  // Just copy construct them into newly allocated space
  CHECK(2 == Constructable::getNumCopyConstructorCalls());
  // Move everything across if reallocation is needed.
  CHECK((Constructable::getNumMoveConstructorCalls() == 0 ||
         Constructable::getNumMoveConstructorCalls() == 4));
  // Without ever moving or copying anything else.
  CHECK(0 == Constructable::getNumCopyAssignmentCalls());
  CHECK(0 == Constructable::getNumMoveAssignmentCalls());

  CHECK(V.begin() + 4 == I);
  assertValuesInOrder(V, 6u, 1, 2, 3, 4, 16, 16);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertRepeatedEmptyTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRepeatedTest");
  auto &V = this->theVector;
  makeSequence(V, 10, 15);

  // Empty insert.
  CHECK(V.end() == V.insert(V.end(), 0, Constructable(42)));
  CHECK(V.begin() + 1 == V.insert(V.begin() + 1, 0, Constructable(42)));
}

// Insert range.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertRangeTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRangeTest");
  auto &V = this->theVector;
  Constructable Arr[3] =
    { Constructable(77), Constructable(77), Constructable(77) };

  makeSequence(V, 1, 3);
  Constructable::reset();
  auto I = V.insert(V.begin() + 1, Arr, Arr + 3);
  // Move construct the top 3 elements into newly allocated space.
  // Possibly move the whole sequence into new space first.
  // FIXME: This is inefficient, we shouldn't move things into newly allocated
  // space, then move them up/around, there should only be 2 or 3 move
  // constructions here.
  CHECK((Constructable::getNumMoveConstructorCalls() == 2 ||
         Constructable::getNumMoveConstructorCalls() == 5));
  // Copy assign the lower 2 new elements into existing space.
  CHECK(2 == Constructable::getNumCopyAssignmentCalls());
  // Copy construct the third element into newly allocated space.
  CHECK(1 == Constructable::getNumCopyConstructorCalls());
  CHECK(V.begin() + 1 == I);
  assertValuesInOrder(V, 6u, 1, 77, 77, 77, 2, 3);
}


TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertRangeAtEndTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRangeTest");
  auto &V = this->theVector;
  Constructable Arr[3] =
    { Constructable(77), Constructable(77), Constructable(77) };

  makeSequence(V, 1, 3);

  // Insert at end.
  Constructable::reset();
  auto I = V.insert(V.end(), Arr, Arr + 3);
  // Copy construct the 3 elements into new space at the top.
  CHECK(3 == Constructable::getNumCopyConstructorCalls());
  // Don't copy/move anything else.
  CHECK(0 == Constructable::getNumCopyAssignmentCalls());
  // Reallocation might occur, causing all elements to be moved into the new
  // buffer.
  CHECK((Constructable::getNumMoveConstructorCalls() == 0 ||
         Constructable::getNumMoveConstructorCalls() == 3));
  CHECK(0 == Constructable::getNumMoveAssignmentCalls());
  CHECK(V.begin() + 3 == I);
  assertValuesInOrder(V, 6u, 1, 2, 3, 77, 77, 77);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest InsertEmptyRangeTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("InsertRangeTest");
  auto &V = this->theVector;
  makeSequence(V, 1, 3);

  // Empty insert.
  CHECK(V.end() == V.insert(V.end(), V.begin(), V.begin()));
  CHECK(V.begin() + 1 == V.insert(V.begin() + 1, V.begin(), V.begin()));
}

// Comparison tests.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ComparisonEqualityTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ComparisonEqualityTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  makeSequence(V, 1, 3);
  makeSequence(U, 1, 3);

  CHECK(V == U);
  CHECK_FALSE(V != U);

  U.clear();
  makeSequence(U, 2, 4);

  CHECK_FALSE(V == U);
  CHECK(V != U);
}

// Comparison tests.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ComparisonLessThanTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  UNSCOPED_INFO("ComparisonLessThanTest");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  V = {1, 2, 4};
  U = {1, 4};

  CHECK(V < U);
  CHECK(V <= U);
  CHECK_FALSE(V > U);
  CHECK_FALSE(V >= U);

  CHECK_FALSE(U < V);
  CHECK_FALSE(U <= V);
  CHECK(U > V);
  CHECK(U >= V);

  U = {1, 2, 4};

  CHECK_FALSE(V < U);
  CHECK(V <= U);
  CHECK_FALSE(V > U);
  CHECK(V >= U);

  CHECK_FALSE(U < V);
  CHECK(U <= V);
  CHECK_FALSE(U > V);
  CHECK(U >= V);
}

// Constant vector tests.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest ConstVectorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  const TestType constVector;

  CHECK(0u == constVector.size());
  CHECK(constVector.empty());
  CHECK(constVector.begin() == constVector.end());
}

// Direct array access.
TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest DirectVectorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  auto &V = this->theVector;
  CHECK(0u == V.size());
  V.reserve(4);
  CHECK(4u <= V.capacity());
  CHECK(0 == Constructable::getNumConstructorCalls());
  V.push_back(1);
  V.push_back(2);
  V.push_back(3);
  V.push_back(4);
  CHECK(4u == V.size());
  CHECK(8 == Constructable::getNumConstructorCalls());
  CHECK(1 == V[0].getValue());
  CHECK(2 == V[1].getValue());
  CHECK(3 == V[2].getValue());
  CHECK(4 == V[3].getValue());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorTest, "SmallVectorTest IteratorTest", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorTest) {
  auto &V = this->theVector;
  std::list<int> L;
  V.insert(V.end(), L.begin(), L.end());
}

template <typename InvalidType> class DualSmallVectorsTest;

template <typename VectorT1, typename VectorT2>
class DualSmallVectorsTest<std::pair<VectorT1, VectorT2>> : public SmallVectorTestBase {
protected:
  VectorT1 theVector;
  VectorT2 otherVector;
};

#define WPIUTIL_TEST_TYPES_DualSmallVectorsTest \
  (std::pair<SmallVector<Constructable, 4>, SmallVector<Constructable, 4>>), \
      (std::pair<SmallVector<Constructable, 4>, SmallVector<Constructable, 2>>), \
      (std::pair<SmallVector<Constructable, 2>, SmallVector<Constructable, 4>>), \
      (std::pair<SmallVector<Constructable, 2>, SmallVector<Constructable, 2>>)

TEMPLATE_TEST_CASE_METHOD(DualSmallVectorsTest, "DualSmallVectorsTest MoveAssignment", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_DualSmallVectorsTest) {
  UNSCOPED_INFO("MoveAssignTest-DualVectorTypes");
  auto &V = this->theVector;
  auto &U = this->otherVector;
  // Set up our vector with four elements.
  for (unsigned I = 0; I < 4; ++I)
    U.push_back(Constructable(I));

  const Constructable *OrigDataPtr = U.data();

  // Move-assign from the other vector.
  V = std::move(static_cast<SmallVectorImpl<Constructable> &>(U));

  // Make sure we have the right result.
  assertValuesInOrder(V, 4u, 0, 1, 2, 3);

  // Make sure the # of constructor/destructor calls line up. There
  // are two live objects after clearing the other vector.
  U.clear();
  CHECK(Constructable::getNumConstructorCalls()-4 == Constructable::getNumDestructorCalls());

  // If the source vector (otherVector) was in small-mode, assert that we just
  // moved the data pointer over.
  CHECK((NumBuiltinElts(U) == 4 || V.data() == OrigDataPtr));

  // There shouldn't be any live objects any more.
  V.clear();
  CHECK(Constructable::getNumConstructorCalls() == Constructable::getNumDestructorCalls());

  // We shouldn't have copied anything in this whole process.
  CHECK(Constructable::getNumCopyConstructorCalls() == 0);
}

struct notassignable {
  int &x;
  notassignable(int &x) : x(x) {}
};

TEST_CASE("SmallVectorCustomTest NoAssignTest", "[wpiutil][llvm]") {
  int x = 0;
  SmallVector<notassignable, 2> vec;
  vec.push_back(notassignable(x));
  x = 42;
  CHECK(42 == vec.pop_back_val().x);
}

struct MovedFrom {
  bool hasValue;
  MovedFrom() : hasValue(true) {
  }
  MovedFrom(MovedFrom&& m) : hasValue(m.hasValue) {
    m.hasValue = false;
  }
  MovedFrom &operator=(MovedFrom&& m) {
    hasValue = m.hasValue;
    m.hasValue = false;
    return *this;
  }
};

TEST_CASE("SmallVectorTest MidInsert", "[wpiutil][llvm]") {
  SmallVector<MovedFrom, 3> v;
  v.push_back(MovedFrom());
  v.insert(v.begin(), MovedFrom());
  for (MovedFrom &m : v)
    CHECK(m.hasValue);
}

enum EmplaceableArgState {
  EAS_Defaulted,
  EAS_Arg,
  EAS_LValue,
  EAS_RValue,
  EAS_Failure
};
template <int I> struct EmplaceableArg {
  EmplaceableArgState State;
  EmplaceableArg() : State(EAS_Defaulted) {}
  EmplaceableArg(EmplaceableArg &&X)
      : State(X.State == EAS_Arg ? EAS_RValue : EAS_Failure) {}
  EmplaceableArg(EmplaceableArg &X)
      : State(X.State == EAS_Arg ? EAS_LValue : EAS_Failure) {}

  explicit EmplaceableArg(bool) : State(EAS_Arg) {}

private:
  EmplaceableArg &operator=(EmplaceableArg &&) = delete;
  EmplaceableArg &operator=(const EmplaceableArg &) = delete;
};

enum EmplaceableState { ES_Emplaced, ES_Moved };
struct Emplaceable {
  EmplaceableArg<0> A0;
  EmplaceableArg<1> A1;
  EmplaceableArg<2> A2;
  EmplaceableArg<3> A3;
  EmplaceableState State;

  Emplaceable() : State(ES_Emplaced) {}

  template <class A0Ty>
  explicit Emplaceable(A0Ty &&A0)
      : A0(std::forward<A0Ty>(A0)), State(ES_Emplaced) {}

  template <class A0Ty, class A1Ty>
  Emplaceable(A0Ty &&A0, A1Ty &&A1)
      : A0(std::forward<A0Ty>(A0)), A1(std::forward<A1Ty>(A1)),
        State(ES_Emplaced) {}

  template <class A0Ty, class A1Ty, class A2Ty>
  Emplaceable(A0Ty &&A0, A1Ty &&A1, A2Ty &&A2)
      : A0(std::forward<A0Ty>(A0)), A1(std::forward<A1Ty>(A1)),
        A2(std::forward<A2Ty>(A2)), State(ES_Emplaced) {}

  template <class A0Ty, class A1Ty, class A2Ty, class A3Ty>
  Emplaceable(A0Ty &&A0, A1Ty &&A1, A2Ty &&A2, A3Ty &&A3)
      : A0(std::forward<A0Ty>(A0)), A1(std::forward<A1Ty>(A1)),
        A2(std::forward<A2Ty>(A2)), A3(std::forward<A3Ty>(A3)),
        State(ES_Emplaced) {}

  Emplaceable(Emplaceable &&) : State(ES_Moved) {}
  Emplaceable &operator=(Emplaceable &&) {
    State = ES_Moved;
    return *this;
  }

private:
  Emplaceable(const Emplaceable &) = delete;
  Emplaceable &operator=(const Emplaceable &) = delete;
};

TEST_CASE("SmallVectorTest EmplaceBack", "[wpiutil][llvm]") {
  EmplaceableArg<0> A0(true);
  EmplaceableArg<1> A1(true);
  EmplaceableArg<2> A2(true);
  EmplaceableArg<3> A3(true);
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back();
    CHECK(&back == &V.back());
    CHECK(V.size() == 1);
    CHECK(back.State == ES_Emplaced);
    CHECK(back.A0.State == EAS_Defaulted);
    CHECK(back.A1.State == EAS_Defaulted);
    CHECK(back.A2.State == EAS_Defaulted);
    CHECK(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(std::move(A0));
    CHECK(&back == &V.back());
    CHECK(V.size() == 1);
    CHECK(back.State == ES_Emplaced);
    CHECK(back.A0.State == EAS_RValue);
    CHECK(back.A1.State == EAS_Defaulted);
    CHECK(back.A2.State == EAS_Defaulted);
    CHECK(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(A0);
    CHECK(&back == &V.back());
    CHECK(V.size() == 1);
    CHECK(back.State == ES_Emplaced);
    CHECK(back.A0.State == EAS_LValue);
    CHECK(back.A1.State == EAS_Defaulted);
    CHECK(back.A2.State == EAS_Defaulted);
    CHECK(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(A0, A1);
    CHECK(&back == &V.back());
    CHECK(V.size() == 1);
    CHECK(back.State == ES_Emplaced);
    CHECK(back.A0.State == EAS_LValue);
    CHECK(back.A1.State == EAS_LValue);
    CHECK(back.A2.State == EAS_Defaulted);
    CHECK(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(std::move(A0), std::move(A1));
    CHECK(&back == &V.back());
    CHECK(V.size() == 1);
    CHECK(back.State == ES_Emplaced);
    CHECK(back.A0.State == EAS_RValue);
    CHECK(back.A1.State == EAS_RValue);
    CHECK(back.A2.State == EAS_Defaulted);
    CHECK(back.A3.State == EAS_Defaulted);
  }
  {
    SmallVector<Emplaceable, 3> V;
    Emplaceable &back = V.emplace_back(std::move(A0), A1, std::move(A2), A3);
    CHECK(&back == &V.back());
    CHECK(V.size() == 1);
    CHECK(back.State == ES_Emplaced);
    CHECK(back.A0.State == EAS_RValue);
    CHECK(back.A1.State == EAS_LValue);
    CHECK(back.A2.State == EAS_RValue);
    CHECK(back.A3.State == EAS_LValue);
  }
  {
    SmallVector<int, 1> V;
    V.emplace_back();
    V.emplace_back(42);
    CHECK(2U == V.size());
    CHECK(0 == V[0]);
    CHECK(42 == V[1]);
  }
}

TEST_CASE("SmallVectorTest DefaultInlinedElements", "[wpiutil][llvm]") {
  SmallVector<int> V;
  CHECK(V.empty());
  V.push_back(7);
  CHECK(V[0] == 7);

  // Check that at least a couple layers of nested SmallVector<T>'s are allowed
  // by the default inline elements policy. This pattern happens in practice
  // with some frequency, and it seems fairly harmless even though each layer of
  // SmallVector's will grow the total sizeof by a vector header beyond the
  // "preferred" maximum sizeof.
  SmallVector<SmallVector<SmallVector<int>>> NestedV;
  NestedV.emplace_back().emplace_back().emplace_back(42);
  CHECK(NestedV[0][0][0] == 42);
}

namespace namespace_with_adl {
struct MyVector {
  std::vector<int> data;
};

std::vector<int>::const_iterator begin(const MyVector &V) {
  return V.data.begin();
}
std::vector<int>::const_iterator end(const MyVector &V) { return V.data.end(); }
} // namespace namespace_with_adl

TEST_CASE("SmallVectorTest ToVector", "[wpiutil][llvm]") {
  {
    std::vector<char> v = {'a', 'b', 'c'};
    auto Vector = to_vector<4>(v);
    static_assert(NumBuiltinElts(Vector) == 4u);
    REQUIRE(3u == Vector.size());
    for (size_t I = 0; I < v.size(); ++I)
      CHECK(v[I] == Vector[I]);
  }
  {
    std::vector<char> v = {'a', 'b', 'c'};
    auto Vector = to_vector(v);
    static_assert(NumBuiltinElts(Vector) != 4u);
    REQUIRE(3u == Vector.size());
    for (size_t I = 0; I < v.size(); ++I)
      CHECK(v[I] == Vector[I]);
  }
  {
    // Check that to_vector and to_vector_of work with types that require ADL
    // for being/end iterators.
    namespace_with_adl::MyVector V = {{1, 2, 3}};
    auto IntVector = to_vector(V);
    CHECK_THAT(IntVector, Catch::Matchers::RangeEquals({1, 2, 3}));
    IntVector = to_vector<3>(V);
    CHECK_THAT(IntVector, Catch::Matchers::RangeEquals({1, 2, 3}));
  }
}

struct To {
  int Content;
  friend bool operator==(const To &LHS, const To &RHS) {
    return LHS.Content == RHS.Content;
  }
};

class From {
public:
  From() = default;
  From(To M) { T = M; }
  operator To() const { return T; }

private:
  To T;
};

TEST_CASE("SmallVectorTest ConstructFromSpanOfConvertibleType", "[wpiutil][llvm]") {
  To to1{1}, to2{2}, to3{3};
  std::vector<From> StdVector = {From(to1), From(to2), From(to3)};
  std::span<const From> Array = StdVector;
  {
    wpi::util::SmallVector<To> Vector(Array);

    REQUIRE(Array.size() == Vector.size());
    for (size_t I = 0; I < Array.size(); ++I)
      CHECK(Array[I] == Vector[I]);
  }
  {
    wpi::util::SmallVector<To, 4> Vector(Array);

    REQUIRE(Array.size() == Vector.size());
    REQUIRE(4u == NumBuiltinElts(Vector));
    for (size_t I = 0; I < Array.size(); ++I)
      CHECK(Array[I] == Vector[I]);
  }
}

TEST_CASE("SmallVectorTest ToVectorOf", "[wpiutil][llvm]") {
  To to1{1}, to2{2}, to3{3};
  std::vector<From> StdVector = {From(to1), From(to2), From(to3)};
  {
    wpi::util::SmallVector<To> Vector = wpi::util::to_vector_of<To>(StdVector);

    REQUIRE(StdVector.size() == Vector.size());
    for (size_t I = 0; I < StdVector.size(); ++I)
      CHECK(StdVector[I] == Vector[I]);
  }
  {
    auto Vector = wpi::util::to_vector_of<To, 4>(StdVector);

    REQUIRE(StdVector.size() == Vector.size());
    static_assert(NumBuiltinElts(Vector) == 4u);
    for (size_t I = 0; I < StdVector.size(); ++I)
      CHECK(StdVector[I] == Vector[I]);
  }
  {
    // Check that to_vector works with types that require ADL for being/end
    // iterators.
    namespace_with_adl::MyVector V = {{1, 2, 3}};
    auto UnsignedVector = to_vector_of<unsigned>(V);
    CHECK_THAT(UnsignedVector, Catch::Matchers::RangeEquals({1u, 2u, 3u}));
    UnsignedVector = to_vector_of<unsigned, 3>(V);
    CHECK_THAT(UnsignedVector, Catch::Matchers::RangeEquals({1u, 2u, 3u}));
  }
}

template <class VectorT>
class SmallVectorReferenceInvalidationTest : public SmallVectorTestBase {
protected:
  const char *AssertionMessage =
      "Attempting to reference an element of the vector in an operation \" "
      "\"that invalidates it";

  VectorT V;

  template <class T> static bool isValueType() {
    return std::is_same_v<T, typename VectorT::value_type>;
  }

  SmallVectorReferenceInvalidationTest() {
    // Fill up the small size so that insertions move the elements.
    for (int I = 0, E = NumBuiltinElts(V); I != E; ++I)
      V.emplace_back(I + 1);
  }
};

// Test one type that's trivially copyable (int) and one that isn't
// (Constructable) since reference invalidation may be fixed differently for
// each.
#define WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest \
  (SmallVector<int, 3>), (SmallVector<Constructable, 3>)

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest PushBack", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.push_back(V.back());
  CHECK(N == V.back());

  // Check that the old value is still there (not moved away).
  CHECK(N == V[V.size() - 2]);

  // Fill storage again.
  V.back() = V.size();
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.push_back(V.back());
  CHECK(int(V.size()) - 1 == V.back());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest PushBackMoved", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.push_back(std::move(V.back()));
  CHECK(N == V.back());
  if (this->template isValueType<Constructable>()) {
    // Check that the value was moved (not copied).
    CHECK(0 == V[V.size() - 2]);
  }

  // Fill storage again.
  V.back() = V.size();
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.push_back(std::move(V.back()));

  // Check the values.
  CHECK(int(V.size()) - 1 == V.back());
  if (this->template isValueType<Constructable>()) {
    // Check the value got moved out.
    CHECK(0 == V[V.size() - 2]);
  }
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest Resize", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  auto &V = this->V;
  (void)V;
  int N = NumBuiltinElts(V);
  V.resize(N + 1, V.back());
  CHECK(N == V.back());

  // Resize to add enough elements that V will grow again. If reference
  // invalidation breaks in the future, sanitizers should be able to catch a
  // use-after-free here.
  V.resize(V.capacity() + 1, V.front());
  CHECK(1 == V.back());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest Append", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  auto &V = this->V;
  (void)V;
  V.append(1, V.back());
  int N = NumBuiltinElts(V);
  CHECK(N == V[N - 1]);

  // Append enough more elements that V will grow again. This tests growing
  // when already in large mode.
  //
  // If reference invalidation breaks in the future, sanitizers should be able
  // to catch a use-after-free here.
  V.append(V.capacity() - V.size() + 1, V.front());
  CHECK(1 == V.back());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest AppendRange", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  auto &V = this->V;
  (void)V;
#if !defined(NDEBUG) && WPIUTIL_HAS_DEATH_TEST
  CHECK_DEATH(V.append(V.begin(), V.begin() + 1), this->AssertionMessage);

  REQUIRE(3u == NumBuiltinElts(V));
  REQUIRE(3u == V.size());
  V.pop_back();
  REQUIRE(2u == V.size());

  // Confirm this checks for growth when there's more than one element
  // appended.
  CHECK_DEATH(V.append(V.begin(), V.end()), this->AssertionMessage);
#endif
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest Assign", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  (void)V;
  int N = NumBuiltinElts(V);
  REQUIRE(unsigned(N) == V.size());
  REQUIRE(unsigned(N) == V.capacity());

  // Check assign that shrinks in small mode.
  V.assign(1, V.back());
  CHECK(1u == V.size());
  CHECK(N == V[0]);

  // Check assign that grows within small mode.
  REQUIRE(V.size() < V.capacity());
  V.assign(V.capacity(), V.back());
  for (int I = 0, E = V.size(); I != E; ++I) {
    CHECK(N == V[I]);

    // Reset to [1, 2, ...].
    V[I] = I + 1;
  }

  // Check assign that grows to large mode.
  REQUIRE(2 == V[1]);
  V.assign(V.capacity() + 1, V[1]);
  for (int I = 0, E = V.size(); I != E; ++I) {
    CHECK(2 == V[I]);

    // Reset to [1, 2, ...].
    V[I] = I + 1;
  }

  // Check assign that shrinks in large mode.
  V.assign(1, V[1]);
  CHECK(2 == V[0]);
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest AssignRange", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  auto &V = this->V;
#if !defined(NDEBUG) && WPIUTIL_HAS_DEATH_TEST
  CHECK_DEATH(V.assign(V.begin(), V.end()), this->AssertionMessage);
  CHECK_DEATH(V.assign(V.begin(), V.end() - 1), this->AssertionMessage);
#endif
  V.assign(V.begin(), V.begin());
  CHECK(V.empty());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest Insert", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  (void)V;

  // Insert a reference to the back (not at end() or else insert delegates to
  // push_back()), growing out of small mode. Confirm the value was copied out
  // (moving out Constructable sets it to 0).
  V.insert(V.begin(), V.back());
  CHECK(int(V.size() - 1) == V.front());
  CHECK(int(V.size() - 1) == V.back());

  // Fill up the vector again.
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Grow again from large storage to large storage.
  V.insert(V.begin(), V.back());
  CHECK(int(V.size() - 1) == V.front());
  CHECK(int(V.size() - 1) == V.back());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest InsertMoved", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  (void)V;

  // Insert a reference to the back (not at end() or else insert delegates to
  // push_back()), growing out of small mode. Confirm the value was copied out
  // (moving out Constructable sets it to 0).
  V.insert(V.begin(), std::move(V.back()));
  CHECK(int(V.size() - 1) == V.front());
  if (this->template isValueType<Constructable>()) {
    // Check the value got moved out.
    CHECK(0 == V.back());
  }

  // Fill up the vector again.
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Grow again from large storage to large storage.
  V.insert(V.begin(), std::move(V.back()));
  CHECK(int(V.size() - 1) == V.front());
  if (this->template isValueType<Constructable>()) {
    // Check the value got moved out.
    CHECK(0 == V.back());
  }
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest InsertN", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  auto &V = this->V;
  (void)V;

  // Cover NumToInsert <= this->end() - I.
  V.insert(V.begin() + 1, 1, V.back());
  int N = NumBuiltinElts(V);
  CHECK(N == V[1]);

  // Cover NumToInsert > this->end() - I, inserting enough elements that V will
  // also grow again; V.capacity() will be more elements than necessary but
  // it's a simple way to cover both conditions.
  //
  // If reference invalidation breaks in the future, sanitizers should be able
  // to catch a use-after-free here.
  V.insert(V.begin(), V.capacity(), V.front());
  CHECK(1 == V.front());
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest InsertRange", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  auto &V = this->V;
  (void)V;
#if !defined(NDEBUG) && WPIUTIL_HAS_DEATH_TEST
  CHECK_DEATH(V.insert(V.begin(), V.begin(), V.begin() + 1), this->AssertionMessage);

  REQUIRE(3u == NumBuiltinElts(V));
  REQUIRE(3u == V.size());
  V.pop_back();
  REQUIRE(2u == V.size());

  // Confirm this checks for growth when there's more than one element
  // inserted.
  CHECK_DEATH(V.insert(V.begin(), V.begin(), V.end()), this->AssertionMessage);
#endif
}

TEMPLATE_TEST_CASE_METHOD(SmallVectorReferenceInvalidationTest, "SmallVectorReferenceInvalidationTest EmplaceBack", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.emplace_back(V.back());
  CHECK(N == V.back());

  // Check that the old value is still there (not moved away).
  CHECK(N == V[V.size() - 2]);

  // Fill storage again.
  V.back() = V.size();
  while (V.size() < V.capacity())
    V.push_back(V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.emplace_back(V.back());
  CHECK(int(V.size()) - 1 == V.back());
}

template <class VectorT>
class SmallVectorInternalReferenceInvalidationTest
    : public SmallVectorTestBase {
protected:
  const char *AssertionMessage =
      "Attempting to reference an element of the vector in an operation \" "
      "\"that invalidates it";

  VectorT V;

  SmallVectorInternalReferenceInvalidationTest() {
    // Fill up the small size so that insertions move the elements.
    for (int I = 0, E = NumBuiltinElts(V); I != E; ++I)
      V.emplace_back(I + 1, I + 1);
  }
};

// Test pairs of the same types from SmallVectorReferenceInvalidationTs.
#define WPIUTIL_TEST_TYPES_SmallVectorInternalReferenceInvalidationTest \
  (SmallVector<std::pair<int, int>, 3>), \
      (SmallVector<std::pair<Constructable, Constructable>, 3>)

TEMPLATE_TEST_CASE_METHOD(SmallVectorInternalReferenceInvalidationTest, "SmallVectorInternalReferenceInvalidationTest EmplaceBack", "[wpiutil][llvm]", WPIUTIL_TEST_TYPES_SmallVectorInternalReferenceInvalidationTest) {
  // Note: setup adds [1, 2, ...] to V until it's at capacity in small mode.
  auto &V = this->V;
  int N = NumBuiltinElts(V);

  // Push back a reference to last element when growing from small storage.
  V.emplace_back(V.back().first, V.back().second);
  CHECK(N == V.back().first);
  CHECK(N == V.back().second);

  // Check that the old value is still there (not moved away).
  CHECK(N == V[V.size() - 2].first);
  CHECK(N == V[V.size() - 2].second);

  // Fill storage again.
  V.back().first = V.back().second = V.size();
  while (V.size() < V.capacity())
    V.emplace_back(V.size() + 1, V.size() + 1);

  // Push back a reference to last element when growing from large storage.
  V.emplace_back(V.back().first, V.back().second);
  CHECK(int(V.size()) - 1 == V.back().first);
  CHECK(int(V.size()) - 1 == V.back().second);
}

} // end namespace
