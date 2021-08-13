// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#define AXIS_TEST(JoystickType, AxisName)                    \
  TEST(JoystickType##Tests, Set##HandType##AxisName##Axis) { \
    JoystickType joy{2};                                     \
    sim::JoystickType##Sim joysim{joy};                      \
    joysim.Set##AxisName(0.35);                              \
    joysim.NotifyNewData();                                  \
    ASSERT_NEAR(joy.Get##AxisName(), 0.35, 0.001);           \
  }

#define HANDED_AXIS_TEST_IMPL(JoystickType, HandType, AxisName)          \
  TEST(JoystickType##Tests, Set##HandType##AxisName##Axis) {             \
    JoystickType joy{2};                                                 \
    sim::JoystickType##Sim joysim{joy};                                  \
    joysim.Set##AxisName(JoystickType::HandType, 0.35);                  \
    joysim.NotifyNewData();                                              \
    ASSERT_NEAR(joy.Get##AxisName(JoystickType::HandType), 0.35, 0.001); \
  }

#define HANDED_AXIS_TEST(JoystickType, AxisName)           \
  HANDED_AXIS_TEST_IMPL(JoystickType, kLeftHand, AxisName) \
  HANDED_AXIS_TEST_IMPL(JoystickType, kRightHand, AxisName)

#define BUTTON_TEST(JoystickType, ButtonName)              \
  TEST(JoystickType##Tests, Set##ButtonName##Button) {     \
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

#define HANDED_BUTTON_TEST_IMPL(JoystickType, HandType, ButtonName)      \
  TEST(JoystickType##Tests, Get##HandType##ButtonName##Button) {         \
    JoystickType joy{1};                                                 \
    sim::JoystickType##Sim joysim{joy};                                  \
                                                                         \
    joysim.Set##ButtonName(JoystickType::HandType, false);               \
    joysim.NotifyNewData();                                              \
    ASSERT_FALSE(joy.Get##ButtonName(JoystickType::HandType));           \
    /* need to call pressed and released to clear flags */               \
    joy.Get##ButtonName##Pressed(JoystickType::HandType);                \
    joy.Get##ButtonName##Released(JoystickType::HandType);               \
                                                                         \
    joysim.Set##ButtonName(JoystickType::HandType, true);                \
    joysim.NotifyNewData();                                              \
    ASSERT_TRUE(joy.Get##ButtonName(JoystickType::HandType));            \
    ASSERT_TRUE(joy.Get##ButtonName##Pressed(JoystickType::HandType));   \
    ASSERT_FALSE(joy.Get##ButtonName##Released(JoystickType::HandType)); \
                                                                         \
    joysim.Set##ButtonName(JoystickType::HandType, false);               \
    joysim.NotifyNewData();                                              \
    ASSERT_FALSE(joy.Get##ButtonName(JoystickType::HandType));           \
    ASSERT_FALSE(joy.Get##ButtonName##Pressed(JoystickType::HandType));  \
    ASSERT_TRUE(joy.Get##ButtonName##Released(JoystickType::HandType));  \
  }

#define HANDED_BUTTON_TEST(JoystickType, ButtonName)           \
  HANDED_BUTTON_TEST_IMPL(JoystickType, kLeftHand, ButtonName) \
  HANDED_BUTTON_TEST_IMPL(JoystickType, kRightHand, ButtonName)
