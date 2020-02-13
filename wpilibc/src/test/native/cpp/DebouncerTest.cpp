// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Debouncer.h"  // NOLINT(build/include_order)

#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(DebouncerTest, DebounceRising) {
  Debouncer debouncer{20_ms};

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  frc::sim::StepTiming(100_ms);

  EXPECT_TRUE(debouncer.Calculate(true));
}

TEST(DebouncerTest, DebounceFalling) {
  Debouncer debouncer{20_ms, Debouncer::DebounceType::kFalling};

  debouncer.Calculate(true);
  EXPECT_TRUE(debouncer.Calculate(false));

  frc::sim::StepTiming(100_ms);

  EXPECT_FALSE(debouncer.Calculate(false));
}

TEST(DebouncerTest, DebounceBoth) {
  Debouncer debouncer{20_ms, Debouncer::DebounceType::kBoth};

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  frc::sim::StepTiming(100_ms);

  EXPECT_TRUE(debouncer.Calculate(true));
  EXPECT_TRUE(debouncer.Calculate(false));

  frc::sim::StepTiming(100_ms);

  EXPECT_FALSE(debouncer.Calculate(false));
}
