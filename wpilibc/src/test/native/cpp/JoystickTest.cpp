// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/JoystickSim.hpp"

using namespace wpi;

// https://github.com/wpilibsuite/allwpilib/issues/1550
TEST(JoystickTest, FastDeconstruction) {
  Joystick joystick{0};
}

AXIS_TEST(Joystick, X)
AXIS_TEST(Joystick, Y)
AXIS_TEST(Joystick, Z)
AXIS_TEST(Joystick, Throttle)
AXIS_TEST(Joystick, Twist)

BUTTON_TEST(Joystick, Trigger)
BUTTON_TEST(Joystick, Top)

TEST(JoystickTest, GetMagnitude) {
  Joystick joy{1};
  sim::JoystickSim joysim{1};

  joysim.SetX(0.5);
  joysim.SetY(0);
  joysim.NotifyNewData();
  ASSERT_NEAR(0.5, joy.GetMagnitude(), 0.001);

  joysim.SetX(0);
  joysim.SetY(-.5);
  joysim.NotifyNewData();
  ASSERT_NEAR(0.5, joy.GetMagnitude(), 0.001);

  joysim.SetX(0.5);
  joysim.SetY(-0.5);
  joysim.NotifyNewData();
  ASSERT_NEAR(0.70710678118, joy.GetMagnitude(), 0.001);
}

TEST(JoystickTest, GetDirection) {
  Joystick joy{1};
  sim::JoystickSim joysim{1};

  joysim.SetX(0.5);
  joysim.SetY(0);
  joysim.NotifyNewData();
  ASSERT_NEAR(wpi::units::radian_t{90_deg}.value(), joy.GetDirection().value(),
              0.001);

  joysim.SetX(0);
  joysim.SetY(-.5);
  joysim.NotifyNewData();
  ASSERT_NEAR(wpi::units::radian_t{0_deg}.value(), joy.GetDirection().value(),
              0.001);

  joysim.SetX(0.5);
  joysim.SetY(-0.5);
  joysim.NotifyNewData();
  ASSERT_NEAR(wpi::units::radian_t{45_deg}.value(), joy.GetDirection().value(),
              0.001);
}
