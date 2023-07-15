// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/AnalogInput.h>
#include <hal/AnalogOutput.h>
#include <wpi/SmallVector.h>

#include "CrossConnects.h"
#include "LifetimeWrappers.h"
#include "gtest/gtest.h"

using namespace hlt;

class AnalogCrossTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(AnalogCrossTest, AnalogCross) {
  auto param = GetParam();

  int32_t status = 0;
  AnalogInputHandle input{param.first, &status};
  ASSERT_EQ(0, status);
  AnalogOutputHandle output{param.second, &status};
  ASSERT_EQ(0, status);

  for (double i = 0; i < 5; i += 0.1) {
    HAL_SetAnalogOutput(output, i, &status);
    ASSERT_EQ(0, status);
    usleep(1000);
    ASSERT_NEAR(i, HAL_GetAnalogVoltage(input, &status), 0.01);
    ASSERT_EQ(0, status);
  }

  for (double i = 5; i > 0; i -= 0.1) {
    HAL_SetAnalogOutput(output, i, &status);
    ASSERT_EQ(0, status);
    usleep(1000);
    ASSERT_NEAR(i, HAL_GetAnalogVoltage(input, &status), 0.01);
    ASSERT_EQ(0, status);
  }
}

TEST(AnalogInputTest, AllocateAll) {
  wpi::SmallVector<AnalogInputHandle, 21> analogHandles;
  for (int i = 0; i < HAL_GetNumAnalogInputs(); i++) {
    int32_t status = 0;
    analogHandles.emplace_back(AnalogInputHandle(i, &status));
    ASSERT_EQ(status, 0);
  }
}

TEST(AnalogInputTest, MultipleAllocateFails) {
  int32_t status = 0;
  AnalogInputHandle handle(0, &status);
  ASSERT_NE(handle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);

  AnalogInputHandle handle2(0, &status);
  ASSERT_EQ(handle2, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

TEST(AnalogInputTest, OverAllocateFails) {
  int32_t status = 0;
  AnalogInputHandle handle(HAL_GetNumAnalogInputs(), &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(AnalogInputTest, UnderAllocateFails) {
  int32_t status = 0;
  AnalogInputHandle handle(-1, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(AnalogOutputTest, AllocateAll) {
  wpi::SmallVector<AnalogOutputHandle, 21> analogHandles;
  for (int i = 0; i < HAL_GetNumAnalogOutputs(); i++) {
    int32_t status = 0;
    analogHandles.emplace_back(AnalogOutputHandle(i, &status));
    ASSERT_EQ(status, 0);
  }
}

TEST(AnalogOutputTest, MultipleAllocateFails) {
  int32_t status = 0;
  AnalogOutputHandle handle(0, &status);
  ASSERT_NE(handle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);

  AnalogOutputHandle handle2(0, &status);
  ASSERT_EQ(handle2, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

TEST(AnalogOutputTest, OverAllocateFails) {
  int32_t status = 0;
  AnalogOutputHandle handle(HAL_GetNumAnalogOutputs(), &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(AnalogOutputTest, UnderAllocateFails) {
  int32_t status = 0;
  AnalogOutputHandle handle(-1, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

INSTANTIATE_TEST_SUITE_P(AnalogCrossConnectsTests, AnalogCrossTest,
                         ::testing::ValuesIn(AnalogCrossConnects));
