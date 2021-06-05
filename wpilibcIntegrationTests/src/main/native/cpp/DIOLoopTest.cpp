// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalInput.h"  // NOLINT(build/include_order)

#include "frc/DigitalOutput.h"  // NOLINT(build/include_order)

#include <units/math.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/Counter.h"
#include "frc/InterruptableSensorBase.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

static constexpr auto kCounterTime = 1_ms;

static constexpr auto kDelayTime = 100_ms;

static constexpr auto kSynchronousInterruptTime = 2_s;
static constexpr auto kSynchronousInterruptTimeTolerance = 10_ms;

/**
 * A fixture with a digital input and a digital output physically wired
 * together.
 */
class DIOLoopTest : public testing::Test {
 protected:
  frc::DigitalInput m_input{TestBench::kLoop1InputChannel};
  frc::DigitalOutput m_output{TestBench::kLoop1OutputChannel};

  void Reset() { m_output.Set(false); }
};

/**
 * Test the DigitalInput and DigitalOutput classes by setting the output and
 * reading the input.
 */
TEST_F(DIOLoopTest, Loop) {
  Reset();

  m_output.Set(false);
  frc::Wait(kDelayTime);
  EXPECT_FALSE(m_input.Get()) << "The digital output was turned off, but "
                              << "the digital input is on.";

  m_output.Set(true);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(m_input.Get()) << "The digital output was turned on, but "
                             << "the digital input is off.";
}
/**
 * Tests to see if the DIO PWM functionality works.
 */
TEST_F(DIOLoopTest, DIOPWM) {
  Reset();

  m_output.Set(false);
  frc::Wait(kDelayTime);
  EXPECT_FALSE(m_input.Get()) << "The digital output was turned off, but "
                              << "the digital input is on.";

  // Set frequency to 2.0 Hz
  m_output.SetPWMRate(2.0);
  // Enable PWM, but leave it off
  m_output.EnablePWM(0.0);
  frc::Wait(0.5_s);
  m_output.UpdateDutyCycle(0.5);
  m_input.RequestInterrupts();
  m_input.SetUpSourceEdge(false, true);
  frc::InterruptableSensorBase::WaitResult result =
      m_input.WaitForInterrupt(3_s, true);

  frc::Wait(0.5_s);
  bool firstCycle = m_input.Get();
  frc::Wait(0.5_s);
  bool secondCycle = m_input.Get();
  frc::Wait(0.5_s);
  bool thirdCycle = m_input.Get();
  frc::Wait(0.5_s);
  bool forthCycle = m_input.Get();
  frc::Wait(0.5_s);
  bool fifthCycle = m_input.Get();
  frc::Wait(0.5_s);
  bool sixthCycle = m_input.Get();
  frc::Wait(0.5_s);
  bool seventhCycle = m_input.Get();
  m_output.DisablePWM();
  frc::Wait(0.5_s);
  bool firstAfterStop = m_input.Get();
  frc::Wait(0.5_s);
  bool secondAfterStop = m_input.Get();

  EXPECT_EQ(frc::InterruptableSensorBase::WaitResult::kFallingEdge, result)
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

  frc::Counter counter{&m_input};

  EXPECT_EQ(0, counter.Get()) << "Counter did not initialize to 0.";

  /* Count 100 ticks.  The counter value should be 100 after this loop. */
  for (int32_t i = 0; i < 100; ++i) {
    m_output.Set(true);
    frc::Wait(kCounterTime);
    m_output.Set(false);
    frc::Wait(kCounterTime);
  }

  EXPECT_EQ(100, counter.Get()) << "Counter did not count up to 100.";
}

static void InterruptHandler(uint32_t interruptAssertedMask, void* param) {
  *reinterpret_cast<int32_t*>(param) = 12345;
}

TEST_F(DIOLoopTest, AsynchronousInterruptWorks) {
  int32_t param = 0;

  // Given an interrupt handler that sets an int32_t to 12345
  m_input.RequestInterrupts(InterruptHandler, &param);
  m_input.EnableInterrupts();

  // If the voltage rises
  m_output.Set(false);
  m_output.Set(true);
  m_input.CancelInterrupts();

  // Then the int32_t should be 12345
  frc::Wait(kDelayTime);
  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}

static void* InterruptTriggerer(void* data) {
  auto& output = *static_cast<frc::DigitalOutput*>(data);
  output.Set(false);
  frc::Wait(kSynchronousInterruptTime);
  output.Set(true);
  return nullptr;
}

TEST_F(DIOLoopTest, SynchronousInterruptWorks) {
  // Given a synchronous interrupt
  m_input.RequestInterrupts();

  // If we have another thread trigger the interrupt in a few seconds
  pthread_t interruptTriggererLoop;
  pthread_create(&interruptTriggererLoop, nullptr, InterruptTriggerer,
                 &m_output);

  // Then this thread should pause and resume after that number of seconds
  frc::Timer timer;
  timer.Start();
  m_input.WaitForInterrupt(kSynchronousInterruptTime + 1_s);
  EXPECT_NEAR_UNITS(kSynchronousInterruptTime, timer.Get(),
                    kSynchronousInterruptTimeTolerance);
}
