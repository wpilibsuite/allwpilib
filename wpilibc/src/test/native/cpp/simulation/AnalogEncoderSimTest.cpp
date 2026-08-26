// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AnalogEncoderSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hardware/discrete/AnalogInput.hpp"
#include "wpi/hardware/rotation/AnalogEncoder.hpp"

TEST_CASE("AnalogEncoderSimTest Basic", "[wpilibc][simulation]") {
  wpi::AnalogInput ai(0);
  wpi::AnalogEncoder encoder{ai, 360, 0};
  wpi::sim::AnalogEncoderSim encoderSim{encoder};

  encoderSim.Set(180);
  CHECK_THAT(encoder.Get(), Catch::Matchers::WithinAbs(180, 1E-8));
  CHECK_THAT(encoderSim.Get(), Catch::Matchers::WithinAbs(180, 1E-8));
}
