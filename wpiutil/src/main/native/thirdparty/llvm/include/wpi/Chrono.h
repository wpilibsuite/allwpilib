//===- llvm/Support/Chrono.h - Utilities for Timing Manipulation-*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef WPIUTIL_WPI_CHRONO_H
#define WPIUTIL_WPI_CHRONO_H

#include "wpi/Compiler.h"

#include <chrono>
#include <ctime>
#include <ratio>

namespace wpi {

class raw_ostream;

namespace sys {

/// A time point on the system clock. This is provided for two reasons:
/// - to insulate us against subtle differences in behavior to differences in
///   system clock precision (which is implementation-defined and differs
///   between platforms).
/// - to shorten the type name
/// The default precision is nanoseconds. If you need a specific precision
/// specify it explicitly. If unsure, use the default. If you need a time point
/// on a clock other than the system_clock, use std::chrono directly.
template <typename D = std::chrono::nanoseconds>
using TimePoint = std::chrono::time_point<std::chrono::system_clock, D>;

/// Convert a TimePoint to std::time_t
inline std::time_t toTimeT(TimePoint<> TP) {
  using namespace std::chrono;
  return system_clock::to_time_t(
      time_point_cast<system_clock::time_point::duration>(TP));
}

/// Convert a std::time_t to a TimePoint
inline TimePoint<std::chrono::seconds>
toTimePoint(std::time_t T) {
  using namespace std::chrono;
  return time_point_cast<seconds>(system_clock::from_time_t(T));
}

/// Convert a std::time_t + nanoseconds to a TimePoint
inline TimePoint<>
toTimePoint(std::time_t T, uint32_t nsec) {
  using namespace std::chrono;
  return time_point_cast<nanoseconds>(system_clock::from_time_t(T))
    + nanoseconds(nsec);
}

} // namespace sys

raw_ostream &operator<<(raw_ostream &OS, sys::TimePoint<> TP);

} // namespace wpi

#endif // WPIUTIL_WPI_CHRONO_H
