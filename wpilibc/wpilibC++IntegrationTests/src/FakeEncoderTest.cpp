/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code	  */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

static const double kDelayTime = 0.01;

class FakeEncoderTest : public testing::Test {
protected:
	Encoder *m_encoder;
	DigitalOutput *m_outputA;
	DigitalOutput *m_outputB;

	virtual void SetUp() {
		m_outputA = new DigitalOutput(TestBench::kLoop2OutputChannel);
		m_outputB = new DigitalOutput(TestBench::kLoop1OutputChannel);
		m_encoder = new Encoder(TestBench::kLoop1InputChannel, TestBench::kLoop2InputChannel);
	}

	virtual void TearDown() {
		delete m_encoder;
		delete m_outputA;
		delete m_outputB;
	}
};

/**
 * Test the encoder by reseting it to 0 and reading the value.
 */
TEST_F(FakeEncoderTest, TestDefaultState) {
	EXPECT_FLOAT_EQ(0.0f, m_encoder->Get())
		<< "The encoder did not start at 0.";
}

/**
 * Test the encoder by setting the digital outputs and reading the value.
 */
TEST_F(FakeEncoderTest, TestCountUp) {
	m_outputA->Set(false);
	m_outputB->Set(false);
	m_encoder->Reset();

	//Sets the outputs such that the encoder moves in the positive direction
	for(int i = 0; i < 100; i++) {
		m_outputA->Set(true);
		Wait(kDelayTime);
		m_outputB->Set(true);
		Wait(kDelayTime);
		m_outputA->Set(false);
		Wait(kDelayTime);
		m_outputB->Set(false);
		Wait(kDelayTime);
	}

	EXPECT_FLOAT_EQ(100.0f, m_encoder->Get())
		<< "Encoder did not count to 100.";
}
