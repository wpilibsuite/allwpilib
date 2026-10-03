// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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

#include "wpi/util/Signal.h"

#include <cmath>
#include <sstream>
#include <string>
#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using namespace wpi::util::sig;

namespace {

int sum = 0;

void f1(int i) {
  sum += i;
}
void f2(int i) /*noexcept*/ {
  sum += 2 * i;
}

struct s {
  static void s1(int i) { sum += i; }
  static void s2(int i) noexcept { sum += 2 * i; }

  void f1(int i) { sum += i; }
  void f2(int i) const { sum += i; }
  void f3(int i) volatile { sum += i; }
  void f4(int i) const volatile { sum += i; }
  void f5(int i) noexcept { sum += i; }
  void f6(int i) const noexcept { sum += i; }
  void f7(int i) volatile noexcept { sum += i; }
  void f8(int i) const volatile noexcept { sum += i; }
};

struct oo {
  void operator()(int i) { sum += i; }
  void operator()(double i) { sum += std::round(4 * i); }
};

struct o1 {
  void operator()(int i) { sum += i; }
};
struct o2 {
  void operator()(int i) const { sum += i; }
};
struct o3 {
  void operator()(int i) volatile { sum += i; }
};
struct o4 {
  void operator()(int i) const volatile { sum += i; }
};
struct o5 {
  void operator()(int i) noexcept { sum += i; }
};
struct o6 {
  void operator()(int i) const noexcept { sum += i; }
};
struct o7 {
  void operator()(int i) volatile noexcept { sum += i; }
};
struct o8 {
  void operator()(int i) const volatile noexcept { sum += i; }
};

}  // namespace

namespace wpi::util {

TEST_CASE("SignalTest FreeConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  auto c1 = sig.connect_connection(f1);
  sig(1);
  REQUIRE(sum == 1);

  sig.connect(f2);
  sig(1);
  REQUIRE(sum == 4);
}

TEST_CASE("SignalTest StaticConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  sig.connect(&s::s1);
  sig(1);
  REQUIRE(sum == 1);

  sig.connect(&s::s2);
  sig(1);
  REQUIRE(sum == 4);
}

TEST_CASE("SignalTest PmfConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;
  s p;

  sig.connect(&s::f1, &p);
  sig.connect(&s::f2, &p);
  sig.connect(&s::f3, &p);
  sig.connect(&s::f4, &p);
  sig.connect(&s::f5, &p);
  sig.connect(&s::f6, &p);
  sig.connect(&s::f7, &p);
  sig.connect(&s::f8, &p);

  sig(1);
  REQUIRE(sum == 8);
}

TEST_CASE("SignalTest ConstPmfConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;
  const s p;

  sig.connect(&s::f2, &p);
  sig.connect(&s::f4, &p);
  sig.connect(&s::f6, &p);
  sig.connect(&s::f8, &p);

  sig(1);
  REQUIRE(sum == 4);
}

TEST_CASE("SignalTest FunctionObjectConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  sig.connect(o1{});
  sig.connect(o2{});
  sig.connect(o3{});
  sig.connect(o4{});
  sig.connect(o5{});
  sig.connect(o6{});
  sig.connect(o7{});
  sig.connect(o8{});

  sig(1);
  REQUIRE(sum == 8);
}

TEST_CASE("SignalTest OverloadedFunctionObjectConnection",
          "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;
  Signal<double> sig1;

  sig.connect(oo{});
  sig(1);
  REQUIRE(sum == 1);

  sig1.connect(oo{});
  sig1(1);
  REQUIRE(sum == 5);
}

TEST_CASE("SignalTest LambdaConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  sig.connect([&](int i) { sum += i; });
  sig(1);
  REQUIRE(sum == 1);

  sig.connect([&](int i) mutable { sum += 2 * i; });
  sig(1);
  REQUIRE(sum == 4);
}

TEST_CASE("SignalTest GenericLambdaConnection", "[wpiutil][sigslot]") {
  std::stringstream s;

  auto f = [&](auto a, auto... args) {
    using result_t = int[];
    s << a;
    result_t r{
        1,
        ((void)(s << args), 1)...,
    };
    (void)r;
  };

  Signal<int> sig1;
  Signal<std::string> sig2;
  Signal<double> sig3;

  sig1.connect(f);
  sig2.connect(f);
  sig3.connect(f);
  sig1(1);
  sig2("foo");
  sig3(4.1);

  REQUIRE(s.str() == "1foo4.1");
}

TEST_CASE("SignalTest LvalueEmission", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  auto c1 = sig.connect_connection(f1);
  int v = 1;
  sig(v);
  REQUIRE(sum == 1);

  sig.connect(f2);
  sig(v);
  REQUIRE(sum == 4);
}

TEST_CASE("SignalTest Mutation", "[wpiutil][sigslot]") {
  int res = 0;
  Signal<int&> sig;

  sig.connect([](int& r) { r += 1; });
  sig(res);
  REQUIRE(res == 1);

  sig.connect([](int& r) mutable { r += 2; });
  sig(res);
  REQUIRE(res == 4);
}

TEST_CASE("SignalTest CompatibleArgs", "[wpiutil][sigslot]") {
  long ll = 0;  // NOLINT(runtime/int)
  std::string ss;
  short ii = 0;  // NOLINT(runtime/int)

  auto f = [&](long l, const std::string& s, short i) {  // NOLINT(runtime/int)
    ll = l;
    ss = s;
    ii = i;
  };

  Signal<int, std::string, bool> sig;
  sig.connect(f);
  sig('0', "foo", true);

  REQUIRE(ll == 48);
  REQUIRE(ss == "foo");
  REQUIRE(ii == 1);
}

TEST_CASE("SignalTest Disconnection", "[wpiutil][sigslot]") {
  // test removing only connected
  {
    sum = 0;
    Signal<int> sig;

    auto sc = sig.connect_connection(f1);
    sig(1);
    REQUIRE(sum == 1);

    sc.disconnect();
    sig(1);
    REQUIRE(sum == 1);
  }

  // test removing first connected
  {
    sum = 0;
    Signal<int> sig;

    auto sc = sig.connect_connection(f1);
    sig(1);
    REQUIRE(sum == 1);

    sig.connect(f2);
    sig(1);
    REQUIRE(sum == 4);

    sc.disconnect();
    sig(1);
    REQUIRE(sum == 6);
  }

  // test removing last connected
  {
    sum = 0;
    Signal<int> sig;

    sig.connect(f1);
    sig(1);
    REQUIRE(sum == 1);

    auto sc = sig.connect_connection(f2);
    sig(1);
    REQUIRE(sum == 4);

    sc.disconnect();
    sig(1);
    REQUIRE(sum == 5);
  }
}

TEST_CASE("SignalTest ScopedConnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  {
    auto sc1 = sig.connect_scoped(f1);
    sig(1);
    REQUIRE(sum == 1);

    auto sc2 = sig.connect_scoped(f2);
    sig(1);
    REQUIRE(sum == 4);
  }

