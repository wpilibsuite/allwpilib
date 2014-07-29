/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static const double kDelayTime = 0.01;

/**
 * A fixture with an analog input and an analog output wired together
 */
class AnalogLoopTest : public testing::Test {
protected:
	AnalogInput *m_input;
	AnalogOutput *m_output;

	virtual void SetUp() {
		m_input = new AnalogInput(TestBench::kFakeAnalogOutputChannel);
		m_output = new AnalogOutput(TestBench::kAnalogOutputChannel);
	}

	virtual void TearDown() {
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
	for(int i = 0; i < 50; i++) {
		m_output->SetVoltage(i / 10.0f);

		Wait(kDelayTime);

		EXPECT_NEAR(m_output->GetVoltage(), m_input->GetVoltage(), 0.01f);
	}
}

/**
 * Test if we can use an analog trigger to  check if the output is within a
 * range correctly.
 */
TEST_F(AnalogLoopTest, AnalogTriggerWorks) {
	AnalogTrigger trigger(m_input);
	trigger.SetLimitsVoltage(2.0f, 3.0f);

	m_output->SetVoltage(1.0f);
	Wait(kDelayTime);

	EXPECT_FALSE(trigger.GetInWindow()) << "Analog trigger is in the window (2V, 3V)";
	EXPECT_FALSE(trigger.GetTriggerState()) << "Analog trigger is on";

	m_output->SetVoltage(2.5f);
	Wait(kDelayTime);

	EXPECT_TRUE(trigger.GetInWindow()) << "Analog trigger is not in the window (2V, 3V)";
	EXPECT_FALSE(trigger.GetTriggerState()) << "Analog trigger is on";

	m_output->SetVoltage(4.0f);
	Wait(kDelayTime);

	EXPECT_FALSE(trigger.GetInWindow()) << "Analog trigger is in the window (2V, 3V)";
	EXPECT_TRUE(trigger.GetTriggerState()) << "Analog trigger is not on";
}

/**
 * Test if we can count the right number of ticks from an analog trigger with
 * a counter.
 */
TEST_F(AnalogLoopTest, AnalogTriggerCounterWorks) {
	AnalogTrigger trigger(m_input);
	trigger.SetLimitsVoltage(2.0f, 3.0f);

	Counter counter(trigger);

	// Turn the analog output low and high 50 times
	for(int i = 0; i < 50; i++) {
		m_output->SetVoltage(1.0);
		Wait(kDelayTime);
		m_output->SetVoltage(4.0);
		Wait(kDelayTime);
	}

	// The counter should be 50
	EXPECT_EQ(50, counter.Get()) << "Analog trigger counter did not count 100 ticks";
}
