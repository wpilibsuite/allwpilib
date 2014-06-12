/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

enum CounterTestType { TEST_VICTOR, TEST_JAGUAR, TEST_TALON };

std::ostream &operator<<(std::ostream &os, CounterTestType const &type) {
	switch(type) {
	case TEST_VICTOR: os << "Victor"; break;
	case TEST_JAGUAR: os << "Jaguar"; break;
	case TEST_TALON: os << "Talon"; break;
	}

	return os;
}

static constexpr double kMotorTime = 0.5;

/**
 * Tests the counter by moving the motor and determining if the
 *  counter is counting.
 */

class CounterTest : public testing::TestWithParam<CounterTestType>  {
protected:
	SpeedController *m_speedController;
	Counter *m_counter;

	virtual void SetUp() {
		switch(GetParam()) {
		case TEST_VICTOR:
			m_counter = new Counter(TestBench::kVictorEncoderChannelA);
			m_speedController = new Victor(TestBench::kVictorChannel);
			break;

		case TEST_JAGUAR:
			m_counter = new Counter(TestBench::kJaguarEncoderChannelA);
			m_speedController = new Jaguar(TestBench::kJaguarChannel);
			break;

		case TEST_TALON:
			m_counter = new Counter(TestBench::kTalonEncoderChannelA);
			m_speedController = new Talon(TestBench::kTalonChannel);
			break;
		}
	}

	virtual void TearDown() {
		delete m_counter;
		delete m_speedController;
		}

	void Reset() {
		m_counter->Reset();
		m_speedController->Set(0.0f);
	}

};


	/**
	 * Test resetting a counter
	 */
	TEST_P(CounterTest, Reset) {
		Reset();
		EXPECT_FLOAT_EQ(0.0f, m_counter->Get()) << "Counter did not restart to 0.";
	}

	/**
	 * Test the counter by running the motor forward and determining if
	 * the counter value has changed.
	 */
	TEST_P(CounterTest, Count) {
		Reset();
		m_counter->Start();

		/*Run the motor forward and determine if the counter is counting. */
		m_speedController->Set(1.0f);
		Wait(kMotorTime);
		m_speedController->Set(0.0f);
		EXPECT_NE(0.0f, m_counter->Get()) << "Counter did not count.";
	}


	INSTANTIATE_TEST_CASE_P(Test, CounterTest,
		testing::Values(TEST_VICTOR, TEST_JAGUAR, TEST_TALON));




