// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/NiDsXboxController.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/NiDsXboxControllerSim.hpp"

using namespace wpi;

BUTTON_TEST(NiDsXboxController, LeftBumperButton)
BUTTON_TEST(NiDsXboxController, RightBumperButton)

BUTTON_TEST(NiDsXboxController, LeftStickButton)
BUTTON_TEST(NiDsXboxController, RightStickButton)
BUTTON_TEST(NiDsXboxController, AButton)
BUTTON_TEST(NiDsXboxController, BButton)
BUTTON_TEST(NiDsXboxController, XButton)
BUTTON_TEST(NiDsXboxController, YButton)
BUTTON_TEST(NiDsXboxController, BackButton)
BUTTON_TEST(NiDsXboxController, StartButton)

AXIS_TEST(NiDsXboxController, LeftX)
AXIS_TEST(NiDsXboxController, RightX)
AXIS_TEST(NiDsXboxController, LeftY)
AXIS_TEST(NiDsXboxController, RightY)

AXIS_TEST(NiDsXboxController, LeftTriggerAxis)
AXIS_TEST(NiDsXboxController, RightTriggerAxis)
