// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/SharpIR.h"
#include "frc/simulation/SharpIRSim.h"

using namespace frc;

TEST(SharpIRTest, SimDevices) {
  SharpIR s = SharpIR::GP2Y0A02YK0F(1);
  SharpIRSim sim(s);

  EXPECT_EQ(20, s.GetRange().value());

  sim.SetRange(units::centimeter_t{30});
  EXPECT_EQ(30, s.GetRange().value());

  sim.SetRange(units::centimeter_t{300});
  EXPECT_EQ(150, s.GetRange().value());
}
