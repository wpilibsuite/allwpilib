// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/GenericHID.h>
#include <frc/simulation/GenericHIDSim.h>

#include <gtest/gtest.h>

using namespace frc;
using RumbleType = GenericHID::RumbleType;
static constexpr double EPSILON = 0.0001;
TEST(GenericHIDTest, RumbleRange) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  for (int i = 0; i <= 100; i++) {
    double rumbleValue = i / 100.0;
    hid.SetRumble(RumbleType::BOTH_RUMBLE, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON);
  }
}

TEST(GenericHIDTest, RumbleTypes) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  // Make sure both are off
  hid.SetRumble(RumbleType::BOTH_RUMBLE, 0);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::BOTH_RUMBLE), EPSILON);

  // test both
  hid.SetRumble(RumbleType::BOTH_RUMBLE, 1);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON);
  hid.SetRumble(RumbleType::BOTH_RUMBLE, 0);

  // test left only
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 1);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON);
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 0);

  // test right only
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON);
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 0);
}
