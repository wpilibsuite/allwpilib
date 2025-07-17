// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "frc/AnalogPotentiometer.h"
#include "frc/simulation/AnalogInputSim.h"
#include "frc/simulation/RoboRioSim.h"

namespace frc {
using namespace frc::sim;
TEST(AnalogPotentiometerTest, InitializeWithAnalogInput) {
  HAL_Initialize(500, 0);

  AnalogInput ai{0};
  AnalogPotentiometer pot{&ai};
  AnalogInputSim sim{ai};

  RoboRioSim::ResetData();
  sim.SetVoltage(2.8);
  EXPECT_EQ(2.8 / 3.3, pot.Get());
}

TEST(AnalogPotentiometerTest, InitializeWithAnalogInputAndScale) {
  HAL_Initialize(500, 0);

  AnalogInput ai{0};
  AnalogPotentiometer pot{&ai, 270.0};
  RoboRioSim::ResetData();
  AnalogInputSim sim{ai};

  sim.SetVoltage(3.3);
  EXPECT_EQ(270.0, pot.Get());

  sim.SetVoltage(2.5);
  EXPECT_EQ((2.5 / 3.3) * 270.0, pot.Get());

  sim.SetVoltage(0.0);
  EXPECT_EQ(0.0, pot.Get());
}

TEST(AnalogPotentiometerTest, InitializeWithChannel) {
  HAL_Initialize(500, 0);

  AnalogPotentiometer pot{1};
  AnalogInputSim sim{1};

  sim.SetVoltage(3.3);
  EXPECT_EQ(1.0, pot.Get());
}

TEST(AnalogPotentiometerTest, InitializeWithChannelAndScale) {
  HAL_Initialize(500, 0);

  AnalogPotentiometer pot{1, 180.0};
  RoboRioSim::ResetData();
  AnalogInputSim sim{1};

  sim.SetVoltage(3.3);
  EXPECT_EQ(180.0, pot.Get());

  sim.SetVoltage(0.0);
  EXPECT_EQ(0.0, pot.Get());
}

TEST(AnalogPotentiometerTest, WithModifiedBatteryVoltage) {
  AnalogPotentiometer pot{1, 180.0, 90.0};
  RoboRioSim::ResetData();
  AnalogInputSim sim{1};

  // Test at 3.3v
  sim.SetVoltage(3.3);
  EXPECT_EQ(270, pot.Get());

  sim.SetVoltage(0.0);
  EXPECT_EQ(90, pot.Get());

  // Simulate a lower battery voltage
  RoboRioSim::SetUserVoltage3V3(units::volt_t{2.5});

  sim.SetVoltage(2.5);
  EXPECT_EQ(270.0, pot.Get());

  sim.SetVoltage(2.0);
  EXPECT_EQ(234.0, pot.Get());

  sim.SetVoltage(0.0);
  EXPECT_EQ(90.0, pot.Get());
}
}  // namespace frc
