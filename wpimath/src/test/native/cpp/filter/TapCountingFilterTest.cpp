// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/timestamp.h>

#include "frc/filter/TapCountFilter.h"
#include "units/time.h"

static units::second_t now = 0_s;

class TapCountFilterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });
  }

  void TearDown() override { WPI_SetNowImpl(nullptr); }
};

TEST_F(TapCountFilterTest, TapCountFilterActivated) {
  frc::TapCountFilter filter{2, 0.2_s};

  EXPECT_FALSE(filter.Calculate(true));  // First tap

  now = 50_ms;
  EXPECT_FALSE(filter.Calculate(false));  // First tap ended

  now = 100_ms;
  EXPECT_TRUE(filter.Calculate(true));  // Second tap

  now = 150_ms;
  EXPECT_TRUE(filter.Calculate(true));  // Still true

  now = 250_ms;
  EXPECT_TRUE(filter.Calculate(true));  // Still true

  now = 300_ms;
  EXPECT_FALSE(filter.Calculate(false));  // Input false, should reset
}

TEST_F(TapCountFilterTest, TapCountFilterExpired) {
  frc::TapCountFilter filter{2, 0.2_s};

  EXPECT_FALSE(filter.Calculate(true));  // First tap

  now = 50_ms;
  filter.Calculate(false);  // First tap ended

  now = 250_ms;
  EXPECT_FALSE(filter.Calculate(true));  // Second tap after window expired

  now = 300_ms;
  EXPECT_FALSE(filter.Calculate(true));  // Still false
}

TEST_F(TapCountFilterTest, TapCountFilterParams) {
  frc::TapCountFilter filter{2, 0.2_s};

  EXPECT_EQ(filter.GetRequiredTaps(), 2);
  EXPECT_EQ(filter.GetTapWindow(), 0.2_s);

  filter.SetRequiredTaps(3);

  EXPECT_EQ(filter.GetRequiredTaps(), 3);

  filter.SetTapWindow(0.5_s);

  EXPECT_EQ(filter.GetTapWindow(), 0.5_s);
}
