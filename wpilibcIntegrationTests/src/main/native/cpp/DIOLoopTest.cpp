// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalInput.h"  // NOLINT(build/include_order)

#include "frc/DigitalOutput.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "frc/AsynchronousInterrupt.h"
#include "frc/Counter.h"
#include "frc/SynchronousInterrupt.h"
#include "frc/Timer.h"
#include "frc2/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kCounterTime = 0.001;

static const double kDelayTime = 0.1;

static const units::second_t kSynchronousInterruptTime = 2.0_s;
static const units::second_t kSynchronousInterruptTimeTolerance = 0.01_s;

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
  frc::SynchronousInterrupt interrupt{m_output};
  interrupt.SetInterruptEdges(false, true);
  frc::SynchronousInterrupt::WaitResult result =
      interrupt.WaitForInterrupt(3_s, true);

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

  EXPECT_EQ(frc::SynchronousInterrupt::WaitResult::kFallingEdge, result)
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

  frc::AsynchronousInterrupt interrupt(m_input,
                                       [&](auto a, auto b) { param = 12345; });

  interrupt.Enable();
  // If the voltage rises
  m_output->Set(false);
  m_output->Set(true);
  interrupt.Disable();

  // Then the int32_t should be 12345
  Wait(kDelayTime);
  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}

TEST_F(DIOLoopTest, SynchronousInterruptWorks) {
  // Given a synchronous interrupt
  frc::SynchronousInterrupt interrupt(m_input);

  std::thread thr([this]() {
    m_output->Set(false);
    frc2::Wait(kSynchronousInterruptTime);
    m_output->Set(true);
  });

  // Then this thread should pause and resume after that number of seconds
  frc2::Timer timer;
  timer.Start();
  interrupt.WaitForInterrupt(kSynchronousInterruptTime + 1.0_s);
  EXPECT_NEAR(kSynchronousInterruptTime.to<double>(), timer.Get().to<double>(),
              kSynchronousInterruptTimeTolerance.to<double>());
}
