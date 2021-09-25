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

#include "wpi/Signal.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

using namespace wpi::sig;

namespace {

int sum = 0;

void f(Connection& c, int i) {
  sum += i;
  c.disconnect();
}

struct s {
  static void sf(Connection& c, int i) {
    sum += i;
    c.disconnect();
  }
  void f(Connection& c, int i) {
    sum += i;
    c.disconnect();
  }
};

struct o {
  void operator()(Connection& c, int i) {
    sum += i;
    c.disconnect();
  }
};

}  // namespace

namespace wpi {

TEST(SignalExtendedTest, FreeConnection) {
  sum = 0;
  Signal<int> sig;
  sig.connect_extended(f);

  sig(1);
  ASSERT_EQ(sum, 1);
  sig(1);
  ASSERT_EQ(sum, 1);
}

TEST(SignalExtendedTest, StaticConnection) {
  sum = 0;
  Signal<int> sig;
  sig.connect_extended(&s::sf);

  sig(1);
  ASSERT_EQ(sum, 1);
  sig(1);
  ASSERT_EQ(sum, 1);
}

TEST(SignalExtendedTest, PmfConnection) {
  sum = 0;
  Signal<int> sig;
  s p;
  sig.connect_extended(&s::f, &p);

  sig(1);
  ASSERT_EQ(sum, 1);
  sig(1);
  ASSERT_EQ(sum, 1);
}

TEST(SignalExtendedTest, FunctionObjectConnection) {
  sum = 0;
  Signal<int> sig;
  sig.connect_extended(o{});

  sig(1);
  ASSERT_EQ(sum, 1);
  sig(1);
  ASSERT_EQ(sum, 1);
}

TEST(SignalExtendedTest, LambdaConnection) {
  sum = 0;
  Signal<int> sig;

  sig.connect_extended([&](Connection& c, int i) {
    sum += i;
    c.disconnect();
  });
  sig(1);
  ASSERT_EQ(sum, 1);

  sig.connect_extended([&](Connection& c, int i) mutable {
    sum += 2 * i;
    c.disconnect();
  });
  sig(1);
  ASSERT_EQ(sum, 3);
  sig(1);
  ASSERT_EQ(sum, 3);
}

}  // namespace wpi
