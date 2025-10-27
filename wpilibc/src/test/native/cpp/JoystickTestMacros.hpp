// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#define AXIS_TEST(JoystickType, AxisName)          \
  TEST(JoystickType##Test, Get##AxisName) {        \
    JoystickType joy{2};                           \
    sim::JoystickType##Sim joysim{joy};            \
    joysim.Set##AxisName(0.35);                    \
    joysim.NotifyNewData();                        \
    ASSERT_NEAR(joy.Get##AxisName(), 0.35, 0.001); \
  }

#define BUTTON_TEST(JoystickType, ButtonName)              \
  TEST(JoystickType##Test, Get##ButtonName) {              \
    JoystickType joy{1};                                   \
    sim::JoystickType##Sim joysim{joy};                    \
                                                           \
    joysim.Set##ButtonName(false);                         \
    joysim.NotifyNewData();                                \
    ASSERT_FALSE(joy.Get##ButtonName());                   \
    /* need to call pressed and released to clear flags */ \
    joy.Get##ButtonName##Pressed();                        \
    joy.Get##ButtonName##Released();                       \
                                                           \
    joysim.Set##ButtonName(true);                          \
    joysim.NotifyNewData();                                \
    ASSERT_TRUE(joy.Get##ButtonName());                    \
    ASSERT_TRUE(joy.Get##ButtonName##Pressed());           \
    ASSERT_FALSE(joy.Get##ButtonName##Released());         \
                                                           \
    joysim.Set##ButtonName(false);                         \
    joysim.NotifyNewData();                                \
    ASSERT_FALSE(joy.Get##ButtonName());                   \
    ASSERT_FALSE(joy.Get##ButtonName##Pressed());          \
    ASSERT_TRUE(joy.Get##ButtonName##Released());          \
  }
