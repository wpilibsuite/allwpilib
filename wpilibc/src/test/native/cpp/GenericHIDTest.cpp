// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/GenericHID.h>
#include <frc/simulation/GenericHIDSim.h>

#include <gtest/gtest.h>

using namespace frc;
using RumbleType = GenericHID::RumbleType;
static constexpr double kEpsilon = 0.0001;
TEST(GenericHIDTest, RumbleRange) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  for (int i = 0; i <= 100; i++) {
    double rumbleValue = i / 100.0;
    hid.SetRumble(RumbleType::kBothRumble, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  }
}

TEST(GenericHIDTest, RumbleTypes) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  // Make sure both are off
  hid.SetRumble(RumbleType::kBothRumble, 0);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kBothRumble), kEpsilon);

  // test both
  hid.SetRumble(RumbleType::kBothRumble, 1);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  hid.SetRumble(RumbleType::kBothRumble, 0);

  // test left only
  hid.SetRumble(RumbleType::kLeftRumble, 1);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  hid.SetRumble(RumbleType::kLeftRumble, 0);

  // test right only
  hid.SetRumble(RumbleType::kRightRumble, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::kLeftRumble), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::kRightRumble), kEpsilon);
  hid.SetRumble(RumbleType::kRightRumble, 0);
}
