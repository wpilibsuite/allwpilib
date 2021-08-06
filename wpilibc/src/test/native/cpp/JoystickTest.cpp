// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Joystick.h"  // NOLINT(build/include_order)

#include "JoystickTestMacros.h"
#include "frc/simulation/JoystickSim.h"
#include "gtest/gtest.h"

using namespace frc;

AXIS_TEST(Joystick, X)
AXIS_TEST(Joystick, Y)
AXIS_TEST(Joystick, Z)
AXIS_TEST(Joystick, Throttle)
AXIS_TEST(Joystick, Twist)

BUTTON_TEST(Joystick, Trigger)
BUTTON_TEST(Joystick, Top)
