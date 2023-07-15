// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/AnalogInput.h>
#include <hal/Relay.h>
#include <wpi/SmallVector.h>

#include "CrossConnects.h"
#include "LifetimeWrappers.h"
#include "gtest/gtest.h"

using namespace hlt;

class RelayAnalogTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(RelayAnalogTest, RelayAnalogCross) {
  auto param = GetParam();

  int32_t status = 0;
  RelayHandle relay{param.first, true, &status};
  ASSERT_EQ(0, status);
  AnalogInputHandle analog{param.second, &status};
  ASSERT_EQ(0, status);
  AnalogTriggerHandle trigger{analog, &status};
  ASSERT_EQ(0, status);
  HAL_SetAnalogTriggerLimitsVoltage(trigger, 1.5, 3.0, &status);
  ASSERT_EQ(0, status);

  HAL_SetRelay(relay, false, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_FALSE(HAL_GetAnalogTriggerTriggerState(trigger, &status));
  ASSERT_EQ(0, status);

  HAL_SetRelay(relay, true, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_TRUE(HAL_GetAnalogTriggerTriggerState(trigger, &status));
  ASSERT_EQ(0, status);

  HAL_SetRelay(relay, false, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_FALSE(HAL_GetAnalogTriggerTriggerState(trigger, &status));
  ASSERT_EQ(0, status);
}

INSTANTIATE_TEST_SUITE_P(RelayAnalogCrossConnectsTests, RelayAnalogTest,
                         ::testing::ValuesIn(RelayAnalogCrossConnects));
