// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DualSenseEdgeController.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/DualSenseEdgeControllerSim.hpp"

using namespace wpi;

TEST(DualSenseEdgeControllerTest, WrappedHID) {
  DualSenseEdgeController controller{2};

  ASSERT_EQ(controller.GetPort(), 2);
  ASSERT_EQ(controller.GetHID().GetPort(), 2);
}

BUTTON_TEST(DualSenseEdgeController, CrossButton)
BUTTON_TEST(DualSenseEdgeController, CircleButton)
BUTTON_TEST(DualSenseEdgeController, SquareButton)
BUTTON_TEST(DualSenseEdgeController, TriangleButton)
BUTTON_TEST(DualSenseEdgeController, CreateButton)
BUTTON_TEST(DualSenseEdgeController, PSButton)
BUTTON_TEST(DualSenseEdgeController, OptionsButton)
BUTTON_TEST(DualSenseEdgeController, L3Button)
BUTTON_TEST(DualSenseEdgeController, R3Button)
BUTTON_TEST(DualSenseEdgeController, L1Button)
BUTTON_TEST(DualSenseEdgeController, R1Button)
BUTTON_TEST(DualSenseEdgeController, DpadUpButton)
BUTTON_TEST(DualSenseEdgeController, DpadDownButton)
BUTTON_TEST(DualSenseEdgeController, DpadLeftButton)
BUTTON_TEST(DualSenseEdgeController, DpadRightButton)
BUTTON_TEST(DualSenseEdgeController, MicrophoneButton)
BUTTON_TEST(DualSenseEdgeController, RightPaddle1Button)
BUTTON_TEST(DualSenseEdgeController, LeftPaddle1Button)
BUTTON_TEST(DualSenseEdgeController, TouchpadButton)
BUTTON_TEST(DualSenseEdgeController, LeftFunctionButton)
BUTTON_TEST(DualSenseEdgeController, RightFunctionButton)

AXIS_TEST(DualSenseEdgeController, LeftX)
AXIS_TEST(DualSenseEdgeController, LeftY)
AXIS_TEST(DualSenseEdgeController, RightX)
AXIS_TEST(DualSenseEdgeController, RightY)
AXIS_TEST(DualSenseEdgeController, L2Axis)
AXIS_TEST(DualSenseEdgeController, R2Axis)
