// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/timestamp.h"

#include <atomic>
#include <optional>
#include <utility>

#ifdef _WIN32
#include <windows.h>

#include <cassert>
#include <exception>
#else
#include <chrono>
#endif

#include <cstdio>

#include "wpi/print.h"

// offset in microseconds
static uint64_t time_since_epoch() noexcept {
#ifdef _WIN32
  FILETIME ft;
  uint64_t tmpres = 0;
  // 100-nanosecond intervals since January 1, 1601 (UTC)
  // which means 0.1 us
  GetSystemTimePreciseAsFileTime(&ft);
  tmpres |= ft.dwHighDateTime;
  tmpres <<= 32;
  tmpres |= ft.dwLowDateTime;
  tmpres /= 10u;  // convert to us
  // January 1st, 1970 - January 1st, 1601 UTC ~ 369 years
  // or 11644473600000000 us
  static const uint64_t deltaepoch = 11644473600000000ull;
  tmpres -= deltaepoch;
  return tmpres;
#else
  // 1-us intervals
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
#endif
}

static uint64_t timestamp() noexcept {
#ifdef _WIN32
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  // there is an imprecision with the initial value,
  // but what matters is that timestamps are monotonic and consistent
  return static_cast<uint64_t>(li.QuadPart);
#else
  // 1-us intervals
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
#endif
}

#ifdef _WIN32
static uint64_t update_frequency() {
  LARGE_INTEGER li;
  if (!QueryPerformanceFrequency(&li) || !li.QuadPart) {
    // log something
    std::terminate();
  }
  return static_cast<uint64_t>(li.QuadPart);
}
#endif

static const uint64_t zerotime_val = time_since_epoch();
static const uint64_t offset_val = timestamp();
#ifdef _WIN32
static const uint64_t frequency_val = update_frequency();
#endif

uint64_t wpi::NowDefault() {
#ifdef _WIN32
  assert(offset_val > 0u);
  assert(frequency_val > 0u);
  uint64_t delta = timestamp() - offset_val;
  // because the frequency is in update per seconds, we have to multiply the
  // delta by 1,000,000
  uint64_t delta_in_us = delta * 1000000ull / frequency_val;
  return delta_in_us + zerotime_val;
#elif defined(__FRC_SYSTEMCORE__)
  // We want clock synchronized across the system, so just use steady_clock.
  return timestamp();
#else
  return zerotime_val + timestamp() - offset_val;
#endif
}

static std::atomic<uint64_t (*)()> now_impl{wpi::NowDefault};

void wpi::SetNowImpl(uint64_t (*func)(void)) {
  now_impl = func ? func : NowDefault;
}

uint64_t wpi::Now() {
  return (now_impl.load())();
}

uint64_t wpi::GetSystemTime() {
  return time_since_epoch();
}

extern "C" {

uint64_t WPI_NowDefault(void) {
  return wpi::NowDefault();
}

void WPI_SetNowImpl(uint64_t (*func)(void)) {
  wpi::SetNowImpl(func);
}

uint64_t WPI_Now(void) {
  return wpi::Now();
}

uint64_t WPI_GetSystemTime(void) {
  return wpi::GetSystemTime();
}

}  // extern "C"
