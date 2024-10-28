// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/timestamp.h"

#include <atomic>
#include <optional>
#include <utility>

#ifdef __FRC_ROBORIO__
#include <stdint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include <FRC_FPGA_ChipObject/RoboRIO_FRC_ChipObject_Aliases.h>
#include <FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tHMB.h>
#pragma GCC diagnostic pop
namespace fpga {
using namespace nFPGA;
using namespace nRoboRIO_FPGANamespace;
}  // namespace fpga
#include <memory>

#include "dlfcn.h"  // NOLINT(build/include_subdir)

#endif

#ifdef _WIN32
#include <windows.h>

#include <cassert>
#include <exception>
#else
#include <chrono>
#endif

#include <cstdio>

#include "wpi/print.h"

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
using NiFpga_FindRegisterFunc = NiFpga_Status (*)(NiFpga_Session session,
                                                  const char* registerName,
                                                  uint32_t* registerOffset);
using NiFpga_ReadU32Func = NiFpga_Status (*)(NiFpga_Session session,
                                             uint32_t indicator,
                                             uint32_t* value);
using NiFpga_WriteU32Func = NiFpga_Status (*)(NiFpga_Session session,
                                              uint32_t control, uint32_t value);
static void dlcloseWrapper(void* handle) {
  dlclose(handle);
}
static std::atomic_flag hmbInitialized = ATOMIC_FLAG_INIT;
static std::atomic_flag nowUseDefaultOnFailure = ATOMIC_FLAG_INIT;
struct HMBLowLevel {
  ~HMBLowLevel() { Reset(); }
  bool Configure(const NiFpga_Session session) {
    int32_t status = 0;
    niFpga.reset(dlopen("libNiFpga.so", RTLD_LAZY));
    if (!niFpga) {
      wpi::print(stderr, "Could not open libNiFpga.so\n");
      return false;
    }
    NiFpga_OpenHmbFunc openHmb = reinterpret_cast<NiFpga_OpenHmbFunc>(
        dlsym(niFpga.get(), "NiFpgaDll_OpenHmb"));
    closeHmb = reinterpret_cast<NiFpga_CloseHmbFunc>(
        dlsym(niFpga.get(), "NiFpgaDll_CloseHmb"));
    NiFpga_FindRegisterFunc findRegister =
        reinterpret_cast<NiFpga_FindRegisterFunc>(
            dlsym(niFpga.get(), "NiFpgaDll_FindRegister"));
    NiFpga_ReadU32Func readU32 = reinterpret_cast<NiFpga_ReadU32Func>(
        dlsym(niFpga.get(), "NiFpgaDll_ReadU32"));
    NiFpga_WriteU32Func writeU32 = reinterpret_cast<NiFpga_WriteU32Func>(
        dlsym(niFpga.get(), "NiFpgaDll_WriteU32"));
    if (openHmb == nullptr || closeHmb == nullptr || findRegister == nullptr ||
        writeU32 == nullptr || readU32 == nullptr) {
      wpi::print(stderr, "Could not find HMB symbols in libNiFpga.so\n");
      niFpga = nullptr;
      return false;
    }
    uint32_t hmbConfigRegister = 0;
    status = findRegister(session, "HMB.Config", &hmbConfigRegister);
    if (status != 0) {
      wpi::print(stderr, "Failed to find HMB.Config register, status code {}\n",
                 status);
      closeHmb = nullptr;
      niFpga = nullptr;
      return false;
    }
    size_t hmbBufferSize = 0;
    status =
        openHmb(session, hmbName, &hmbBufferSize,
                reinterpret_cast<void**>(const_cast<uint32_t**>(&hmbBuffer)));
    if (status != 0) {
      wpi::print(stderr, "Failed to open HMB, status code {}\n", status);
      closeHmb = nullptr;
      niFpga = nullptr;
      return false;
    }
    fpga::tHMB::tConfig cfg;
    uint32_t read = 0;
    status = readU32(session, hmbConfigRegister, &read);
    cfg.value = read;
    cfg.Enables_Timestamp = 1;
    status = writeU32(session, hmbConfigRegister, cfg.value);
    hmbSession.emplace(session);
    hmbInitialized.test_and_set();
    return true;
  }
  void Reset() {
    hmbInitialized.clear();
    std::optional<NiFpga_Session> oldSesh;
    hmbSession.swap(oldSesh);
    if (oldSesh.has_value()) {
      closeHmb(oldSesh.value(), hmbName);
      niFpga = nullptr;
    }
  }
  std::optional<NiFpga_Session> hmbSession;
  NiFpga_CloseHmbFunc closeHmb = nullptr;
  volatile uint32_t* hmbBuffer = nullptr;
  std::unique_ptr<void, decltype(&dlcloseWrapper)> niFpga{nullptr,
                                                          dlcloseWrapper};
};
struct HMBHolder {
  bool Configure(void* col, std::unique_ptr<fpga::tHMB> hmbObject) {
    hmb = std::move(hmbObject);
    chipObjectLibrary.reset(col);
    if (!lowLevel.Configure(hmb->getSystemInterface()->getHandle())) {
      hmb = nullptr;
      chipObjectLibrary = nullptr;
      return false;
    }
    return true;
  }
  void Reset() {
    lowLevel.Reset();
    hmb = nullptr;
    chipObjectLibrary = nullptr;
  }
  HMBLowLevel lowLevel;
  std::unique_ptr<fpga::tHMB> hmb;
  std::unique_ptr<void, decltype(&dlcloseWrapper)> chipObjectLibrary{
      nullptr, dlcloseWrapper};
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
#else
  return zerotime_val + timestamp() - offset_val;
#endif
}

