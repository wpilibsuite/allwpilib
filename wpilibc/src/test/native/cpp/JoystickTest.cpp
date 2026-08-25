// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Joystick.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "JoystickTestMacros.hpp"
#include "wpi/simulation/JoystickSim.hpp"

using namespace wpi;

// https://github.com/wpilibsuite/allwpilib/issues/1550
TEST_CASE("JoystickTest FastDeconstruction", "[wpilibc]") {
  Joystick joystick{0};
}

AXIS_TEST(Joystick, X)
AXIS_TEST(Joystick, Y)
AXIS_TEST(Joystick, Z)
AXIS_TEST(Joystick, Throttle)
AXIS_TEST(Joystick, Twist)

BUTTON_TEST(Joystick, Trigger)
BUTTON_TEST(Joystick, Top)

TEST_CASE("JoystickTest GetMagnitude", "[wpilibc]") {
  Joystick joy{1};
  sim::JoystickSim joysim{1};

  joysim.SetX(0.5);
  joysim.SetY(0);
  joysim.NotifyNewData();
  REQUIRE_THAT(0.5, Catch::Matchers::WithinAbs(joy.GetMagnitude(), 0.001));

  joysim.SetX(0);
  joysim.SetY(-.5);
  joysim.NotifyNewData();
  REQUIRE_THAT(0.5, Catch::Matchers::WithinAbs(joy.GetMagnitude(), 0.001));

  joysim.SetX(0.5);
  joysim.SetY(-0.5);
  joysim.NotifyNewData();
  REQUIRE_THAT(0.70710678118,
               Catch::Matchers::WithinAbs(joy.GetMagnitude(), 0.001));
}

TEST_CASE("JoystickTest GetDirection", "[wpilibc]") {
  Joystick joy{1};
  sim::JoystickSim joysim{1};

  joysim.SetX(0.5);
  joysim.SetY(0);
  joysim.NotifyNewData();
  REQUIRE_THAT(wpi::units::radians<>{90_deg}.value(),
               Catch::Matchers::WithinAbs(joy.GetDirection().value(), 0.001));

  joysim.SetX(0);
  joysim.SetY(-.5);
  joysim.NotifyNewData();
  REQUIRE_THAT(wpi::units::radians<>{0_deg}.value(),
               Catch::Matchers::WithinAbs(joy.GetDirection().value(), 0.001));

  joysim.SetX(0.5);
  joysim.SetY(-0.5);
  joysim.NotifyNewData();
  REQUIRE_THAT(wpi::units::radians<>{45_deg}.value(),
               Catch::Matchers::WithinAbs(joy.GetDirection().value(), 0.001));
}
