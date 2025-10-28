// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/hardware/range/SharpIR.hpp"
#include "wpi/simulation/SharpIRSim.hpp"

using namespace wpi;

TEST(SharpIRTest, SimDevices) {
  SharpIR s = SharpIR::GP2Y0A02YK0F(1);
  SharpIRSim sim(s);

  EXPECT_EQ(0.2, s.GetRange().value());

  sim.SetRange(30_cm);
  EXPECT_EQ(0.3, s.GetRange().value());

  sim.SetRange(300_cm);
  EXPECT_EQ(1.5, s.GetRange().value());
}
