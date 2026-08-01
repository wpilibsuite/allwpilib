// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define AXIS_TEST(JoystickType, AxisName)                  \
  TEST_CASE(#JoystickType "Test Get" #AxisName,            \
            "[wpilibc][hid][controller]") {                \
    JoystickType joy{2};                                   \
    sim::JoystickType##Sim joysim{joy};                    \
    joysim.Set##AxisName(0.35);                            \
    joysim.NotifyNewData();                                \
    REQUIRE_THAT(joy.Get##AxisName(),                      \
                 Catch::Matchers::WithinAbs(0.35, 0.001)); \
  }

#define BUTTON_TEST(JoystickType, ButtonName)              \
  TEST_CASE(#JoystickType "Test Get" #ButtonName,          \
            "[wpilibc][hid][controller]") {                \
    JoystickType joy{1};                                   \
    sim::JoystickType##Sim joysim{joy};                    \
                                                           \
    joysim.Set##ButtonName(false);                         \
    joysim.NotifyNewData();                                \
    REQUIRE_FALSE(joy.Get##ButtonName());                  \
    /* need to call pressed and released to clear flags */ \
    joy.Get##ButtonName##Pressed();                        \
    joy.Get##ButtonName##Released();                       \
                                                           \
    joysim.Set##ButtonName(true);                          \
    joysim.NotifyNewData();                                \
    REQUIRE(joy.Get##ButtonName());                        \
    REQUIRE(joy.Get##ButtonName##Pressed());               \
    REQUIRE_FALSE(joy.Get##ButtonName##Released());        \
                                                           \
    joysim.Set##ButtonName(false);                         \
    joysim.NotifyNewData();                                \
    REQUIRE_FALSE(joy.Get##ButtonName());                  \
    REQUIRE_FALSE(joy.Get##ButtonName##Pressed());         \
    REQUIRE(joy.Get##ButtonName##Released());              \
  }
