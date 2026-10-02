// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/drivers/motor/Spark.hpp"
#include "wpi/simulation/PWMMotorControllerSim.hpp"

TEST_CASE("PWMMotorControllerTest ReportsThrottleToSimulation",
          "[drivers][pwm-motor-controller]") {
  wpi::Spark motor{0};
  wpi::sim::PWMMotorControllerSim sim{motor};

  motor.SetThrottle(0.354);

  CHECK(0.354 == sim.GetThrottle());
}
