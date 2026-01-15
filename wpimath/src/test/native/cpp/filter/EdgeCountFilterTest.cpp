// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/timestamp.h>

#include "frc/filter/EdgeCountFilter.h"
#include "units/time.h"

static units::second_t now = 0_s;

class EdgeCounterFilterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });
    now = 0_ms;
  }

  void TearDown() override {
    now = 0_ms;
    WPI_SetNowImpl(nullptr);
  }
};

TEST_F(EdgeCounterFilterTest, EdgeCounterFilterActivated) {
  frc::EdgeCounterFilter filter{2, 0.2_s};

  EXPECT_FALSE(filter.Calculate(true));  // First edge

  now = 50_ms;
  EXPECT_FALSE(filter.Calculate(false));  // First edge ended

  now = 100_ms;
  EXPECT_TRUE(filter.Calculate(true));  // Second edge

  now = 150_ms;
  EXPECT_TRUE(filter.Calculate(true));  // Still true

  now = 250_ms;
  EXPECT_TRUE(filter.Calculate(true));  // Still true

  now = 300_ms;
  EXPECT_FALSE(filter.Calculate(false));  // Input false, should reset
}

TEST_F(EdgeCounterFilterTest, EdgeCounterFilterExpired) {
  frc::EdgeCounterFilter filter{2, 0.2_s};

  EXPECT_FALSE(filter.Calculate(true));  // First edge

  now = 50_ms;
  filter.Calculate(false);  // First edge ended

  now = 250_ms;
  EXPECT_FALSE(filter.Calculate(true));  // Second edge after window expired

  now = 300_ms;
  EXPECT_FALSE(filter.Calculate(true));  // Still false
}

TEST_F(EdgeCounterFilterTest, EdgeCounterFilterParams) {
  frc::EdgeCounterFilter filter{2, 0.2_s};

  EXPECT_EQ(filter.GetRequiredEdges(), 2);
  EXPECT_EQ(filter.GetWindowTime(), 0.2_s);

  filter.SetRequiredEdges(3);

  EXPECT_EQ(filter.GetRequiredEdges(), 3);

  filter.SetWindowTime(0.5_s);

  EXPECT_EQ(filter.GetWindowTime(), 0.5_s);
}
