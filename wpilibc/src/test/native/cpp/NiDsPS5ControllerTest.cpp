// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/NiDsPS5Controller.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/NiDsPS5ControllerSim.hpp"

using namespace wpi;

BUTTON_TEST(NiDsPS5Controller, SquareButton)
BUTTON_TEST(NiDsPS5Controller, CrossButton)
BUTTON_TEST(NiDsPS5Controller, CircleButton)
BUTTON_TEST(NiDsPS5Controller, TriangleButton)

BUTTON_TEST(NiDsPS5Controller, L1Button)
BUTTON_TEST(NiDsPS5Controller, R1Button)
BUTTON_TEST(NiDsPS5Controller, L2Button)
BUTTON_TEST(NiDsPS5Controller, R2Button)

BUTTON_TEST(NiDsPS5Controller, CreateButton)
BUTTON_TEST(NiDsPS5Controller, OptionsButton)

BUTTON_TEST(NiDsPS5Controller, L3Button)
BUTTON_TEST(NiDsPS5Controller, R3Button)

BUTTON_TEST(NiDsPS5Controller, PSButton)
BUTTON_TEST(NiDsPS5Controller, TouchpadButton)

AXIS_TEST(NiDsPS5Controller, LeftX)
AXIS_TEST(NiDsPS5Controller, RightX)
AXIS_TEST(NiDsPS5Controller, LeftY)
AXIS_TEST(NiDsPS5Controller, RightY)
AXIS_TEST(NiDsPS5Controller, L2Axis)
AXIS_TEST(NiDsPS5Controller, R2Axis)
