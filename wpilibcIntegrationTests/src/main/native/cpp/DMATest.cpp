// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "TestBench.h"
#include "frc/AnalogInput.h"
#include "frc/AnalogOutput.h"
#include "frc/DMA.h"
#include "frc/DMASample.h"
#include "frc/DigitalOutput.h"
#include "frc/Timer.h"
#include "frc/motorcontrol/Jaguar.h"

using namespace frc;

static constexpr auto kDelayTime = 100_ms;

class DMATest : public testing::Test {
 protected:
  AnalogInput m_analogInput{TestBench::kAnalogOutputChannel};
  AnalogOutput m_analogOutput{TestBench::kFakeAnalogOutputChannel};
  DigitalOutput m_manualTrigger{TestBench::kLoop1InputChannel};
  Jaguar m_pwm{TestBench::kFakePwmOutput};
  DMA m_dma;

  void SetUp() override {
    m_dma.AddAnalogInput(&m_analogInput);
    m_dma.SetExternalTrigger(&m_manualTrigger, false, true);
    m_manualTrigger.Set(true);
  }
};

TEST_F(DMATest, DISABLED_PausingWorks) {
  m_dma.Start(1024);
  m_dma.SetPause(true);
  m_manualTrigger.Set(false);

  frc::DMASample sample;
  int32_t remaining = 0;
  int32_t status = 0;

  auto timedOut = sample.Update(&m_dma, 5_ms, &remaining, &status);

  ASSERT_EQ(DMASample::DMAReadStatus::kTimeout, timedOut);
}

TEST_F(DMATest, DISABLED_RemovingTriggersWorks) {
  m_dma.ClearExternalTriggers();
  m_dma.Start(1024);
  m_manualTrigger.Set(false);

  frc::DMASample sample;
  int32_t remaining = 0;
  int32_t status = 0;

  auto timedOut = sample.Update(&m_dma, 5_ms, &remaining, &status);

  ASSERT_EQ(DMASample::DMAReadStatus::kTimeout, timedOut);
}

TEST_F(DMATest, DISABLED_ManualTriggerOnlyHappensOnce) {
  m_dma.Start(1024);
  m_manualTrigger.Set(false);

  frc::DMASample sample;
  int32_t remaining = 0;
  int32_t status = 0;

  auto timedOut = sample.Update(&m_dma, 5_ms, &remaining, &status);

  ASSERT_EQ(DMASample::DMAReadStatus::kOk, timedOut);
  ASSERT_EQ(0, remaining);
  timedOut = sample.Update(&m_dma, 5_ms, &remaining, &status);
  ASSERT_EQ(DMASample::DMAReadStatus::kTimeout, timedOut);
}

TEST_F(DMATest, DISABLED_AnalogIndividualTriggers) {
  m_dma.Start(1024);
  for (double i = 0; i < 5; i += 0.5) {
    frc::DMASample sample;
    int32_t remaining = 0;
    int32_t status = 0;

    m_analogOutput.SetVoltage(i);
    frc::Wait(kDelayTime);
    m_manualTrigger.Set(false);
    auto timedOut = sample.Update(&m_dma, 1_ms, &remaining, &status);
    m_manualTrigger.Set(true);
    ASSERT_EQ(DMASample::DMAReadStatus::kOk, timedOut);
    ASSERT_EQ(0, status);
    ASSERT_EQ(0, remaining);
    ASSERT_DOUBLE_EQ(m_analogInput.GetVoltage(),
                     sample.GetAnalogInputVoltage(&m_analogInput, &status));
  }
}

TEST_F(DMATest, DISABLED_AnalogMultipleTriggers) {
  m_dma.Start(1024);
  std::vector<double> values;
  for (double i = 0; i < 5; i += 0.5) {
    values.push_back(i);
    m_analogOutput.SetVoltage(i);
    frc::Wait(kDelayTime);
    m_manualTrigger.Set(false);
    frc::Wait(kDelayTime);
    m_manualTrigger.Set(true);
  }

  for (size_t i = 0; i < values.size(); i++) {
    frc::DMASample sample;
    int32_t remaining = 0;
    int32_t status = 0;
    auto timedOut = sample.Update(&m_dma, 1_ms, &remaining, &status);
    ASSERT_EQ(DMASample::DMAReadStatus::kOk, timedOut);
    ASSERT_EQ(0, status);
    ASSERT_EQ(values.size() - i - 1, (uint32_t)remaining);
    ASSERT_DOUBLE_EQ(values[i],
                     sample.GetAnalogInputVoltage(&m_analogInput, &status));
  }
}

TEST_F(DMATest, DISABLED_TimedTriggers) {
  m_dma.SetTimedTrigger(10_ms);
  m_dma.Start(1024);
  frc::Wait(kDelayTime);
  m_dma.SetPause(true);

  frc::DMASample sample;
  int32_t remaining = 0;
  int32_t status = 0;

  auto timedOut = sample.Update(&m_dma, 1_ms, &remaining, &status);
  ASSERT_EQ(DMASample::DMAReadStatus::kOk, timedOut);
  ASSERT_EQ(0, status);
  ASSERT_GT(remaining, 5);
}

TEST_F(DMATest, DISABLED_PWMTimedTriggers) {
  m_dma.ClearExternalTriggers();
  m_dma.SetPwmEdgeTrigger(&m_pwm, true, false);
  m_dma.Start(1024);
  frc::Wait(kDelayTime);
  m_dma.SetPause(true);

  frc::DMASample sample;
  int32_t remaining = 0;
  int32_t status = 0;

  auto timedOut = sample.Update(&m_dma, 1_ms, &remaining, &status);
  ASSERT_EQ(DMASample::DMAReadStatus::kOk, timedOut);
  ASSERT_EQ(0, status);
  ASSERT_GT(remaining, 5);
}
