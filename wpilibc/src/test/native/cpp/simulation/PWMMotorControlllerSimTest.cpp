// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/simulation/PWMMotorControllerSim.hpp"
// clang-format on

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hardware/motor/Spark.hpp"

namespace wpi::sim {
TEST_CASE("PWMMotorControllerSimTest TestMotor", "[wpilibc][simulation]") {
  wpi::Spark spark{0};
  wpi::sim::PWMMotorControllerSim sim{spark};

  spark.SetThrottle(0);
  CHECK((0) == (sim.GetThrottle()));

  spark.SetThrottle(0.354);
  CHECK((0.354) == (sim.GetThrottle()));

  spark.SetThrottle(-0.785);
  CHECK((-0.785) == (sim.GetThrottle()));
}
}  // namespace wpi::sim
