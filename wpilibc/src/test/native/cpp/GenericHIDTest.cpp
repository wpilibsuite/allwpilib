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
    hid.SetRumble(RumbleType::LEFT_RUMBLE, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::LEFT_RUMBLE), kEpsilon);

    hid.SetRumble(RumbleType::RIGHT_RUMBLE, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::RIGHT_RUMBLE), kEpsilon);

    hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE),
                kEpsilon);

    hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, rumbleValue);
    EXPECT_NEAR(rumbleValue, sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE),
                kEpsilon);
  }
}

TEST(GenericHIDTest, RumbleTypes) {
  GenericHID hid{0};
  sim::GenericHIDSim sim{0};

  // Make sure all are off
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 0);
  hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 0);
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 0);
  hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 0);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), kEpsilon);

  // test left only
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 1);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::LEFT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), kEpsilon);
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 0);

  // test right only
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_RUMBLE), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::RIGHT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), kEpsilon);
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 0);

  // test left trigger only
  hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_RUMBLE), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), kEpsilon);
  hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 0);

  // test right trigger only
  hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 1);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::RIGHT_RUMBLE), kEpsilon);
  EXPECT_NEAR(0, sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), kEpsilon);
  EXPECT_NEAR(1, sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), kEpsilon);
  hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 0);
}
