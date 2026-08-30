// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/timestamp.hpp"

#include <atomic>
#include <chrono>

#include "wpi/util/timestamp.h"

static int64_t timestamp() noexcept {
  // 1-ns intervals
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

static const int64_t original_program_start_time = timestamp();
static std::atomic<int64_t> program_start_time{original_program_start_time};

int64_t wpi::util::NowDefault() {
  return timestamp();
}

static std::atomic<int64_t (*)()> now_impl{wpi::util::NowDefault};

void wpi::util::SetNowImpl(int64_t (*func)(void)) {
  if (!func) {
    now_impl = wpi::util::NowDefault;
    program_start_time = original_program_start_time;
  } else {
    now_impl = func;
    program_start_time = func();
  }
}

int64_t wpi::util::Now() {
  return (now_impl.load())();
}

int64_t wpi::util::GetProgramStartTime() {
  return program_start_time;
}

int64_t wpi::util::GetSystemTime() {
  // 1-ns intervals
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

extern "C" {

int64_t WPI_NowDefault(void) {
  return wpi::util::NowDefault();
}

void WPI_SetNowImpl(int64_t (*func)(void)) {
  wpi::util::SetNowImpl(func);
}

int64_t WPI_Now(void) {
  return wpi::util::Now();
}

int64_t WPI_GetProgramStartTime(void) {
  return wpi::util::GetProgramStartTime();
}

int64_t WPI_GetSystemTime(void) {
  return wpi::util::GetSystemTime();
}

}  // extern "C"
