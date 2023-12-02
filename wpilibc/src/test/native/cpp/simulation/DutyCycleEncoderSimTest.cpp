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

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);

  constexpr units::turn_t kTestValue{5.67};
  sim.Set(kTestValue);
  EXPECT_EQ(kTestValue, enc.Get());
}

TEST(DutyCycleEncoderSimTest, SetDistance) {
  HAL_Initialize(500, 0);

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);
  sim.SetDistance(19.1);
  EXPECT_EQ(19.1, enc.GetDistance());
}

TEST(DutyCycleEncoderSimTest, SetDistancePerRotation) {
  HAL_Initialize(500, 0);

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);
  sim.Set(units::turn_t{1.5});
  enc.SetDistancePerRotation(42);
  EXPECT_EQ(63, enc.GetDistance());
}

TEST(DutyCycleEncoderSimTest, SetAbsolutePosition) {
  HAL_Initialize(500, 0);

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);
  sim.SetAbsolutePosition(0.75);
  EXPECT_EQ(0.75, enc.GetAbsolutePosition());
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

TEST(DutyCycleEncoderSimTest, Reset) {
  HAL_Initialize(500, 0);

  DutyCycleEncoder enc{0};
  DutyCycleEncoderSim sim(enc);
  sim.SetDistance(2.5);
  EXPECT_EQ(2.5, enc.GetDistance());
  enc.Reset();
  EXPECT_EQ(0, enc.GetDistance());
}

}  // namespace frc::sim
