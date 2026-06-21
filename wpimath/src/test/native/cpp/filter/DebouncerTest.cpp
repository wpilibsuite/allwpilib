// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/filter/Debouncer.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/time.hpp"
#include "wpi/util/timestamp.h"

static wpi::units::second_t now = 0_s;

class DebouncerTest {
 protected:
  DebouncerTest() {
    WPI_SetNowImpl(
        [] { return wpi::units::microsecond_t{now}.to<uint64_t>(); });
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
                                 wpi::math::Debouncer::DebounceType::kFalling};

  debouncer.Calculate(true);
  CHECK(debouncer.Calculate(false));

  now += 1_s;

  CHECK_FALSE(debouncer.Calculate(false));
}

TEST_CASE_METHOD(DebouncerTest, "DebouncerTest DebounceBoth", "[wpimath]") {
  wpi::math::Debouncer debouncer{20_ms,
                                 wpi::math::Debouncer::DebounceType::kBoth};

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
                                 wpi::math::Debouncer::DebounceType::kBoth};

  CHECK(debouncer.GetDebounceTime() == 20_ms);
  CHECK(debouncer.GetDebounceType() ==
        wpi::math::Debouncer::DebounceType::kBoth);

  debouncer.SetDebounceTime(100_ms);

  CHECK(debouncer.GetDebounceTime() == 100_ms);

  debouncer.SetDebounceType(wpi::math::Debouncer::DebounceType::kFalling);

  CHECK(debouncer.GetDebounceType() ==
        wpi::math::Debouncer::DebounceType::kFalling);

  CHECK(debouncer.Calculate(false));
}