static std::atomic<uint64_t (*)()> now_impl{wpi::NowDefault};

void wpi::impl::SetupNowDefaultOnRio() {
#ifdef __FRC_ROBORIO__
  nowUseDefaultOnFailure.test_and_set();
#endif
}

#ifdef __FRC_ROBORIO__
template <>
bool wpi::impl::SetupNowRio(void* chipObjectLibrary,
                            std::unique_ptr<fpga::tHMB> hmbObject) {
  if (!hmbInitialized.test()) {
    return hmb.Configure(chipObjectLibrary, std::move(hmbObject));
  }
  return true;
}
#endif

bool wpi::impl::SetupNowRio(uint32_t session) {
#ifdef __FRC_ROBORIO__
  if (!hmbInitialized.test()) {
    return hmb.lowLevel.Configure(session);
  }
#endif
  return true;
}

void wpi::impl::ShutdownNowRio() {
#ifdef __FRC_ROBORIO__
  hmb.Reset();
#endif
}

void wpi::SetNowImpl(uint64_t (*func)(void)) {
  now_impl = func ? func : NowDefault;
}

uint64_t wpi::Now() {
#ifdef __FRC_ROBORIO__
  // Same code as HAL_GetFPGATime()
  if (!hmbInitialized.test()) {
    if (nowUseDefaultOnFailure.test()) {
      return timestamp() - offset_val;
    } else {
      wpi::print(stderr,
                 "FPGA not yet configured in wpi::Now(). This is a fatal "
                 "error. The process is being terminated.\n");
      std::fflush(stderr);
      // Attempt to force a segfault to get a better java log
      *reinterpret_cast<int*>(0) = 0;
      // If that fails, terminate
      std::terminate();
      return 1;
    }
  }

  asm("dmb");
  uint64_t upper1 = hmb.lowLevel.hmbBuffer[timestampUpperOffset];
  asm("dmb");
  uint32_t lower = hmb.lowLevel.hmbBuffer[timestampLowerOffset];
  asm("dmb");
  uint64_t upper2 = hmb.lowLevel.hmbBuffer[timestampUpperOffset];

  if (upper1 != upper2) {
    // Rolled over between the lower call, reread lower
    asm("dmb");
    lower = hmb.lowLevel.hmbBuffer[timestampLowerOffset];
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

void WPI_Impl_SetupNowUseDefaultOnRio(void) {
  return wpi::impl::SetupNowDefaultOnRio();
}

void WPI_Impl_SetupNowRioWithSession(uint32_t session) {
  wpi::impl::SetupNowRio(session);
}

void WPI_Impl_ShutdownNowRio(void) {
  return wpi::impl::ShutdownNowRio();
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
