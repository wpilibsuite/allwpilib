// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "CrossConnects.h"
#include "LifetimeWrappers.h"

using namespace hlt;

class DutyCycleTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(DutyCycleTest, DutyCycle) {
  auto param = GetParam();

  int32_t status = 0;
  PWMHandle pwmHandle(param.first, &status);
  ASSERT_NE(pwmHandle, HAL_kInvalidHandle);
  ASSERT_EQ(0, status);

  // Ensure our PWM is disabled, and set up properly
  HAL_SetPWMPulseTimeMicroseconds(pwmHandle, 0, &status);
  ASSERT_EQ(0, status);
  HAL_SetPWMConfigMicroseconds(pwmHandle, 2000, 1000, 1000, 0, 0, &status);
  HAL_SetPWMConfigMicroseconds(pwmHandle, 5050, 2525, 2525, 2525, 0, &status);
  ASSERT_EQ(0, status);
  HAL_SetPWMPeriodScale(pwmHandle, 0, &status);
  ASSERT_EQ(0, status);

  DIOHandle dioHandle{param.second, true, &status};
  ASSERT_EQ(0, status);

  DutyCycleHandle dutyCycle{dioHandle, &status};
  ASSERT_EQ(0, status);

  HAL_SetPWMSpeed(pwmHandle, 0.5, &status);
  ASSERT_EQ(0, status);

  // Sleep enough time for the frequency to converge
  usleep(3500000);

  ASSERT_NEAR(
      1000 / 5.05,
      static_cast<double>(HAL_GetDutyCycleFrequency(dutyCycle, &status)), 1);

  // TODO measure output
}

INSTANTIATE_TEST_SUITE_P(DutyCycleCrossConnTests, DutyCycleTest,
                         ::testing::ValuesIn(PWMCrossConnects));
