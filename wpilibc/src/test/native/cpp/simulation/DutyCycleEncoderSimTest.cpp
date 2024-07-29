// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DutyCycleEncoderSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DutyCycleEncoder.h"

namespace frc::sim {

TEST(DutyCycleEncoderSimTest, Set) {
  HAL_Initialize(500, 0);

  DutyCycleEncoder enc{0, 10, 0};
  DutyCycleEncoderSim sim(enc);

  constexpr double kTestValue{5.67};
  sim.Set(kTestValue);
  EXPECT_EQ(kTestValue, enc.Get());
}

TEST(DutyCycleEncoderSimTest, SetIsConnected) {
  HAL_Initialize(500, 0);

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);
  sim.SetConnected(true);
  EXPECT_TRUE(enc.IsConnected());
  sim.SetConnected(false);
  EXPECT_FALSE(enc.IsConnected());
}

}  // namespace frc::sim
