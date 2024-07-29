//===- llvm/unittest/Support/Chrono.cpp - Time utilities tests ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "wpi/Chrono.h"
#include "gtest/gtest.h"

using namespace wpi;
using namespace wpi::sys;
using namespace std::chrono;

namespace {

TEST(Chrono, TimeTConversion) {
  EXPECT_EQ(time_t(0), toTimeT(toTimePoint(time_t(0))));
  EXPECT_EQ(time_t(1), toTimeT(toTimePoint(time_t(1))));
  EXPECT_EQ(time_t(47), toTimeT(toTimePoint(time_t(47))));

  TimePoint<> TP;
  EXPECT_EQ(TP, toTimePoint(toTimeT(TP)));
  TP += seconds(1);
  EXPECT_EQ(TP, toTimePoint(toTimeT(TP)));
  TP += hours(47);
  EXPECT_EQ(TP, toTimePoint(toTimeT(TP)));
}

// Test that toTimePoint and toTimeT can be called with a arguments with varying
// precisions.
TEST(Chrono, ImplicitConversions) {
  std::time_t TimeT = 47;
  TimePoint<seconds> Sec = toTimePoint(TimeT);
  TimePoint<milliseconds> Milli = toTimePoint(TimeT);
  TimePoint<microseconds> Micro = toTimePoint(TimeT);
  TimePoint<nanoseconds> Nano = toTimePoint(TimeT);
  EXPECT_EQ(Sec, Milli);
  EXPECT_EQ(Sec, Micro);
  EXPECT_EQ(Sec, Nano);
  EXPECT_EQ(TimeT, toTimeT(Sec));
  EXPECT_EQ(TimeT, toTimeT(Milli));
  EXPECT_EQ(TimeT, toTimeT(Micro));
  EXPECT_EQ(TimeT, toTimeT(Nano));
}

} // anonymous namespace
