// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/counter/Tachometer.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/counter/EdgeConfiguration.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/units/time.hpp"

TEST_CASE("Tachometer SetRateWindow", "[wpilibc][counter]") {
  HAL_Initialize();

  wpi::Tachometer tachometer(0, wpi::EdgeConfiguration::RISING_EDGE);

  CHECK_NOTHROW(tachometer.SetRateWindow(wpi::units::millisecond_t{5}));
  CHECK_NOTHROW(tachometer.SetRateWindow(wpi::units::millisecond_t{255}));
  CHECK_THROWS(tachometer.SetRateWindow(wpi::units::millisecond_t{4}));
  CHECK_THROWS(tachometer.SetRateWindow(wpi::units::millisecond_t{256}));
}

TEST_CASE("Tachometer stopped state matches rate", "[wpilibc][counter]") {
  HAL_Initialize();

  wpi::Tachometer tachometer(0, wpi::EdgeConfiguration::RISING_EDGE);

  CHECK(tachometer.GetFrequency().value() == 0.0);
  CHECK(tachometer.GetStopped());
}
