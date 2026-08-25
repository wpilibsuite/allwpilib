// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/counter/Tachometer.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/hardware/counter/EdgeConfiguration.hpp"
#include "wpi/units/time.hpp"

TEST_CASE("Tachometer SetRateWindow", "[wpilibc][counter]") {
  HAL_Initialize();

  wpi::Tachometer tachometer(0, wpi::EdgeConfiguration::RISING_EDGE);

  CHECK_NOTHROW(tachometer.SetRateWindow(wpi::units::milliseconds<>{5}));
  CHECK_NOTHROW(tachometer.SetRateWindow(wpi::units::milliseconds<>{255}));
  CHECK_THROWS(tachometer.SetRateWindow(wpi::units::milliseconds<>{4}));
  CHECK_THROWS(tachometer.SetRateWindow(wpi::units::milliseconds<>{256}));
}

TEST_CASE("Tachometer stopped state matches rate", "[wpilibc][counter]") {
  HAL_Initialize();

  wpi::Tachometer tachometer(0, wpi::EdgeConfiguration::RISING_EDGE);

  CHECK(tachometer.GetFrequency().value() == 0.0);
  CHECK(tachometer.GetStopped());
}
