// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/timestamp.h>

#include "frc/filter/Debouncer.h"
#include "gtest/gtest.h"
#include "units/time.h"

static units::second_t now = 0_s;

class DebouncerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });
  }

  void TearDown() override { WPI_SetNowImpl(nullptr); }
};

TEST_F(DebouncerTest, DebounceRising) {
  frc::Debouncer debouncer{20_ms};

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  now += 1_s;

  EXPECT_TRUE(debouncer.Calculate(true));
}

TEST_F(DebouncerTest, DebounceFalling) {
  frc::Debouncer debouncer{20_ms, frc::Debouncer::DebounceType::kFalling};

  debouncer.Calculate(true);
  EXPECT_TRUE(debouncer.Calculate(false));

  now += 1_s;

  EXPECT_FALSE(debouncer.Calculate(false));
}

TEST_F(DebouncerTest, DebounceBoth) {
  frc::Debouncer debouncer{20_ms, frc::Debouncer::DebounceType::kBoth};

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  now += 1_s;

  EXPECT_TRUE(debouncer.Calculate(true));
  EXPECT_TRUE(debouncer.Calculate(false));

  now += 1_s;

  EXPECT_FALSE(debouncer.Calculate(false));
}
