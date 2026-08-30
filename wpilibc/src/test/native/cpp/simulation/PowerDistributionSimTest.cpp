// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/PowerDistributionSim.hpp"

#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Ports.h"
#include "wpi/hardware/power/PowerDistribution.hpp"

namespace wpi::sim {

TEST_CASE("PowerDistributionSimTest Initialize", "[wpilibc][simulation]") {
  HAL_Initialize();
  PowerDistributionSim sim{2};
  sim.ResetData();
  CHECK_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PowerDistribution pdp(CANPort::CAN_S0, 2,
                        wpi::PowerDistribution::ModuleType::CTRE);
  CHECK(sim.GetInitialized());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());

  callback.Reset();
  sim.SetInitialized(false);
  CHECK(callback.WasTriggered());
  CHECK_FALSE(callback.GetLastValue());
}

TEST_CASE("PowerDistributionSimTest SetTemperature", "[wpilibc][simulation]") {
  HAL_Initialize();
  PowerDistribution pdp{CANPort::CAN_S0, 2,
                        wpi::PowerDistribution::ModuleType::CTRE};
  PowerDistributionSim sim(pdp);

  DoubleCallback callback;
  auto cb = sim.RegisterTemperatureCallback(callback.GetCallback(), false);

  sim.SetTemperature(35.04);
  CHECK(35.04 == sim.GetTemperature());
  CHECK(35.04 == pdp.GetTemperature());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("PowerDistributionSimTest SetVoltage", "[wpilibc][simulation]") {
  HAL_Initialize();
  PowerDistribution pdp{CANPort::CAN_S0, 2,
                        wpi::PowerDistribution::ModuleType::CTRE};
  PowerDistributionSim sim(pdp);

  DoubleCallback callback;
  auto cb = sim.RegisterVoltageCallback(callback.GetCallback(), false);

  sim.SetVoltage(35.04);
  CHECK(35.04 == sim.GetVoltage());
  CHECK(35.04 == pdp.GetVoltage());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("PowerDistributionSimTest SetCurrent", "[wpilibc][simulation]") {
  HAL_Initialize();
  PowerDistribution pdp{CANPort::CAN_S0, 2,
                        wpi::PowerDistribution::ModuleType::CTRE};
  PowerDistributionSim sim(pdp);

  for (int channel = 0; channel < HAL_GetNumCTREPDPChannels(); ++channel) {
    DoubleCallback callback;
    auto cb =
        sim.RegisterCurrentCallback(channel, callback.GetCallback(), false);

    const double TEST_CURRENT = 35.04 + channel;
    sim.SetCurrent(channel, TEST_CURRENT);
    CHECK(TEST_CURRENT == sim.GetCurrent(channel));
    CHECK(TEST_CURRENT == pdp.GetCurrent(channel));
    CHECK(callback.WasTriggered());
    CHECK(callback.GetLastValue());
  }
}

TEST_CASE("PowerDistributionSimTest GetAllCurrents", "[wpilibc][simulation]") {
  HAL_Initialize();
  PowerDistribution pdp{CANPort::CAN_S0, 2,
                        wpi::PowerDistribution::ModuleType::REV};
  PowerDistributionSim sim(pdp);

  // setup
  for (int channel = 0; channel < pdp.GetNumChannels(); ++channel) {
    const double TEST_CURRENT = 24 - channel;
    sim.SetCurrent(channel, TEST_CURRENT);
  }

  // run it
  std::vector<double> currents = pdp.GetAllCurrents();

  // verify
  for (int channel = 0; channel < pdp.GetNumChannels(); ++channel) {
    const double TEST_CURRENT = 24 - channel;
    CHECK(TEST_CURRENT == currents[channel]);
  }
}

}  // namespace wpi::sim
