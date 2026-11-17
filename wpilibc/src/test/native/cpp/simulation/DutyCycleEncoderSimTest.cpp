// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DutyCycleEncoderSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/hardware/rotation/DutyCycleEncoder.hpp"

namespace wpi::sim {

TEST_CASE("DutyCycleEncoderSimTest Set", "[wpilibc][simulation]") {
  HAL_Initialize();

  DutyCycleEncoder enc{0, 10, 0};
  DutyCycleEncoderSim sim(enc);

  constexpr double TEST_VALUE{5.67};
  sim.Set(TEST_VALUE);
  CHECK(TEST_VALUE == enc.Get());
}

TEST_CASE("DutyCycleEncoderSimTest SetIsConnected", "[wpilibc][simulation]") {
  HAL_Initialize();

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);
  sim.SetConnected(true);
  CHECK(enc.IsConnected());
  sim.SetConnected(false);
  CHECK_FALSE(enc.IsConnected());
}

}  // namespace wpi::sim
