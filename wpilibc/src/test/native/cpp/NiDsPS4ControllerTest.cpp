// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/NiDsPS4Controller.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/NiDsPS4ControllerSim.hpp"

using namespace wpi;

BUTTON_TEST(NiDsPS4Controller, SquareButton)
BUTTON_TEST(NiDsPS4Controller, CrossButton)
BUTTON_TEST(NiDsPS4Controller, CircleButton)
BUTTON_TEST(NiDsPS4Controller, TriangleButton)

BUTTON_TEST(NiDsPS4Controller, L1Button)
BUTTON_TEST(NiDsPS4Controller, R1Button)
BUTTON_TEST(NiDsPS4Controller, L2Button)
BUTTON_TEST(NiDsPS4Controller, R2Button)

BUTTON_TEST(NiDsPS4Controller, ShareButton)
BUTTON_TEST(NiDsPS4Controller, OptionsButton)

BUTTON_TEST(NiDsPS4Controller, L3Button)
BUTTON_TEST(NiDsPS4Controller, R3Button)

BUTTON_TEST(NiDsPS4Controller, PSButton)
BUTTON_TEST(NiDsPS4Controller, TouchpadButton)

AXIS_TEST(NiDsPS4Controller, LeftX)
AXIS_TEST(NiDsPS4Controller, RightX)
AXIS_TEST(NiDsPS4Controller, LeftY)
AXIS_TEST(NiDsPS4Controller, RightY)
AXIS_TEST(NiDsPS4Controller, L2Axis)
AXIS_TEST(NiDsPS4Controller, R2Axis)
