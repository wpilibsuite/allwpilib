// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PS5Controller.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

#include "JoystickTestMacros.h"
#include "frc/simulation/PS5ControllerSim.h"

using namespace frc;

BUTTON_TEST(PS5Controller, SquareButton)
BUTTON_TEST(PS5Controller, CrossButton)
BUTTON_TEST(PS5Controller, CircleButton)
BUTTON_TEST(PS5Controller, TriangleButton)

BUTTON_TEST(PS5Controller, L1Button)
BUTTON_TEST(PS5Controller, R1Button)
BUTTON_TEST(PS5Controller, L2Button)
BUTTON_TEST(PS5Controller, R2Button)

BUTTON_TEST(PS5Controller, CreateButton)
BUTTON_TEST(PS5Controller, OptionsButton)

BUTTON_TEST(PS5Controller, L3Button)
BUTTON_TEST(PS5Controller, R3Button)

BUTTON_TEST(PS5Controller, PSButton)
BUTTON_TEST(PS5Controller, TouchpadButton)

AXIS_TEST(PS5Controller, LeftX)
AXIS_TEST(PS5Controller, RightX)
AXIS_TEST(PS5Controller, LeftY)
AXIS_TEST(PS5Controller, RightY)
AXIS_TEST(PS5Controller, L2Axis)
AXIS_TEST(PS5Controller, R2Axis)
