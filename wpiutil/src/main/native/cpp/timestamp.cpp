// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/timestamp.h"

#include <atomic>

#ifdef __FRC_ROBORIO__

#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

#include <FRC_FPGA_ChipObject/RoboRIO_FRC_ChipObject_Aliases.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tGlobal.h>

#pragma GCC diagnostic pop

namespace fpga {
using namespace nFPGA;
using namespace nRoboRIO_FPGANamespace;
}  // namespace fpga

#include <memory>

#endif

#ifdef _WIN32
#include <windows.h>

#include <cassert>
#include <exception>
#else
#include <chrono>
#endif

#include <cstdio>

#include "fmt/format.h"

#ifdef __FRC_ROBORIO__
static std::unique_ptr<fpga::tGlobal> global;
#endif

// offset in microseconds
static uint64_t time_since_epoch() noexcept {
#ifdef _WIN32
  FILETIME ft;
  uint64_t tmpres = 0;
  // 100-nanosecond intervals since January 1, 1601 (UTC)
  // which means 0.1 us
  GetSystemTimeAsFileTime(&ft);
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
#else
  return zerotime_val + timestamp() - offset_val;
#endif
}

static std::atomic<uint64_t (*)()> now_impl{wpi::NowDefault};

#ifdef __FRC_ROBORIO__
namespace wpi {
void SetupNowRio() {
  if (!global) {
    int32_t status = 0;
    global.reset(fpga::tGlobal::create(&status));
  }
}
}  // namespace wpi
#endif

void wpi::SetNowImpl(uint64_t (*func)(void)) {
  now_impl = func ? func : NowDefault;
}

uint64_t wpi::Now() {
#ifdef __FRC_ROBORIO__
  // Same code as HAL_GetFPGATime()
  if (!global) {
    std::fputs(
        "FPGA not yet configured in wpi::Now(). Time will not be correct",
        stderr);
    std::fflush(stderr);
    return 0;
  }
  int32_t status = 0;
  uint64_t upper1 = global->readLocalTimeUpper(&status);
  uint32_t lower = global->readLocalTime(&status);
  uint64_t upper2 = global->readLocalTimeUpper(&status);
  if (status != 0) {
    goto err;
  }
  if (upper1 != upper2) {
    // Rolled over between the lower call, reread lower
    lower = global->readLocalTime(&status);
    if (status != 0) {
      goto err;
    }
  }
  return (upper2 << 32) + lower;

err:
  fmt::print(stderr,
             "Call to FPGA failed in wpi::Now() with status {}. "
             "Initialization might have failed. Time will not be correct\n",
             status);
  std::fflush(stderr);
  return 0;
#else
  return (now_impl.load())();
#endif
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
