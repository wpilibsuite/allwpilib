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
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/nInterfaceGlobals.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tHMB.h>
#include <FRC_NetworkCommunication/LoadOut.h>
#pragma GCC diagnostic pop
namespace fpga {
using namespace nFPGA;
using namespace nRoboRIO_FPGANamespace;
}  // namespace fpga
#include <memory>

#include "dlfcn.h"
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
namespace {
static constexpr const char hmbName[] = "HMB_0_RAM";
static constexpr int timestampLowerOffset = 0xF0;
static constexpr int timestampUpperOffset = 0xF1;
static constexpr int hmbTimestampOffset = 5;  // 5 us offset
using NiFpga_CloseHmbFunc = NiFpga_Status (*)(const NiFpga_Session session,
                                              const char* memoryName);
using NiFpga_OpenHmbFunc = NiFpga_Status (*)(const NiFpga_Session session,
                                             const char* memoryName,
                                             size_t* memorySize,
                                             void** virtualAddress);
struct HMBHolder {
  ~HMBHolder() {
    if (hmb) {
      closeHmb(hmb->getSystemInterface()->getHandle(), hmbName);
      dlclose(niFpga);
    }
  }
  explicit operator bool() const { return hmb != nullptr; }
  void Configure() {
    nFPGA::nRoboRIO_FPGANamespace::g_currentTargetClass =
        nLoadOut::getTargetClass();
    int32_t status = 0;
    hmb.reset(fpga::tHMB::create(&status));
    niFpga = dlopen("libNiFpga.so", RTLD_LAZY);
    if (!niFpga) {
      hmb = nullptr;
      return;
    }
    NiFpga_OpenHmbFunc openHmb = reinterpret_cast<NiFpga_OpenHmbFunc>(
        dlsym(niFpga, "NiFpgaDll_OpenHmb"));
    closeHmb = reinterpret_cast<NiFpga_CloseHmbFunc>(
        dlsym(niFpga, "NiFpgaDll_CloseHmb"));
    if (openHmb == nullptr || closeHmb == nullptr) {
      closeHmb = nullptr;
      dlclose(niFpga);
      hmb = nullptr;
      return;
    }
    size_t hmbBufferSize = 0;
    status =
        openHmb(hmb->getSystemInterface()->getHandle(), hmbName, &hmbBufferSize,
                reinterpret_cast<void**>(const_cast<uint32_t**>(&hmbBuffer)));
    if (status != 0) {
      closeHmb = nullptr;
      dlclose(niFpga);
      hmb = nullptr;
      return;
    }
    auto cfg = hmb->readConfig(&status);
    cfg.Enables_Timestamp = 1;
    hmb->writeConfig(cfg, &status);
  }
  std::unique_ptr<fpga::tHMB> hmb;
  void* niFpga = nullptr;
  NiFpga_CloseHmbFunc closeHmb = nullptr;
  volatile uint32_t* hmbBuffer = nullptr;
};
static HMBHolder hmb;
}  // namespace
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

void wpi::impl::SetupNowRio() {
#ifdef __FRC_ROBORIO__
  if (!hmb) {
    hmb.Configure();
  }
#endif
}

void wpi::SetNowImpl(uint64_t (*func)(void)) {
  now_impl = func ? func : NowDefault;
}

uint64_t wpi::Now() {
#ifdef __FRC_ROBORIO__
  // Same code as HAL_GetFPGATime()
  if (!hmb) {
    std::fputs(
        "FPGA not yet configured in wpi::Now(). Time will not be correct",
        stderr);
    std::fflush(stderr);
    return 0;
  }

  asm("dmb");
  uint64_t upper1 = hmb.hmbBuffer[timestampUpperOffset];
  asm("dmb");
  uint32_t lower = hmb.hmbBuffer[timestampLowerOffset];
  asm("dmb");
  uint64_t upper2 = hmb.hmbBuffer[timestampUpperOffset];

  if (upper1 != upper2) {
    // Rolled over between the lower call, reread lower
    asm("dmb");
    lower = hmb.hmbBuffer[timestampLowerOffset];
  }
  // 5 is added here because the time to write from the FPGA
  // to the HMB buffer is longer then the time to read
  // from the time register. This would cause register based
  // timestamps to be ahead of HMB timestamps, which could
  // be very bad.
  return (upper2 << 32) + lower + hmbTimestampOffset;
#else
  return (now_impl.load())();
#endif
}

uint64_t wpi::GetSystemTime() {
  return time_since_epoch();
}

extern "C" {

void WPI_Impl_SetupNowRio(void) {
  return wpi::impl::SetupNowRio();
}

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
