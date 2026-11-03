// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/timestamp.h"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/timestamp.hpp"

namespace {

int64_t mockTime = 0;

int64_t MockNow() {
  return mockTime;
}

class ResetNowImpl {
 public:
  ~ResetNowImpl() { wpi::util::SetNowImpl(nullptr); }
};

}  // namespace

TEST_CASE("TimestampTest ProgramStartTimeIsStable", "[wpiutil]") {
  int64_t startTime = wpi::util::GetProgramStartTime();

  CHECK(startTime == wpi::util::GetProgramStartTime());
  CHECK(startTime == WPI_GetProgramStartTime());
  CHECK(startTime <= wpi::util::NowDefault());
}

TEST_CASE("TimestampTest SetNowImplUpdatesProgramStartTime", "[wpiutil]") {
  ResetNowImpl reset;
  int64_t originalStartTime = wpi::util::GetProgramStartTime();

  mockTime = 1234;
  wpi::util::SetNowImpl(MockNow);

  CHECK(1234u == wpi::util::Now());
  CHECK(1234u == wpi::util::GetProgramStartTime());

  mockTime = 5678;

  CHECK(5678u == wpi::util::Now());
  CHECK(1234u == wpi::util::GetProgramStartTime());

  wpi::util::SetNowImpl(nullptr);

  CHECK(originalStartTime == wpi::util::GetProgramStartTime());
}

TEST_CASE("TimestampTest WPISetNowImplNullRestoresProgramStartTime",
          "[wpiutil]") {
  ResetNowImpl reset;
  int64_t originalStartTime = WPI_GetProgramStartTime();

  mockTime = 4321;
  WPI_SetNowImpl(MockNow);

  CHECK(4321u == WPI_Now());
  CHECK(4321u == WPI_GetProgramStartTime());

  WPI_SetNowImpl(nullptr);

  CHECK(originalStartTime == WPI_GetProgramStartTime());
}
