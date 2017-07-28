/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
#include "support/timestamp.h"

#ifdef _WIN32
#include <cassert>
#include <exception>
#include <windows.h>
#else
#include <chrono>
#endif

// offset in microseconds
static unsigned long long zerotime() {
#ifdef _WIN32
  FILETIME ft;
  unsigned long long tmpres = 0;
  // 100-nanosecond intervals since January 1, 1601 (UTC)
  // which means 0.1 us
  GetSystemTimeAsFileTime(&ft);
  tmpres |= ft.dwHighDateTime;
  tmpres <<= 32;
  tmpres |= ft.dwLowDateTime;
  // January 1st, 1970 - January 1st, 1601 UTC ~ 369 years
  // or 116444736000000000 us
  static const unsigned long long deltaepoch = 116444736000000000ull;
  tmpres -= deltaepoch;
  return tmpres;
#else
  // 100-ns intervals
  using namespace std::chrono;
  return duration_cast<nanoseconds>(
    high_resolution_clock::now().time_since_epoch()).count() / 100u;
#endif
}

static unsigned long long timestamp() {
#ifdef _WIN32
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  // there is an imprecision with the initial value,
  // but what matters is that timestamps are monotonic and consistent
  return static_cast<unsigned long long>(li.QuadPart);
#else
  // 100-ns intervals
  using namespace std::chrono;
  return duration_cast<nanoseconds>(
    steady_clock::now().time_since_epoch()).count() / 100u;
#endif
}

#ifdef _WIN32
static unsigned long long update_frequency() {
  LARGE_INTEGER li;
  if (!QueryPerformanceFrequency(&li) || !li.QuadPart) {
    // log something
    std::terminate();
  }
  return static_cast<unsigned long long>(li.QuadPart);
}
#endif

static const unsigned long long zerotime_val = zerotime();
static const unsigned long long offset_val = timestamp();
#ifdef _WIN32
static const unsigned long long frequency_val = update_frequency();
#endif

unsigned long long wpi::Now() {
#ifdef _WIN32
  assert(offset_val > 0u);
  assert(frequency_val > 0u);
  unsigned long long delta = timestamp() - offset_val;
  // because the frequency is in update per seconds, we have to multiply the
  // delta by 10,000,000
  unsigned long long delta_in_us = delta * 10000000ull / frequency_val;
  return delta_in_us + zerotime_val;
#else
  return zerotime_val + timestamp() - offset_val;
#endif
}

unsigned long long WPI_Now() {
  return wpi::Now();
}
