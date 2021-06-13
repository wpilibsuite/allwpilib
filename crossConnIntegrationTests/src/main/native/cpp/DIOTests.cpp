#include "CrossConnects.h"

#include "gtest/gtest.h"
#include "LifetimeWrappers.h"

#include <wpi/SmallVector.h>
#include "hal/DIO.h"

using namespace hlt;

class DIOTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(DIOTest, TestDIOCross) {
  auto param = GetParam();
  int32_t status = 0;
  DIOHandle first{param.first, false, &status};
  ASSERT_EQ(0, status);
  DIOHandle second{param.second, true, &status};
  ASSERT_EQ(0, status);

  HAL_SetDIO(first, false, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_FALSE(HAL_GetDIO(first, &status));
  ASSERT_EQ(0, status);
  ASSERT_FALSE(HAL_GetDIO(second, &status));
  ASSERT_EQ(0, status);

  HAL_SetDIO(first, true, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_TRUE(HAL_GetDIO(second, &status));
  ASSERT_EQ(0, status);

  HAL_SetDIODirection(first, true, &status);
  ASSERT_EQ(0, status);
  HAL_SetDIODirection(second, false, &status);
  ASSERT_EQ(0, status);

  HAL_SetDIO(second, false, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_FALSE(HAL_GetDIO(first, &status));
  ASSERT_EQ(0, status);

  HAL_SetDIO(second, true, &status);
  ASSERT_EQ(0, status);
  usleep(1000);
  ASSERT_TRUE(HAL_GetDIO(first, &status));
  ASSERT_EQ(0, status);
}

TEST(DIOTest, TestAllocateAll) {
  wpi::SmallVector<DIOHandle, 32> dioHandles;
  for (int i = 0; i < HAL_GetNumDigitalChannels(); i++) {
    int32_t status = 0;
    dioHandles.emplace_back(i, true, &status);
    ASSERT_EQ(status, 0);
  }
}

TEST(DIOTest, TestMultipleAllocateFails) {
  int32_t status = 0;
  DIOHandle handle(0, true, &status);
  ASSERT_NE(handle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);

  DIOHandle handle2(0, true, &status);
  ASSERT_EQ(handle2, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

TEST(DIOTest, TestOverAllocateFails) {
  int32_t status = 0;
  DIOHandle handle(HAL_GetNumDigitalChannels(), true, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(DIOTest, TestUnderAllocateFails) {
  int32_t status = 0;
  DIOHandle handle(-1, true, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(DIOTest, TestCrossAllocationFails) {
  int32_t status = 0;
  PWMHandle pwmHandle(10, &status);
  ASSERT_NE(pwmHandle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);
  DIOHandle handle(10, true, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

INSTANTIATE_TEST_SUITE_P(DIOCrossConnectsTest, DIOTest,
                         ::testing::ValuesIn(DIOCrossConnects));
