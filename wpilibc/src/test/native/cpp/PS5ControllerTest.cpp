// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/nids/PS5Controller.hpp"

#include <gtest/gtest.h>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/nids/PS5ControllerSim.hpp"

using namespace wpi;
using namespace wpi::nids;

BUTTON_TEST_NIDS(PS5Controller, SquareButton)
BUTTON_TEST_NIDS(PS5Controller, CrossButton)
BUTTON_TEST_NIDS(PS5Controller, CircleButton)
BUTTON_TEST_NIDS(PS5Controller, TriangleButton)

BUTTON_TEST_NIDS(PS5Controller, L1Button)
BUTTON_TEST_NIDS(PS5Controller, R1Button)
BUTTON_TEST_NIDS(PS5Controller, L2Button)
BUTTON_TEST_NIDS(PS5Controller, R2Button)

BUTTON_TEST_NIDS(PS5Controller, CreateButton)
BUTTON_TEST_NIDS(PS5Controller, OptionsButton)

BUTTON_TEST_NIDS(PS5Controller, L3Button)
BUTTON_TEST_NIDS(PS5Controller, R3Button)

BUTTON_TEST_NIDS(PS5Controller, PSButton)
BUTTON_TEST_NIDS(PS5Controller, TouchpadButton)

AXIS_TEST_NIDS(PS5Controller, LeftX)
AXIS_TEST_NIDS(PS5Controller, RightX)
AXIS_TEST_NIDS(PS5Controller, LeftY)
AXIS_TEST_NIDS(PS5Controller, RightY)
AXIS_TEST_NIDS(PS5Controller, L2Axis)
AXIS_TEST_NIDS(PS5Controller, R2Axis)
