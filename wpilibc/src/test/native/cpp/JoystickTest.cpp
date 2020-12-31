// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Joystick.h"  // NOLINT(build/include_order)

#include "frc/simulation/JoystickSim.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(JoystickTests, GetX) {
  Joystick joy{1};
  sim::JoystickSim joysim{joy};

  joysim.SetX(0.25);
  joysim.NotifyNewData();
  ASSERT_NEAR(joy.GetX(), 0.25, 0.001);

  joysim.SetX(0);
  joysim.NotifyNewData();
  ASSERT_NEAR(joy.GetX(), 0, 0.001);
}

TEST(JoystickTests, GetY) {
  Joystick joy{1};
  sim::JoystickSim joysim{joy};

  joysim.SetY(0.25);
  joysim.NotifyNewData();
  ASSERT_NEAR(joy.GetY(), 0.25, 0.001);

  joysim.SetY(0);
  joysim.NotifyNewData();
  ASSERT_NEAR(joy.GetY(), 0, 0.001);
}
