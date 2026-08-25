// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/filter/Debouncer.hpp"

#include <stdint.h>

#include <catch2/catch_test_macros.hpp>

#include "wpi/units/time.hpp"
#include "wpi/util/timestamp.h"

static wpi::units::seconds<> now = 0_s;

class DebouncerTest {
 public:
  DebouncerTest() {
    WPI_SetNowImpl([] { return wpi::units::nanoseconds<>{now}.to<int64_t>(); });
  }

  ~DebouncerTest() { WPI_SetNowImpl(nullptr); }
};

TEST_CASE_METHOD(DebouncerTest, "DebouncerTest DebounceRising", "[wpimath]") {
  wpi::math::Debouncer debouncer{20_ms};

  debouncer.Calculate(false);
  CHECK_FALSE(debouncer.Calculate(true));

  now += 1_s;

  CHECK(debouncer.Calculate(true));
}

TEST_CASE_METHOD(DebouncerTest, "DebouncerTest DebounceFalling", "[wpimath]") {
  wpi::math::Debouncer debouncer{20_ms,
                                 wpi::math::Debouncer::DebounceType::FALLING};

  debouncer.Calculate(true);
  CHECK(debouncer.Calculate(false));

  now += 1_s;

  CHECK_FALSE(debouncer.Calculate(false));
}

TEST_CASE_METHOD(DebouncerTest, "DebouncerTest DebounceBoth", "[wpimath]") {
  wpi::math::Debouncer debouncer{20_ms,
                                 wpi::math::Debouncer::DebounceType::BOTH};

  debouncer.Calculate(false);
  CHECK_FALSE(debouncer.Calculate(true));

  now += 1_s;

  CHECK(debouncer.Calculate(true));
  CHECK(debouncer.Calculate(false));

  now += 1_s;

  CHECK_FALSE(debouncer.Calculate(false));
}

TEST_CASE_METHOD(DebouncerTest, "DebouncerTest DebounceParams", "[wpimath]") {
  wpi::math::Debouncer debouncer{20_ms,
                                 wpi::math::Debouncer::DebounceType::BOTH};

  CHECK(debouncer.GetDebounceTime() == 20_ms);
  CHECK(debouncer.GetDebounceType() ==
        wpi::math::Debouncer::DebounceType::BOTH);

  debouncer.SetDebounceTime(100_ms);

  CHECK(debouncer.GetDebounceTime() == 100_ms);

  debouncer.SetDebounceType(wpi::math::Debouncer::DebounceType::FALLING);

  CHECK(debouncer.GetDebounceType() ==
        wpi::math::Debouncer::DebounceType::FALLING);

  CHECK(debouncer.Calculate(false));
}
