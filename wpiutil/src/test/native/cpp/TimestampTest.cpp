// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/timestamp.h"

#include <gtest/gtest.h>

#include "wpi/util/timestamp.hpp"

namespace {

uint64_t mockTime = 0;

uint64_t MockNow() {
  return mockTime;
}

class ResetNowImpl {
 public:
  ~ResetNowImpl() { wpi::util::SetNowImpl(nullptr); }
};

}  // namespace

TEST(TimestampTest, ProgramStartTimeIsStable) {
  uint64_t startTime = wpi::util::GetProgramStartTime();

  EXPECT_EQ(startTime, wpi::util::GetProgramStartTime());
  EXPECT_EQ(startTime, WPI_GetProgramStartTime());
  EXPECT_LE(startTime, wpi::util::NowDefault());
}

TEST(TimestampTest, SetNowImplUpdatesProgramStartTime) {
  ResetNowImpl reset;
  uint64_t originalStartTime = wpi::util::GetProgramStartTime();

  mockTime = 1234;
  wpi::util::SetNowImpl(MockNow);

  EXPECT_EQ(1234u, wpi::util::Now());
  EXPECT_EQ(1234u, wpi::util::GetProgramStartTime());

  mockTime = 5678;

  EXPECT_EQ(5678u, wpi::util::Now());
  EXPECT_EQ(1234u, wpi::util::GetProgramStartTime());

  wpi::util::SetNowImpl(nullptr);

  EXPECT_EQ(originalStartTime, wpi::util::GetProgramStartTime());
}

TEST(TimestampTest, WPISetNowImplDefaultRestoresProgramStartTime) {
  ResetNowImpl reset;
  uint64_t originalStartTime = WPI_GetProgramStartTime();

  mockTime = 4321;
  WPI_SetNowImpl(MockNow);

  EXPECT_EQ(4321u, WPI_Now());
  EXPECT_EQ(4321u, WPI_GetProgramStartTime());

  WPI_SetNowImpl(WPI_NowDefault);

  EXPECT_EQ(originalStartTime, WPI_GetProgramStartTime());
}
