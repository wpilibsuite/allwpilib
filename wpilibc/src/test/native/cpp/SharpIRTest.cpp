// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/range/SharpIR.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/simulation/SharpIRSim.hpp"

using namespace wpi;

TEST_CASE("SharpIRTest SimDevices", "[wpilibc]") {
  SharpIR s = SharpIR::GP2Y0A02YK0F(1);
  SharpIRSim sim(s);

  CHECK(0.2 == s.GetRange().value());

  sim.SetRange(30_cm);
  CHECK(0.3 == s.GetRange().value());

  sim.SetRange(300_cm);
  CHECK(1.5 == s.GetRange().value());
}
