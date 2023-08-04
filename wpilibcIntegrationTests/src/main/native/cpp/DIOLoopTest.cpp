// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DigitalInput.h"  // NOLINT(build/include_order)

#include "frc/DigitalOutput.h"  // NOLINT(build/include_order)

#include <units/math.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/AsynchronousInterrupt.h"
#include "frc/Counter.h"
#include "frc/SynchronousInterrupt.h"
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
  frc::SynchronousInterrupt interrupt{m_output};
  interrupt.SetInterruptEdges(false, true);
  frc::SynchronousInterrupt::WaitResult result =
      interrupt.WaitForInterrupt(3_s, true);

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

TEST_F(DIOLoopTest, AsynchronousInterruptWorks) {
  Reset();

  int32_t param = 0;

  frc::AsynchronousInterrupt interrupt(m_input,
                                       [&](auto a, auto b) { param = 12345; });

  interrupt.Enable();
  // If the voltage rises
  m_output.Set(false);
  m_output.Set(true);

  // Then the int32_t should be 12345
  frc::Wait(kDelayTime);

  interrupt.Disable();

  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}

TEST_F(DIOLoopTest, SynchronousInterruptWorks) {
  Reset();

  // Given a synchronous interrupt
  frc::SynchronousInterrupt interrupt(m_input);

  std::thread thr([this]() {
    m_output.Set(false);
    frc::Wait(kSynchronousInterruptTime);
    m_output.Set(true);
  });

  // Then this thread should pause and resume after that number of seconds
  frc::Timer timer;
  timer.Start();
  interrupt.WaitForInterrupt(kSynchronousInterruptTime + 1_s);
  auto time = timer.Get().value();
  if (thr.joinable())
    thr.join();
  EXPECT_NEAR(kSynchronousInterruptTime.value(), time,
              kSynchronousInterruptTimeTolerance.value());
}
