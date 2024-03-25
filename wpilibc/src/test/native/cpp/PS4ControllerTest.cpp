// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PS4Controller.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

#include "JoystickTestMacros.h"
#include "frc/simulation/PS4ControllerSim.h"

using namespace frc;

BUTTON_TEST(PS4Controller, SquareButton)
BUTTON_TEST(PS4Controller, CrossButton)
BUTTON_TEST(PS4Controller, CircleButton)
BUTTON_TEST(PS4Controller, TriangleButton)

BUTTON_TEST(PS4Controller, L1Button)
BUTTON_TEST(PS4Controller, R1Button)
BUTTON_TEST(PS4Controller, L2Button)
BUTTON_TEST(PS4Controller, R2Button)

BUTTON_TEST(PS4Controller, ShareButton)
BUTTON_TEST(PS4Controller, OptionsButton)

BUTTON_TEST(PS4Controller, L3Button)
BUTTON_TEST(PS4Controller, R3Button)

BUTTON_TEST(PS4Controller, PSButton)
BUTTON_TEST(PS4Controller, TouchpadButton)

AXIS_TEST(PS4Controller, LeftX)
AXIS_TEST(PS4Controller, RightX)
AXIS_TEST(PS4Controller, LeftY)
AXIS_TEST(PS4Controller, RightY)
AXIS_TEST(PS4Controller, L2Axis)
AXIS_TEST(PS4Controller, R2Axis)
