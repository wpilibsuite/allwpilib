/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <thread>

#include <units/units.h>

#include "frc/Debouncer.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(DebouncerTest, DebounceRisingTest) {
  Debouncer debouncer(20_ms);

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(debouncer.Calculate(true));
}

TEST(DebouncerTest, DebounceFallingTest) {
  Debouncer debouncer(20_ms, Debouncer::DebounceType::kFalling);

  debouncer.Calculate(true);
  EXPECT_TRUE(debouncer.Calculate(false));

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(debouncer.Calculate(false));
}

TEST(DebouncerTest, DebounceBothTest) {
  Debouncer debouncer(20_ms, Debouncer::DebounceType::kBoth);

  debouncer.Calculate(false);
  EXPECT_FALSE(debouncer.Calculate(true));

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_TRUE(debouncer.Calculate(true));
  EXPECT_TRUE(debouncer.Calculate(false));

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_FALSE(debouncer.Calculate(false));
}
