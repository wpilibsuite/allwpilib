// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/timestamp.hpp"

#include <atomic>
#include <chrono>

#include "wpi/util/timestamp.h"

static uint64_t timestamp() noexcept {
  // 1-us intervals
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

static const uint64_t original_program_start_time = timestamp();
static std::atomic<uint64_t> program_start_time{original_program_start_time};

uint64_t wpi::util::NowDefault() {
  return timestamp();
}

static std::atomic<uint64_t (*)()> now_impl{wpi::util::NowDefault};

void wpi::util::SetNowImpl(uint64_t (*func)(void)) {
  if (!func || func == wpi::util::NowDefault || func == WPI_NowDefault) {
    now_impl = wpi::util::NowDefault;
    program_start_time = original_program_start_time;
  } else {
    now_impl = func;
    program_start_time = func();
  }
}

uint64_t wpi::util::Now() {
  return (now_impl.load())();
}

uint64_t wpi::util::GetProgramStartTime() {
  return program_start_time;
}

uint64_t wpi::util::GetSystemTime() {
  // 1-us intervals
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

extern "C" {

uint64_t WPI_NowDefault(void) {
  return wpi::util::NowDefault();
}

void WPI_SetNowImpl(uint64_t (*func)(void)) {
  wpi::util::SetNowImpl(func);
}

uint64_t WPI_Now(void) {
  return wpi::util::Now();
}

uint64_t WPI_GetProgramStartTime(void) {
  return wpi::util::GetProgramStartTime();
}

uint64_t WPI_GetSystemTime(void) {
  return wpi::util::GetSystemTime();
}

}  // extern "C"
