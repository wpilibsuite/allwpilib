// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/nids/XboxController.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/nids/XboxControllerSim.hpp"

using namespace wpi;
using namespace wpi::nids;

BUTTON_TEST_NIDS(XboxController, LeftBumperButton)
BUTTON_TEST_NIDS(XboxController, RightBumperButton)

BUTTON_TEST_NIDS(XboxController, LeftStickButton)
BUTTON_TEST_NIDS(XboxController, RightStickButton)

BUTTON_TEST_NIDS(XboxController, AButton)
BUTTON_TEST_NIDS(XboxController, BButton)
BUTTON_TEST_NIDS(XboxController, XButton)
BUTTON_TEST_NIDS(XboxController, YButton)
BUTTON_TEST_NIDS(XboxController, BackButton)
BUTTON_TEST_NIDS(XboxController, StartButton)

AXIS_TEST_NIDS(XboxController, LeftX)
AXIS_TEST_NIDS(XboxController, RightX)
AXIS_TEST_NIDS(XboxController, LeftY)
AXIS_TEST_NIDS(XboxController, RightY)

AXIS_TEST_NIDS(XboxController, LeftTriggerAxis)
AXIS_TEST_NIDS(XboxController, RightTriggerAxis)
