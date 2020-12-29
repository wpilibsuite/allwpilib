// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TestBench.h"
#include "frc/AnalogInput.h"
#include "frc/AnalogOutput.h"
#include "frc/AnalogTrigger.h"
#include "frc/Counter.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kDelayTime = 0.01;

/**
 * A fixture with an analog input and an analog output wired together
 */
class AnalogLoopTest : public testing::Test {
 protected:
  AnalogInput* m_input;
  AnalogOutput* m_output;

  void SetUp() override {
    m_input = new AnalogInput(TestBench::kFakeAnalogOutputChannel);
    m_output = new AnalogOutput(TestBench::kAnalogOutputChannel);
  }

  void TearDown() override {
    delete m_input;
    delete m_output;
  }
};

/**
 * Test analog inputs and outputs by setting one and making sure the other
 * matches.
 */
TEST_F(AnalogLoopTest, AnalogInputWorks) {
  // Set the output voltage and check if the input measures the same voltage
  for (int32_t i = 0; i < 50; i++) {
    m_output->SetVoltage(i / 10.0);

    Wait(kDelayTime);

    EXPECT_NEAR(m_output->GetVoltage(), m_input->GetVoltage(), 0.01);
  }
}

/**
 * Test if we can use an analog trigger to  check if the output is within a
 * range correctly.
 */
TEST_F(AnalogLoopTest, AnalogTriggerWorks) {
  AnalogTrigger trigger(m_input);
  trigger.SetLimitsVoltage(2.0, 3.0);

  m_output->SetVoltage(1.0);
  Wait(kDelayTime);

  EXPECT_FALSE(trigger.GetInWindow())
      << "Analog trigger is in the window (2V, 3V)";
  EXPECT_FALSE(trigger.GetTriggerState()) << "Analog trigger is on";

  m_output->SetVoltage(2.5);
  Wait(kDelayTime);

  EXPECT_TRUE(trigger.GetInWindow())
      << "Analog trigger is not in the window (2V, 3V)";
  EXPECT_FALSE(trigger.GetTriggerState()) << "Analog trigger is on";

  m_output->SetVoltage(4.0);
  Wait(kDelayTime);

  EXPECT_FALSE(trigger.GetInWindow())
      << "Analog trigger is in the window (2V, 3V)";
  EXPECT_TRUE(trigger.GetTriggerState()) << "Analog trigger is not on";
}

/**
 * Test if we can count the right number of ticks from an analog trigger with
 * a counter.
 */
TEST_F(AnalogLoopTest, AnalogTriggerCounterWorks) {
  AnalogTrigger trigger(m_input);
  trigger.SetLimitsVoltage(2.0, 3.0);

  Counter counter(trigger);

  // Turn the analog output low and high 50 times
  for (int32_t i = 0; i < 50; i++) {
    m_output->SetVoltage(1.0);
    Wait(kDelayTime);
    m_output->SetVoltage(4.0);
    Wait(kDelayTime);
  }

  // The counter should be 50
  EXPECT_EQ(50, counter.Get())
      << "Analog trigger counter did not count 50 ticks";
}

static void InterruptHandler(uint32_t interruptAssertedMask, void* param) {
  *reinterpret_cast<int32_t*>(param) = 12345;
}

TEST_F(AnalogLoopTest, AsynchronusInterruptWorks) {
  int32_t param = 0;
  AnalogTrigger trigger(m_input);
  trigger.SetLimitsVoltage(2.0, 3.0);

  // Given an interrupt handler that sets an int32_t to 12345
  std::shared_ptr<AnalogTriggerOutput> triggerOutput =
      trigger.CreateOutput(AnalogTriggerType::kState);
  triggerOutput->RequestInterrupts(InterruptHandler, &param);
  triggerOutput->EnableInterrupts();

  // If the analog output moves from below to above the window
  m_output->SetVoltage(0.0);
  Wait(kDelayTime);
  m_output->SetVoltage(5.0);
  triggerOutput->CancelInterrupts();

  // Then the int32_t should be 12345
  Wait(kDelayTime);
  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}
