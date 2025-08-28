// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/timestamp.h>

#include "units/time.h"
#include "wpimath/filter/Debouncer.h"

static units::second_t now = 0_s;

class DebouncerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });
  }

  void TearDown() override { WPI_SetNowImpl(nullptr); }
};

TEST_F(DebouncerTest, DebounceRising) {
  wpimath::Debouncer debouncer{20_ms};

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  now += 1_s;

  EXPECT_TRUE(debouncer.Calculate(true));
}

TEST_F(DebouncerTest, DebounceFalling) {
  wpimath::Debouncer debouncer{20_ms,
                               wpimath::Debouncer::DebounceType::kFalling};

  debouncer.Calculate(true);
  EXPECT_TRUE(debouncer.Calculate(false));

  now += 1_s;

  EXPECT_FALSE(debouncer.Calculate(false));
}

TEST_F(DebouncerTest, DebounceBoth) {
  wpimath::Debouncer debouncer{20_ms, wpimath::Debouncer::DebounceType::kBoth};

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  now += 1_s;

  EXPECT_TRUE(debouncer.Calculate(true));
  EXPECT_TRUE(debouncer.Calculate(false));

  now += 1_s;

  EXPECT_FALSE(debouncer.Calculate(false));
}

TEST_F(DebouncerTest, DebounceParams) {
  wpimath::Debouncer debouncer{20_ms, wpimath::Debouncer::DebounceType::kBoth};

  EXPECT_TRUE(debouncer.GetDebounceTime() == 20_ms);
  EXPECT_TRUE(debouncer.GetDebounceType() ==
              wpimath::Debouncer::DebounceType::kBoth);

  debouncer.SetDebounceTime(100_ms);

  EXPECT_TRUE(debouncer.GetDebounceTime() == 100_ms);

  debouncer.SetDebounceType(wpimath::Debouncer::DebounceType::kFalling);

  EXPECT_TRUE(debouncer.GetDebounceType() ==
              wpimath::Debouncer::DebounceType::kFalling);

  EXPECT_TRUE(debouncer.Calculate(false));
}
