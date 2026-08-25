// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/rotation/AnalogPotentiometer.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/simulation/AnalogInputSim.hpp"
#include "wpi/simulation/RoboRioSim.hpp"

namespace wpi {
using namespace wpi::sim;

static void ResetAnalogPotentiometerTestData(int channel) {
  RoboRioSim::ResetData();
  AnalogInputSim{channel}.ResetData();
}

TEST_CASE("AnalogPotentiometerTest InitializeWithAnalogInput", "[wpilibc]") {
  HAL_Initialize();
  ResetAnalogPotentiometerTestData(0);

  AnalogInput ai{0};
  AnalogPotentiometer pot{&ai};
  AnalogInputSim sim{ai};

  sim.SetVoltage(2.8);
  CHECK(2.8 / 3.3 == pot.Get());
}

TEST_CASE("AnalogPotentiometerTest InitializeWithAnalogInputAndScale",
          "[wpilibc]") {
  HAL_Initialize();
  ResetAnalogPotentiometerTestData(0);

  AnalogInput ai{0};
  AnalogPotentiometer pot{&ai, 270.0};
  AnalogInputSim sim{ai};

  sim.SetVoltage(3.3);
  CHECK(270.0 == pot.Get());

  sim.SetVoltage(2.5);
  CHECK(2.5 / 3.3 * 270.0 == pot.Get());

  sim.SetVoltage(0.0);
  CHECK(0.0 == pot.Get());
}

TEST_CASE("AnalogPotentiometerTest InitializeWithChannel", "[wpilibc]") {
  HAL_Initialize();
  ResetAnalogPotentiometerTestData(1);

  AnalogPotentiometer pot{1};
  AnalogInputSim sim{1};

  sim.SetVoltage(3.3);
  CHECK(1.0 == pot.Get());
}

TEST_CASE("AnalogPotentiometerTest InitializeWithChannelAndScale",
          "[wpilibc]") {
  HAL_Initialize();
  ResetAnalogPotentiometerTestData(1);

  AnalogPotentiometer pot{1, 180.0};
  AnalogInputSim sim{1};

  sim.SetVoltage(3.3);
  CHECK(180.0 == pot.Get());

  sim.SetVoltage(0.0);
  CHECK(0.0 == pot.Get());
}

TEST_CASE("AnalogPotentiometerTest WithModifiedBatteryVoltage", "[wpilibc]") {
  HAL_Initialize();
  ResetAnalogPotentiometerTestData(1);

  AnalogPotentiometer pot{1, 180.0, 90.0};
  AnalogInputSim sim{1};

  // Test at 3.3v
  sim.SetVoltage(3.3);
  CHECK(270 == pot.Get());

  sim.SetVoltage(0.0);
  CHECK(90 == pot.Get());

  // Simulate a lower battery voltage
  RoboRioSim::SetUserVoltage3V3(wpi::units::volts<>{2.5});

  sim.SetVoltage(2.5);
  CHECK(270.0 == pot.Get());

  sim.SetVoltage(2.0);
  CHECK(234.0 == pot.Get());

  sim.SetVoltage(0.0);
  CHECK(90.0 == pot.Get());
}
}  // namespace wpi
