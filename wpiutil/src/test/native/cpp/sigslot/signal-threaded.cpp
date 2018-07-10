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

#include <array>
#include <atomic>
#include <thread>

using namespace wpi::sig;

namespace {

std::atomic<int> sum{0};

void f(int i) { sum += i; }

void emit_many(Signal_mt<int>& sig) {
  for (int i = 0; i < 10000; ++i) sig(1);
}

void connect_emit(Signal_mt<int>& sig) {
  for (int i = 0; i < 100; ++i) {
    auto s = sig.connect_scoped(f);
    for (int j = 0; j < 100; ++j) sig(1);
  }
}

}  // namespace

namespace wpi {

TEST(Signal, ThreadedMix) {
  sum = 0;

  Signal_mt<int> sig;

  std::array<std::thread, 10> threads;
  for (auto& t : threads) t = std::thread(connect_emit, std::ref(sig));

  for (auto& t : threads) t.join();
}

TEST(Signal, ThreadedEmission) {
  sum = 0;

  Signal_mt<int> sig;
  sig.connect(f);

  std::array<std::thread, 10> threads;
  for (auto& t : threads) t = std::thread(emit_many, std::ref(sig));

  for (auto& t : threads) t.join();

  ASSERT_EQ(sum, 100000);
}

}  // namespace wpi
