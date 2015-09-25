/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Counter.h>
#include <DigitalInput.h>
#include <DigitalOutput.h>
#include <Timer.h>
#include "gtest/gtest.h"
#include "TestBench.h"

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
  DigitalInput *m_input;
  DigitalOutput *m_output;

  virtual void SetUp() override {
    m_input = new DigitalInput(TestBench::kLoop1InputChannel);
    m_output = new DigitalOutput(TestBench::kLoop1OutputChannel);
  }

  virtual void TearDown() override {
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
 * Test a fake "counter" that uses the DIO loop as an input to make sure the
 * Counter class works
 */
TEST_F(DIOLoopTest, FakeCounter) {
  Reset();

  Counter counter(m_input);

  EXPECT_EQ(0, counter.Get()) << "Counter did not initialize to 0.";

  /* Count 100 ticks.  The counter value should be 100 after this loop. */
  for (int i = 0; i < 100; i++) {
    m_output->Set(true);
    Wait(kCounterTime);
    m_output->Set(false);
    Wait(kCounterTime);
  }

  EXPECT_EQ(100, counter.Get()) << "Counter did not count up to 100.";
}

static void InterruptHandler(uint32_t interruptAssertedMask, void *param) {
  *(int *)param = 12345;
}

TEST_F(DIOLoopTest, AsynchronousInterruptWorks) {
  int param = 0;

  // Given an interrupt handler that sets an int to 12345
  m_input->RequestInterrupts(InterruptHandler, &param);
  m_input->EnableInterrupts();

  // If the voltage rises
  m_output->Set(false);
  m_output->Set(true);
  m_input->CancelInterrupts();

  // Then the int should be 12345
  Wait(kDelayTime);
  EXPECT_EQ(12345, param) << "The interrupt did not run.";
}

static void *InterruptTriggerer(void *data) {
  DigitalOutput *output = static_cast<DigitalOutput *>(data);
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
  pthread_create(&interruptTriggererLoop, nullptr, InterruptTriggerer, m_output);

  // Then this thread should pause and resume after that number of seconds
  Timer timer;
  timer.Start();
  m_input->WaitForInterrupt(kSynchronousInterruptTime + 1.0);
  EXPECT_NEAR(kSynchronousInterruptTime, timer.Get(),
              kSynchronousInterruptTimeTolerance);
}
