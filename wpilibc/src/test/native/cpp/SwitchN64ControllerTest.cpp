// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/SwitchN64Controller.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/SwitchN64ControllerSim.hpp"

using namespace wpi;

TEST(SwitchN64ControllerTest, WrappedHID) {
  SwitchN64Controller controller{2};

  ASSERT_EQ(controller.GetPort(), 2);
  ASSERT_EQ(controller.GetHID().GetPort(), 2);
}

BUTTON_TEST(SwitchN64Controller, AButton)
BUTTON_TEST(SwitchN64Controller, BButton)
BUTTON_TEST(SwitchN64Controller, CLeftButton)
BUTTON_TEST(SwitchN64Controller, CUpButton)
BUTTON_TEST(SwitchN64Controller, CaptureButton)
BUTTON_TEST(SwitchN64Controller, HomeButton)
BUTTON_TEST(SwitchN64Controller, StartButton)
BUTTON_TEST(SwitchN64Controller, CDownButton)
BUTTON_TEST(SwitchN64Controller, LButton)
BUTTON_TEST(SwitchN64Controller, RButton)
BUTTON_TEST(SwitchN64Controller, DpadUpButton)
BUTTON_TEST(SwitchN64Controller, DpadDownButton)
BUTTON_TEST(SwitchN64Controller, DpadLeftButton)
BUTTON_TEST(SwitchN64Controller, DpadRightButton)
BUTTON_TEST(SwitchN64Controller, CRightButton)

AXIS_TEST(SwitchN64Controller, LeftX)
AXIS_TEST(SwitchN64Controller, LeftY)
AXIS_TEST(SwitchN64Controller, ZAxis)
AXIS_TEST(SwitchN64Controller, ZRAxis)
