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

#include "gtest/gtest.h"

namespace {

template <typename T>
struct object {
  object(T i) : v{i} {}  // NOLINT(runtime/explicit)

  void inc_val(const T& i) {
    if (i != v) {
      v++;
      sig(v);
    }
  }

  void dec_val(const T& i) {
    if (i != v) {
      v--;
      sig(v);
    }
  }

  T v;
  wpi::sig::Signal_r<T> sig;
};

}  // namespace

namespace wpi {

TEST(Signal, Recursive) {
  object<int> i1(-1);
  object<int> i2(10);

  i1.sig.connect(&object<int>::dec_val, &i2);
  i2.sig.connect(&object<int>::inc_val, &i1);

  i1.inc_val(0);

  ASSERT_EQ(i1.v, i2.v);
}

TEST(Signal, SelfRecursive) {
  int i = 0;

  wpi::sig::Signal_r<int> s;
  s.connect([&](int v) {
    if (i < 10) {
      i++;
      s(v + 1);
    }
  });

  s(0);

  ASSERT_EQ(i, 10);
}

}  // namespace wpi