  sig(1);
  REQUIRE(sum == 4);

  sum = 0;

  {
    ScopedConnection sc1 = sig.connect_connection(f1);
    sig(1);
    REQUIRE(sum == 1);

    ScopedConnection sc2 = sig.connect_connection(f2);
    sig(1);
    REQUIRE(sum == 4);
  }

  sig(1);
  REQUIRE(sum == 4);
}

TEST_CASE("SignalTest ConnectionBlocking", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  auto c1 = sig.connect_connection(f1);
  sig.connect(f2);
  sig(1);
  REQUIRE(sum == 3);

  c1.block();
  sig(1);
  REQUIRE(sum == 5);

  c1.unblock();
  sig(1);
  REQUIRE(sum == 8);
}

TEST_CASE("SignalTest ConnectionBlocker", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  auto c1 = sig.connect_connection(f1);
  sig.connect(f2);
  sig(1);
  REQUIRE(sum == 3);

  {
    auto cb = c1.blocker();
    sig(1);
    REQUIRE(sum == 5);
  }

  sig(1);
  REQUIRE(sum == 8);
}

TEST_CASE("SignalTest SignalBlocking", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  sig.connect(f1);
  sig.connect(f2);
  sig(1);
  REQUIRE(sum == 3);

  sig.block();
  sig(1);
  REQUIRE(sum == 3);

  sig.unblock();
  sig(1);
  REQUIRE(sum == 6);
}

TEST_CASE("SignalTest AllDisconnection", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  sig.connect(f1);
  sig.connect(f2);
  sig(1);
  REQUIRE(sum == 3);

  sig.disconnect_all();
  sig(1);
  REQUIRE(sum == 3);
}

TEST_CASE("SignalTest ConnectionCopyingMoving", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  auto sc1 = sig.connect_connection(f1);
  auto sc2 = sig.connect_connection(f2);

  auto sc3 = sc1;
  auto sc4{sc2};

  auto sc5 = std::move(sc3);
  auto sc6{std::move(sc4)};

  sig(1);
  REQUIRE(sum == 3);

  sc5.block();
  sig(1);
  REQUIRE(sum == 5);

  sc1.unblock();
  sig(1);
  REQUIRE(sum == 8);

  sc6.disconnect();
  sig(1);
  REQUIRE(sum == 9);
}

TEST_CASE("SignalTest ScopedConnectionMoving", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  {
    auto sc1 = sig.connect_scoped(f1);
    sig(1);
    REQUIRE(sum == 1);

    auto sc2 = sig.connect_scoped(f2);
    sig(1);
    REQUIRE(sum == 4);

    auto sc3 = std::move(sc1);
    sig(1);
    REQUIRE(sum == 7);

    auto sc4{std::move(sc2)};
    sig(1);
    REQUIRE(sum == 10);
  }

  sig(1);
  REQUIRE(sum == 10);
}

TEST_CASE("SignalTest SignalMoving", "[wpiutil][sigslot]") {
  sum = 0;
  Signal<int> sig;

  sig.connect(f1);
  sig.connect(f2);

  sig(1);
  REQUIRE(sum == 3);

  auto sig2 = std::move(sig);
  sig2(1);
  REQUIRE(sum == 6);

  auto sig3 = std::move(sig2);
  sig3(1);
  REQUIRE(sum == 9);
}

template <typename T>
struct object {
  object();
  object(T i) : v{i} {}  // NOLINT

  const T& val() const { return v; }
  T& val() { return v; }
  void set_val(const T& i) {
    if (i != v) {
      v = i;
      s(i);
    }
  }

  Signal<T>& sig() { return s; }

 private:
  T v;
  Signal<T> s;
};

TEST_CASE("SignalTest Loop", "[wpiutil][sigslot]") {
  object<int> i1(0);
  object<int> i2(3);

  i1.sig().connect(&object<int>::set_val, &i2);
  i2.sig().connect(&object<int>::set_val, &i1);

  i1.set_val(1);

  REQUIRE(i1.val() == 1);
  REQUIRE(i2.val() == 1);
}

}  // namespace wpi::util
