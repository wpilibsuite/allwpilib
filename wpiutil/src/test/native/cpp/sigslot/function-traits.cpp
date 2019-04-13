/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

/*

Sigslot, a signal-slot library

https://github.com/palacaze/sigslot

MIT License

Copyright (c) 2017 Pierre-Antoine Lacaze

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "wpi/Signal.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <type_traits>

using namespace wpi::sig::trait;

namespace {

void f1(int, char, float) {}
void f2(int, char, float) noexcept {}

struct oo {
  void operator()(int) {}
  void operator()(int, char, float) {}
};

struct s {
  static void s1(int, char, float) {}
  static void s2(int, char, float) noexcept {}

  void f1(int, char, float) {}
  void f2(int, char, float) const {}
  void f3(int, char, float) volatile {}
  void f4(int, char, float) const volatile {}
  void f5(int, char, float) noexcept {}
  void f6(int, char, float) const noexcept {}
  void f7(int, char, float) volatile noexcept {}
  void f8(int, char, float) const volatile noexcept {}
};

struct o1 {
  void operator()(int, char, float) {}
};
struct o2 {
  void operator()(int, char, float) const {}
};
struct o3 {
  void operator()(int, char, float) volatile {}
};
struct o4 {
  void operator()(int, char, float) const volatile {}
};
struct o5 {
  void operator()(int, char, float) noexcept {}
};
struct o6 {
  void operator()(int, char, float) const noexcept {}
};
struct o7 {
  void operator()(int, char, float) volatile noexcept {}
};
struct o8 {
  void operator()(int, char, float) const volatile noexcept {}
};

using t = typelist<int, char, float>;

static_assert(is_callable_v<t, decltype(f1)>, "");
static_assert(is_callable_v<t, decltype(f2)>, "");
static_assert(is_callable_v<t, decltype(&s::s1)>, "");
static_assert(is_callable_v<t, decltype(&s::s2)>, "");
static_assert(is_callable_v<t, oo>, "");
static_assert(is_callable_v<t, decltype(&s::f1), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f2), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f3), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f4), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f5), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f6), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f7), s*>, "");
static_assert(is_callable_v<t, decltype(&s::f8), s*>, "");
static_assert(is_callable_v<t, o1>, "");
static_assert(is_callable_v<t, o2>, "");
static_assert(is_callable_v<t, o3>, "");
static_assert(is_callable_v<t, o4>, "");
static_assert(is_callable_v<t, o5>, "");
static_assert(is_callable_v<t, o6>, "");
static_assert(is_callable_v<t, o7>, "");
static_assert(is_callable_v<t, o8>, "");

}  // namespace

namespace wpi {

TEST(Signal, FunctionTraits) {
  auto l1 = [](int, char, float) {};
  auto l2 = [&](int, char, float) mutable {};
  auto l3 = [&](auto...) mutable {};

  static_assert(is_callable_v<t, decltype(l1)>, "");
  static_assert(is_callable_v<t, decltype(l2)>, "");
  static_assert(is_callable_v<t, decltype(l3)>, "");

  f1(0, '0', 0.0);
  f2(0, '0', 0.0);
}

}  // namespace wpi
