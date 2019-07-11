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

#include <cmath>
#include <sstream>
#include <string>

using namespace wpi::sig;

namespace {

int sum = 0;

void f1(int i) { sum += i; }
struct o1 {
  void operator()(int i) { sum += 2 * i; }
};

struct s {
  void f1(int i) { sum += i; }
  void f2(int i) const { sum += 2 * i; }
};

struct oo {
  void operator()(int i) { sum += i; }
  void operator()(double i) { sum += std::round(4 * i); }
};

struct dummy {};

static_assert(trait::is_callable_v<trait::typelist<int>, decltype(&s::f1),
                                   std::shared_ptr<s>>,
              "");

}  // namespace

namespace wpi {

TEST(Signal, TrackShared) {
  sum = 0;
  Signal<int> sig;

  auto s1 = std::make_shared<s>();
  sig.connect(&s::f1, s1);

  auto s2 = std::make_shared<s>();
  std::weak_ptr<s> w2 = s2;
  sig.connect(&s::f2, w2);

  sig(1);
  ASSERT_EQ(sum, 3);

  s1.reset();
  sig(1);
  ASSERT_EQ(sum, 5);

  s2.reset();
  sig(1);
  ASSERT_EQ(sum, 5);
}

TEST(Signal, TrackOther) {
  sum = 0;
  Signal<int> sig;

  auto d1 = std::make_shared<dummy>();
  sig.connect(f1, d1);

  auto d2 = std::make_shared<dummy>();
  std::weak_ptr<dummy> w2 = d2;
  sig.connect(o1(), w2);

  sig(1);
  ASSERT_EQ(sum, 3);

  d1.reset();
  sig(1);
  ASSERT_EQ(sum, 5);

  d2.reset();
  sig(1);
  ASSERT_EQ(sum, 5);
}

TEST(Signal, TrackOverloadedFunctionObject) {
  sum = 0;
  Signal<int> sig;
  Signal<double> sig1;

  auto d1 = std::make_shared<dummy>();
  sig.connect(oo{}, d1);
  sig(1);
  ASSERT_EQ(sum, 1);

  d1.reset();
  sig(1);
  ASSERT_EQ(sum, 1);

  auto d2 = std::make_shared<dummy>();
  std::weak_ptr<dummy> w2 = d2;
  sig1.connect(oo{}, w2);
  sig1(1);
  ASSERT_EQ(sum, 5);

  d2.reset();
  sig1(1);
  ASSERT_EQ(sum, 5);
}

TEST(Signal, TrackGenericLambda) {
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

  auto d1 = std::make_shared<dummy>();
  sig1.connect(f, d1);
  sig2.connect(f, d1);
  sig3.connect(f, d1);

  sig1(1);
  sig2("foo");
  sig3(4.1);
  ASSERT_EQ(s.str(), "1foo4.1");

  d1.reset();
  sig1(2);
  sig2("bar");
  sig3(3.0);
  ASSERT_EQ(s.str(), "1foo4.1");
}

}  // namespace wpi
