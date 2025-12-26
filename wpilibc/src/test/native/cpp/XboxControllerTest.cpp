// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/nids/XboxController.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/nids/XboxControllerSim.hpp"

using namespace wpi;
using namespace wpi::nids;

BUTTON_TEST(XboxController, LeftBumperButton)
BUTTON_TEST(XboxController, RightBumperButton)

BUTTON_TEST(XboxController, LeftStickButton)
BUTTON_TEST(XboxController, RightStickButton)

BUTTON_TEST(XboxController, AButton)
BUTTON_TEST(XboxController, BButton)
BUTTON_TEST(XboxController, XButton)
BUTTON_TEST(XboxController, YButton)
BUTTON_TEST(XboxController, BackButton)
BUTTON_TEST(XboxController, StartButton)

AXIS_TEST(XboxController, LeftX)
AXIS_TEST(XboxController, RightX)
AXIS_TEST(XboxController, LeftY)
AXIS_TEST(XboxController, RightY)

AXIS_TEST(XboxController, LeftTriggerAxis)
AXIS_TEST(XboxController, RightTriggerAxis)
