// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <thread>

#include <hal/HAL.h>
#include <wpi/SmallVector.h>
#include <wpi/condition_variable.h>
#include <wpi/priority_mutex.h>

#include "CrossConnects.h"
#include "LifetimeWrappers.h"
#include "gtest/gtest.h"

using namespace hlt;

class PWMTest : public ::testing::TestWithParam<std::pair<int, int>> {};

struct InterruptCheckData {
  wpi::SmallVector<uint64_t, 8> risingStamps;
  wpi::SmallVector<uint64_t, 8> fallingStamps;
  wpi::priority_mutex mutex;
  wpi::condition_variable cond;
  HAL_InterruptHandle handle;
};

// TODO switch this to DMA
void TestTiming(int squelch, std::pair<int, int> param) {
  // Initialize interrupt
  int32_t status = 0;
  InterruptHandle interruptHandle(&status);
  // Ensure we have a valid interrupt handle
  ASSERT_NE(interruptHandle, HAL_kInvalidHandle);

  status = 0;
  PWMHandle pwmHandle(param.first, &status);
  ASSERT_NE(pwmHandle, HAL_kInvalidHandle);

  // Ensure our PWM is disabled, and set up properly
  HAL_SetPWMRaw(pwmHandle, 0, &status);
  HAL_SetPWMConfig(pwmHandle, 2.0, 1.0, 1.0, 0, 0, &status);
  HAL_SetPWMPeriodScale(pwmHandle, squelch, &status);

  unsigned int checkPeriod = 0;
  switch (squelch) {
    case (0):
      checkPeriod = 5050;
      break;
    case (1):
      checkPeriod = 10100;
      break;
    case (3):
      checkPeriod = 20200;
      break;
  }

  status = 0;
  DIOHandle dioHandle(param.second, true, &status);
  ASSERT_NE(dioHandle, HAL_kInvalidHandle);

  InterruptCheckData interruptData;
  interruptData.handle = interruptHandle;

  // Can use any type for the interrupt handle
  HAL_RequestInterrupts(interruptHandle, dioHandle,
                        HAL_AnalogTriggerType::HAL_Trigger_kInWindow, &status);

  HAL_SetInterruptUpSourceEdge(interruptHandle, true, true, &status);

  // Loop to test 5 speeds
  for (unsigned int i = 1000; i < 2100; i += 250) {
    interruptData.risingStamps.clear();
    interruptData.fallingStamps.clear();

    std::atomic_bool runThread{true};

    status = 0;
    std::thread interruptThread([&]() {
      while (runThread) {
        int32_t threadStatus = 0;
        auto mask =
            HAL_WaitForInterrupt(interruptHandle, 5, true, &threadStatus);

        if ((mask & 0x100) == 0x100 && interruptData.risingStamps.size() == 0 &&
            interruptData.fallingStamps.size() == 0) {
          // Falling edge at start of tracking. Skip
          continue;
        }

        int32_t status = 0;
        if ((mask & 0x1) == 0x1) {
          auto ts = HAL_ReadInterruptRisingTimestamp(interruptHandle, &status);
          // Rising Edge
          interruptData.risingStamps.push_back(ts);
        } else if ((mask & 0x100) == 0x100) {
          auto ts = HAL_ReadInterruptFallingTimestamp(interruptHandle, &status);
          // Falling Edge
          interruptData.fallingStamps.push_back(ts);
        }

        if (interruptData.risingStamps.size() >= 4 &&
            interruptData.fallingStamps.size() >= 4) {
          interruptData.cond.notify_all();
          runThread = false;
          break;
        }
      }
    });

    // Ensure our interrupt actually got created correctly.
    ASSERT_EQ(status, 0);
    HAL_SetPWMSpeed(pwmHandle, (i - 1000) / 1000.0, &status);
    ASSERT_EQ(status, 0);
    {
      std::unique_lock<wpi::priority_mutex> lock(interruptData.mutex);
      // Wait for lock
      // TODO: Add Timeout
      auto timeout = interruptData.cond.wait_for(lock, std::chrono::seconds(2));
      if (timeout == std::cv_status::timeout) {
        runThread = false;
        if (interruptThread.joinable()) {
          interruptThread.join();
        }
        ASSERT_TRUE(false);  // Exit test as failure on timeout
      }
    }

    HAL_SetPWMRaw(pwmHandle, 0, &status);

    // Ensure our interrupts have the proper counts
    ASSERT_EQ(interruptData.risingStamps.size(),
              interruptData.fallingStamps.size());

    ASSERT_GT(interruptData.risingStamps.size(), 0u);

    ASSERT_EQ(interruptData.risingStamps.size() % 2, 0u);
    ASSERT_EQ(interruptData.fallingStamps.size() % 2, 0u);

    for (size_t j = 0; j < interruptData.risingStamps.size(); j++) {
      uint64_t width =
          interruptData.fallingStamps[j] - interruptData.risingStamps[j];
      ASSERT_NEAR(width, i, 10);
    }

    for (unsigned int j = 0; j < interruptData.risingStamps.size() - 1; j++) {
      uint64_t period =
          interruptData.risingStamps[j + 1] - interruptData.risingStamps[j];
      ASSERT_NEAR(period, checkPeriod, 10);
    }
    runThread = false;
    if (interruptThread.joinable()) {
      interruptThread.join();
    }
  }
}

TEST_P(PWMTest, TestTiming4x) {
  auto param = GetParam();
  TestTiming(3, param);
}

TEST_P(PWMTest, TestTiming2x) {
  auto param = GetParam();
  TestTiming(1, param);
}

TEST_P(PWMTest, TestTiming1x) {
  auto param = GetParam();
  TestTiming(0, param);
}

TEST(PWMTest, TestAllocateAll) {
  wpi::SmallVector<PWMHandle, 21> pwmHandles;
  for (int i = 0; i < HAL_GetNumPWMChannels(); i++) {
    int32_t status = 0;
    pwmHandles.emplace_back(PWMHandle(i, &status));
    ASSERT_EQ(status, 0);
  }
}

TEST(PWMTest, TestMultipleAllocateFails) {
  int32_t status = 0;
  PWMHandle handle(0, &status);
  ASSERT_NE(handle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);

  PWMHandle handle2(0, &status);
  ASSERT_EQ(handle2, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

TEST(PWMTest, TestOverAllocateFails) {
  int32_t status = 0;
  PWMHandle handle(HAL_GetNumPWMChannels(), &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(PWMTest, TestUnderAllocateFails) {
  int32_t status = 0;
  PWMHandle handle(-1, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(PWMTest, TestCrossAllocationFails) {
  int32_t status = 0;
  DIOHandle dioHandle(10, true, &status);
  ASSERT_NE(dioHandle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);
  PWMHandle handle(10, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

INSTANTIATE_TEST_SUITE_P(PWMCrossConnectTests, PWMTest,
                         ::testing::ValuesIn(PWMCrossConnects));
