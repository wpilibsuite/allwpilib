// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalInput.h"  // NOLINT(build/include_order)

#include "frc/DigitalOutput.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "frc/Counter.h"
#include "frc/InterruptableSensorBase.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kCounterTime = 0.001;

static const double kDelayTime = 0.1;

static const double kSynchronousInterruptTime = 2.0;
static const double kSynchronousInterruptTimeTolerance = 0.01;

/**
 * A fixture with a digital input and a digital output physically wired
 * together.
 */
class DIOLoopTest : public testing::Test {
 protected:
  DigitalInput* m_input;
  DigitalOutput* m_output;

  void SetUp() override {
    m_input = new DigitalInput(TestBench::kLoop1InputChannel);
    m_output = new DigitalOutput(TestBench::kLoop1OutputChannel);
  }

  void TearDown() override {
    delete m_input;
    delete m_output;
  }

  void Reset() { m_output->Set(false); }
};

/**
 * Test the DigitalInput and DigitalOutput classes by setting the output and
 * reading the input.
 */
TEST_F(DIOLoopTest, Loop) {
  Reset();

  m_output->Set(false);
  Wait(kDelayTime);
  EXPECT_FALSE(m_input->Get()) << "The digital output was turned off, but "
                               << "the digital input is on.";

  m_output->Set(true);
  Wait(kDelayTime);
  EXPECT_TRUE(m_input->Get()) << "The digital output was turned on, but "
                              << "the digital input is off.";
}
/**
 * Tests to see if the DIO PWM functionality works.
 */
TEST_F(DIOLoopTest, DIOPWM) {
  Reset();

  m_output->Set(false);
  Wait(kDelayTime);
  EXPECT_FALSE(m_input->Get()) << "The digital output was turned off, but "
                               << "the digital input is on.";

  // Set frequency to 2.0 Hz
  m_output->SetPWMRate(2.0);
  // Enable PWM, but leave it off
  m_output->EnablePWM(0.0);
  Wait(0.5);
  m_output->UpdateDutyCycle(0.5);
  m_input->RequestInterrupts();
  m_input->SetUpSourceEdge(false, true);
  InterruptableSensorBase::WaitResult result =
      m_input->WaitForInterrupt(3.0, true);

  Wait(0.5);
  bool firstCycle = m_input->Get();
  Wait(0.5);
  bool secondCycle = m_input->Get();
  Wait(0.5);
  bool thirdCycle = m_input->Get();
  Wait(0.5);
  bool forthCycle = m_input->Get();
  Wait(0.5);
  bool fifthCycle = m_input->Get();
  Wait(0.5);
  bool sixthCycle = m_input->Get();
  Wait(0.5);
  bool seventhCycle = m_input->Get();
  m_output->DisablePWM();
  Wait(0.5);
  bool firstAfterStop = m_input->Get();
  Wait(0.5);
  bool secondAfterStop = m_input->Get();

  EXPECT_EQ(InterruptableSensorBase::WaitResult::kFallingEdge, result)
      << "WaitForInterrupt was not falling.";

  EXPECT_FALSE(firstCycle) << "Input not low after first delay";
  EXPECT_TRUE(secondCycle) << "Input not high after second delay";
  EXPECT_FALSE(thirdCycle) << "Input not low after third delay";
  EXPECT_TRUE(forthCycle) << "Input not high after forth delay";
  EXPECT_FALSE(fifthCycle) << "Input not low after fifth delay";
  EXPECT_TRUE(sixthCycle) << "Input not high after sixth delay";
  EXPECT_FALSE(seventhCycle) << "Input not low after seventh delay";
  EXPECT_FALSE(firstAfterStop) << "Input not low after stopping first read";
  EXPECT_FALSE(secondAfterStop) << "Input not low after stopping second read";
}

/**
 * Test a fake "counter" that uses the DIO loop as an input to make sure the
 * Counter class works
 */
TEST_F(DIOLoopTest, FakeCounter) {
  Reset();

  Counter counter(m_input);

  EXPECT_EQ(0, counter.Get()) << "Counter did not initialize to 0.";

  /* Count 100 ticks.  The counter value should be 100 after this loop. */
  for (int32_t i = 0; i < 100; i++) {
    m_output->Set(true);
    Wait(kCounterTime);
    m_output->Set(false);
    Wait(kCounterTime);
  }

  EXPECT_EQ(100, counter.Get()) << "Counter did not count up to 100.";
}

static void InterruptHandler(uint32_t interruptAssertedMask, void* param) {
  *reinterpret_cast<int32_t*>(param) = 12345;
}

TEST_F(DIOLoopTest, AsynchronousInterruptWorks) {
  int32_t param = 0;

  // Given an interrupt handler that sets an int32_t to 12345
  m_input->RequestInterrupts(InterruptHandler, &param);
  m_input->EnableInterrupts();

  // If the voltage rises
  m_output->Set(false);
  m_output->Set(true);
  m_input->CancelInterrupts();

  // Then the int32_t should be 12345
  Wait(kDelayTime);
  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}

static void* InterruptTriggerer(void* data) {
  DigitalOutput* output = static_cast<DigitalOutput*>(data);
  output->Set(false);
  Wait(kSynchronousInterruptTime);
  output->Set(true);
  return nullptr;
}

TEST_F(DIOLoopTest, SynchronousInterruptWorks) {
  // Given a synchronous interrupt
  m_input->RequestInterrupts();

  // If we have another thread trigger the interrupt in a few seconds
  pthread_t interruptTriggererLoop;
  pthread_create(&interruptTriggererLoop, nullptr, InterruptTriggerer,
                 m_output);

  // Then this thread should pause and resume after that number of seconds
  Timer timer;
  timer.Start();
  m_input->WaitForInterrupt(kSynchronousInterruptTime + 1.0);
  EXPECT_NEAR(kSynchronousInterruptTime, timer.Get(),
              kSynchronousInterruptTimeTolerance);
}
