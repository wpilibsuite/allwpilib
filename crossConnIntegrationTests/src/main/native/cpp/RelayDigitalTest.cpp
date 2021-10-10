// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <hal/Relay.h>
#include <wpi/SmallVector.h>

#include "CrossConnects.h"
#include "LifetimeWrappers.h"
#include "gtest/gtest.h"

using namespace hlt;

class RelayDigitalTest : public ::testing::TestWithParam<RelayCross> {};

TEST_P(RelayDigitalTest, RelayCross) {
  auto param = GetParam();
  int32_t status = 0;
  RelayHandle fwd{param.Relay, true, &status};
  ASSERT_EQ(0, status);
  RelayHandle rev{param.Relay, false, &status};
  ASSERT_EQ(0, status);
  DIOHandle fwdInput{param.FwdDio, true, &status};
  ASSERT_EQ(0, status);
  DIOHandle revInput{param.RevDio, true, &status};
  ASSERT_EQ(0, status);

  HAL_SetRelay(fwd, false, &status);
  ASSERT_EQ(0, status);
  HAL_SetRelay(rev, false, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_FALSE(HAL_GetDIO(fwdInput, &status));
  ASSERT_EQ(0, status);
  ASSERT_FALSE(HAL_GetDIO(revInput, &status));
  ASSERT_EQ(0, status);

  HAL_SetRelay(fwd, false, &status);
  ASSERT_EQ(0, status);
  HAL_SetRelay(rev, true, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_FALSE(HAL_GetDIO(fwdInput, &status));
  ASSERT_EQ(0, status);
  ASSERT_TRUE(HAL_GetDIO(revInput, &status));
  ASSERT_EQ(0, status);

  HAL_SetRelay(fwd, true, &status);
  ASSERT_EQ(0, status);
  HAL_SetRelay(rev, false, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_TRUE(HAL_GetDIO(fwdInput, &status));
  ASSERT_EQ(0, status);
  ASSERT_FALSE(HAL_GetDIO(revInput, &status));
  ASSERT_EQ(0, status);

  HAL_SetRelay(fwd, true, &status);
  ASSERT_EQ(0, status);
  HAL_SetRelay(rev, true, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_TRUE(HAL_GetDIO(fwdInput, &status));
  ASSERT_EQ(0, status);
  ASSERT_TRUE(HAL_GetDIO(revInput, &status));
  ASSERT_EQ(0, status);
}

TEST(RelayDigitalTest, AllocateAll) {
  wpi::SmallVector<RelayHandle, 32> relayHandles;
  for (int i = 0; i < HAL_GetNumRelayChannels(); i++) {
    int32_t status = 0;
    relayHandles.emplace_back(i / 2, i % 2, &status);
    ASSERT_EQ(status, 0);
  }
}

TEST(RelayDigitalTest, MultipleAllocateFails) {
  int32_t status = 0;
  RelayHandle handle(0, true, &status);
  ASSERT_NE(handle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);

  RelayHandle handle2(0, true, &status);
  ASSERT_EQ(handle2, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

TEST(RelayDigitalTest, OverAllocateFails) {
  int32_t status = 0;
  RelayHandle handle(HAL_GetNumRelayChannels(), true, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(RelayDigitalTest, UnderAllocateFails) {
  int32_t status = 0;
  RelayHandle handle(-1, true, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

INSTANTIATE_TEST_SUITE_P(RelayDigitalCrossConnectsTests, RelayDigitalTest,
                         ::testing::ValuesIn(RelayCrossConnects));
