// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PWMSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/PWM.h"

namespace frc::sim {

constexpr double kPWMStepSize = 1.0 / 2000.0;

TEST(PWMSimTest, Initialize) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PWM pwm{0};
  EXPECT_TRUE(sim.GetInitialized());
}

TEST(PWMSimTest, SetPulseTime) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterPulseMicrosecondCallback(callback.GetCallback(), false);
  PWM pwm{0};
  sim.SetPulseMicrosecond(2290);
  EXPECT_EQ(2290, sim.GetPulseMicrosecond());
  EXPECT_EQ(2290, std::lround(pwm.GetPulseTime().value()));
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(2290, callback.GetLastValue());
}

TEST(PWMSimTest, SetSpeed) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  DoubleCallback callback;

  auto cb = sim.RegisterSpeedCallback(callback.GetCallback(), false);
  PWM pwm{0};
  double kTestValue = 0.3504;
  pwm.SetSpeed(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetSpeed(), kPWMStepSize);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_NEAR(kTestValue, callback.GetLastValue(), kPWMStepSize);
  EXPECT_NEAR(kTestValue / 2 + 0.5, sim.GetPosition(), kPWMStepSize * 2);
  EXPECT_NEAR(kTestValue / 2 + 0.5, pwm.GetPosition(), kPWMStepSize * 2);

  kTestValue = -1.0;
  pwm.SetSpeed(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(0.0, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(0.0, pwm.GetPosition(), kPWMStepSize);

  kTestValue = 0.0;
  pwm.SetSpeed(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(0.5, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(0.5, pwm.GetPosition(), kPWMStepSize);

  kTestValue = 1.0;
  pwm.SetSpeed(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetPosition(), kPWMStepSize);

  kTestValue = 1.1;
  pwm.SetSpeed(kTestValue);

  EXPECT_NEAR(1.0, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(1.0, pwm.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(1.0, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(1.0, pwm.GetPosition(), kPWMStepSize);
}

TEST(PWMSimTest, SetPosition) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  DoubleCallback callback;

  auto cb = sim.RegisterPositionCallback(callback.GetCallback(), false);
  PWM pwm{0};
  double kTestValue = 0.3504;
  pwm.SetPosition(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetPosition(), kPWMStepSize);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_NEAR(kTestValue, callback.GetLastValue(), kPWMStepSize);
  EXPECT_NEAR(kTestValue * 2 - 1.0, sim.GetSpeed(), kPWMStepSize * 2);
  EXPECT_NEAR(kTestValue * 2 - 1.0, pwm.GetSpeed(), kPWMStepSize * 2);

  kTestValue = -1.0;
  pwm.SetPosition(kTestValue);

  EXPECT_NEAR(0.0, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(0.0, pwm.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetSpeed(), kPWMStepSize);

  kTestValue = 0.0;
  pwm.SetPosition(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(-1.0, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(-1.0, pwm.GetSpeed(), kPWMStepSize);

  kTestValue = 1.0;
  pwm.SetPosition(kTestValue);

  EXPECT_NEAR(kTestValue, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(kTestValue, pwm.GetSpeed(), kPWMStepSize);

  kTestValue = 1.1;
  pwm.SetPosition(kTestValue);

  EXPECT_NEAR(1.0, sim.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(1.0, pwm.GetPosition(), kPWMStepSize);
  EXPECT_NEAR(1.0, sim.GetSpeed(), kPWMStepSize);
  EXPECT_NEAR(1.0, pwm.GetSpeed(), kPWMStepSize);
}

TEST(PWMSimTest, SetPeriodScale) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterPeriodScaleCallback(callback.GetCallback(), false);
  PWM pwm{0};
  sim.SetPeriodScale(3504);
  EXPECT_EQ(3504, sim.GetPeriodScale());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(PWMSimTest, SetZeroLatch) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterZeroLatchCallback(callback.GetCallback(), false);
  PWM pwm{0};
  pwm.SetZeroLatch();

  EXPECT_TRUE(callback.WasTriggered());
}

}  // namespace frc::sim
