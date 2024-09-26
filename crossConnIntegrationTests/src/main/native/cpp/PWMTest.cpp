// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <thread>
#include <utility>

#include <gtest/gtest.h>
#include <hal/DMA.h>
#include <hal/HAL.h>
#include <wpi/SmallVector.h>
#include <wpi/condition_variable.h>
#include <wpi/priority_mutex.h>

#include "CrossConnects.h"
#include "LifetimeWrappers.h"

using namespace hlt;

class PWMTest : public ::testing::TestWithParam<std::pair<int, int>> {};

void TestTimingDMA(int squelch, std::pair<int, int> param) {
  // Initialize DMA
  int32_t status = 0;
  DMAHandle dmaHandle(&status);
  ASSERT_NE(dmaHandle, HAL_kInvalidHandle);
  ASSERT_EQ(0, status);

  status = 0;
  PWMHandle pwmHandle(param.first, &status);
  ASSERT_NE(pwmHandle, HAL_kInvalidHandle);
  ASSERT_EQ(0, status);

  // Ensure our PWM is disabled, and set up properly
  HAL_SetPWMPulseTimeMicroseconds(pwmHandle, 0, &status);
  HAL_SetPWMConfigMicroseconds(pwmHandle, 2000, 1000, 1000, 0, 0, &status);
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

  HAL_AddDMADigitalSource(dmaHandle, dioHandle, &status);
  ASSERT_EQ(0, status);

  HAL_SetDMAExternalTrigger(dmaHandle, dioHandle,
                            HAL_AnalogTriggerType::HAL_Trigger_kInWindow, true,
                            true, &status);
  ASSERT_EQ(0, status);

  // Loop to test 5 speeds
  for (unsigned int testWidth = 1000; testWidth < 2100; testWidth += 250) {
    HAL_StartDMA(dmaHandle, 1024, &status);
    ASSERT_EQ(0, status);

    while (true) {
      int32_t remaining = 0;
      HAL_DMASample testSample;
      HAL_ReadDMA(dmaHandle, &testSample, 0.01, &remaining, &status);
      if (remaining == 0) {
        break;
      }
    }

    HAL_SetPWMSpeed(pwmHandle, (testWidth - 1000) / 1000.0, &status);

    constexpr const int kSampleCount = 15;
    HAL_DMASample dmaSamples[kSampleCount];
    int readCount = 0;
    while (readCount < kSampleCount) {
      status = 0;
      int32_t remaining = 0;
      HAL_DMAReadStatus readStatus = HAL_ReadDMA(
          dmaHandle, &dmaSamples[readCount], 1.0, &remaining, &status);
      ASSERT_EQ(0, status);
      ASSERT_EQ(HAL_DMAReadStatus::HAL_DMA_OK, readStatus);
      readCount++;
    }

    HAL_SetPWMSpeed(pwmHandle, 0, &status);
    HAL_StopDMA(dmaHandle, &status);

    // Find first rising edge
    int startIndex = 4;
    while (startIndex < 6) {
      status = 0;
      auto value = HAL_GetDMASampleDigitalSource(&dmaSamples[startIndex],
                                                 dioHandle, &status);
      ASSERT_EQ(0, status);
      if (value) {
        break;
      }
      startIndex++;
    }
    ASSERT_LT(startIndex, 6);

    // Check that samples alternate
    bool previous = false;
    int iterationCount = 0;
    for (int i = startIndex; i < startIndex + 8; i++) {
      auto value =
          HAL_GetDMASampleDigitalSource(&dmaSamples[i], dioHandle, &status);
      ASSERT_EQ(0, status);
      ASSERT_NE(previous, value);
      previous = !previous;
      iterationCount++;
    }
    ASSERT_EQ(iterationCount, 8);
    iterationCount = 0;

    // Check width between samples
    for (int i = startIndex; i < startIndex + 8; i += 2) {
      auto width = HAL_GetDMASampleTime(&dmaSamples[i + 1], &status) -
                   HAL_GetDMASampleTime(&dmaSamples[i], &status);
      ASSERT_NEAR(testWidth, width, 10);
      iterationCount++;
    }
    ASSERT_EQ(iterationCount, 4);
    iterationCount = 0;

    // Check period between samples
    for (int i = startIndex; i < startIndex + 6; i += 2) {
      auto period = HAL_GetDMASampleTime(&dmaSamples[i + 2], &status) -
                    HAL_GetDMASampleTime(&dmaSamples[i], &status);
      ASSERT_NEAR(checkPeriod, period, 10);
      iterationCount++;
    }
    ASSERT_EQ(iterationCount, 3);
  }
}

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
  HAL_SetPWMPulseTimeMicroseconds(pwmHandle, 0, &status);
  HAL_SetPWMConfigMicroseconds(pwmHandle, 2000, 1000, 1000, 0, 0, &status);
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

    HAL_SetPWMPulseTimeMicroseconds(pwmHandle, 0, &status);

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

TEST_P(PWMTest, Timing4x) {
  auto param = GetParam();
  TestTiming(3, param);
}

TEST_P(PWMTest, Timing2x) {
  auto param = GetParam();
  TestTiming(1, param);
}

TEST_P(PWMTest, Timing1x) {
  auto param = GetParam();
  TestTiming(0, param);
}

TEST_P(PWMTest, TimingDMA4x) {
  auto param = GetParam();
  TestTimingDMA(3, param);
}

TEST_P(PWMTest, TimingDMA2x) {
  auto param = GetParam();
  TestTimingDMA(1, param);
}

TEST_P(PWMTest, TimingDMA1x) {
  auto param = GetParam();
  TestTimingDMA(0, param);
}

TEST(PWMTest, AllocateAll) {
  wpi::SmallVector<PWMHandle, 21> pwmHandles;
  for (int i = 0; i < HAL_GetNumPWMChannels(); i++) {
    int32_t status = 0;
    pwmHandles.emplace_back(PWMHandle(i, &status));
    ASSERT_EQ(status, 0);
  }
}

TEST(PWMTest, MultipleAllocateFails) {
  int32_t status = 0;
  PWMHandle handle(0, &status);
  ASSERT_NE(handle, HAL_kInvalidHandle);
  ASSERT_EQ(status, 0);

  PWMHandle handle2(0, &status);
  ASSERT_EQ(handle2, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_IS_ALLOCATED);
}

TEST(PWMTest, OverAllocateFails) {
  int32_t status = 0;
  PWMHandle handle(HAL_GetNumPWMChannels(), &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(PWMTest, UnderAllocateFails) {
  int32_t status = 0;
  PWMHandle handle(-1, &status);
  ASSERT_EQ(handle, HAL_kInvalidHandle);
  ASSERT_LAST_ERROR_STATUS(status, RESOURCE_OUT_OF_RANGE);
}

TEST(PWMTest, CrossAllocationFails) {
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
