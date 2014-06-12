/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static const double kDelayTime = 0.1;

/**
 * A fixture with a digital input and a digital output physically wired
 * together.
 */
class DIOLoopTest : public testing::Test {
protected:
	DigitalInput *m_input;
	DigitalOutput *m_output;

	virtual void SetUp() {
		m_input = new DigitalInput(TestBench::kLoop1InputChannel);
		m_output = new DigitalOutput(TestBench::kLoop1OutputChannel);
	}

	virtual void TearDown() {
		delete m_input;
		delete m_output;
	}

	void Reset() {
		m_output->Set(false);
	}
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
	counter.Start();

	EXPECT_EQ(0, counter.Get()) << "Counter did not initialize to 0.";

	/* Count 100 ticks.  The counter value should be 100 after this loop. */
	for(int i = 0; i < 100; i++) {
		m_output->Set(true);
		m_output->Set(false);
	}
  
  Wait(kDelayTime);

	EXPECT_EQ(100, counter.Get()) << "Counter did not count up to 100.";
}
