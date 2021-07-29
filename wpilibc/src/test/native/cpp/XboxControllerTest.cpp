// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/XboxController.h"  // NOLINT(build/include_order)

#include "JoystickTestMacros.h"
#include "frc/simulation/XboxControllerSim.h"
#include "gtest/gtest.h"

using namespace frc;

HANDED_BUTTON_TEST(XboxController, Bumper)
HANDED_BUTTON_TEST(XboxController, StickButton)

BUTTON_TEST(XboxController, AButton)
BUTTON_TEST(XboxController, BButton)
BUTTON_TEST(XboxController, XButton)
BUTTON_TEST(XboxController, YButton)
BUTTON_TEST(XboxController, BackButton)
BUTTON_TEST(XboxController, StartButton)

HANDED_AXIS_TEST(XboxController, X)
HANDED_AXIS_TEST(XboxController, Y)
HANDED_AXIS_TEST(XboxController, TriggerAxis)
