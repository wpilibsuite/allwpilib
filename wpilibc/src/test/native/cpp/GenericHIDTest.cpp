// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/GenericHID.hpp"

#include <gtest/gtest.h>

#include "wpi/simulation/GenericHIDSim.hpp"

using namespace wpi;
using RumbleType = GenericHID::RumbleType;
static constexpr double kEpsilon = 0.0001;
TEST(GenericHIDTest, RumbleRange) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  for (int i = 0; i <= 100; i++) {
    double rumbleValue = i / 100.0;
    hid.SetRumble(RumbleType::kLeftRumble, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);

    hid.SetRumble(RumbleType::kRightRumble, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);

    hid.SetRumble(RumbleType::kLeftTriggerRumble, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::kLeftTriggerRumble),
                kEpsilon);

    hid.SetRumble(RumbleType::kRightTriggerRumble, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::kRightTriggerRumble),
                kEpsilon);
  }
}

TEST(GenericHIDTest, RumbleTypes) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  // Make sure all are off
  hid.SetRumble(RumbleType::kLeftRumble, 0);
  hid.SetRumble(RumbleType::kLeftTriggerRumble, 0);
  hid.SetRumble(RumbleType::kRightRumble, 0);
  hid.SetRumble(RumbleType::kRightTriggerRumble, 0);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftTriggerRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightTriggerRumble), kEpsilon);

  // test left only
  hid.SetRumble(RumbleType::kLeftRumble, 1);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftTriggerRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightTriggerRumble), kEpsilon);
  hid.SetRumble(RumbleType::kLeftRumble, 0);

  // test right only
  hid.SetRumble(RumbleType::kRightRumble, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftTriggerRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightTriggerRumble), kEpsilon);
  hid.SetRumble(RumbleType::kRightRumble, 0);

  // test left trigger only
  hid.SetRumble(RumbleType::kLeftTriggerRumble, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kLeftTriggerRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightTriggerRumble), kEpsilon);
  hid.SetRumble(RumbleType::kLeftTriggerRumble, 0);

  // test right trigger only
  hid.SetRumble(RumbleType::kRightTriggerRumble, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftTriggerRumble), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kRightTriggerRumble), kEpsilon);
  hid.SetRumble(RumbleType::kRightTriggerRumble, 0);
}
