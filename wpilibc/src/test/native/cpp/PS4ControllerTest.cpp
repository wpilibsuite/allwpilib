// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/PS4Controller.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/PS4ControllerSim.hpp"

using namespace wpi;

TEST(PS4ControllerTest, WrappedHID) {
  PS4Controller controller{2};

  ASSERT_EQ(controller.GetPort(), 2);
  ASSERT_EQ(controller.GetHID().GetPort(), 2);
}

BUTTON_TEST(PS4Controller, CrossButton)
BUTTON_TEST(PS4Controller, CircleButton)
BUTTON_TEST(PS4Controller, SquareButton)
BUTTON_TEST(PS4Controller, TriangleButton)
BUTTON_TEST(PS4Controller, ShareButton)
BUTTON_TEST(PS4Controller, PSButton)
BUTTON_TEST(PS4Controller, OptionsButton)
BUTTON_TEST(PS4Controller, L3Button)
BUTTON_TEST(PS4Controller, R3Button)
BUTTON_TEST(PS4Controller, L1Button)
BUTTON_TEST(PS4Controller, R1Button)
BUTTON_TEST(PS4Controller, DpadUpButton)
BUTTON_TEST(PS4Controller, DpadDownButton)
BUTTON_TEST(PS4Controller, DpadLeftButton)
BUTTON_TEST(PS4Controller, DpadRightButton)
BUTTON_TEST(PS4Controller, TouchpadButton)

AXIS_TEST(PS4Controller, LeftX)
AXIS_TEST(PS4Controller, LeftY)
AXIS_TEST(PS4Controller, RightX)
AXIS_TEST(PS4Controller, RightY)
AXIS_TEST(PS4Controller, L2Axis)
AXIS_TEST(PS4Controller, R2Axis)
