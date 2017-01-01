/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"  // NOLINT(build/include_order)

#include "AnalogOutput.h"
#include "AnalogTrigger.h"
#include "DigitalOutput.h"
#include "TestBench.h"
#include "Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static const double kDelayTime = 0.001;

class FakeEncoderTest : public testing::Test {
 protected:
  DigitalOutput* m_outputA;
  DigitalOutput* m_outputB;
  AnalogOutput* m_indexOutput;

  Encoder* m_encoder;
  AnalogTrigger* m_indexAnalogTrigger;
  std::shared_ptr<AnalogTriggerOutput> m_indexAnalogTriggerOutput;

  void SetUp() override {
    m_outputA = new DigitalOutput(TestBench::kLoop2OutputChannel);
    m_outputB = new DigitalOutput(TestBench::kLoop1OutputChannel);
    m_indexOutput = new AnalogOutput(TestBench::kAnalogOutputChannel);
    m_outputA->Set(false);
    m_outputB->Set(false);
    m_encoder = new Encoder(TestBench::kLoop1InputChannel,
                            TestBench::kLoop2InputChannel);
    m_indexAnalogTrigger =
        new AnalogTrigger(TestBench::kFakeAnalogOutputChannel);
    m_indexAnalogTrigger->SetLimitsVoltage(2.0, 3.0);
    m_indexAnalogTriggerOutput =
        m_indexAnalogTrigger->CreateOutput(AnalogTriggerType::kState);
  }

  void TearDown() override {
    delete m_outputA;
    delete m_outputB;
    delete m_indexOutput;
    delete m_encoder;
    delete m_indexAnalogTrigger;
  }

  /**
   * Output pulses to the encoder's input channels to simulate a change of 100
   * ticks
   */
  void Simulate100QuadratureTicks() {
    for (int32_t i = 0; i < 100; i++) {
      m_outputA->Set(true);
      Wait(kDelayTime);
      m_outputB->Set(true);
      Wait(kDelayTime);
      m_outputA->Set(false);
      Wait(kDelayTime);
      m_outputB->Set(false);
      Wait(kDelayTime);
    }
  }

  void SetIndexHigh() {
    m_indexOutput->SetVoltage(5.0);
    Wait(kDelayTime);
  }

  void SetIndexLow() {
    m_indexOutput->SetVoltage(0.0);
    Wait(kDelayTime);
  }
};

/**
 * Test the encoder by reseting it to 0 and reading the value.
 */
TEST_F(FakeEncoderTest, TestDefaultState) {
  EXPECT_FLOAT_EQ(0.0, m_encoder->Get()) << "The encoder did not start at 0.";
}

/**
 * Test the encoder by setting the digital outputs and reading the value.
 */
TEST_F(FakeEncoderTest, TestCountUp) {
  m_encoder->Reset();
  Simulate100QuadratureTicks();

  EXPECT_FLOAT_EQ(100.0, m_encoder->Get()) << "Encoder did not count to 100.";
}

/**
 * Test that the encoder can stay reset while the index source is high
 */
TEST_F(FakeEncoderTest, TestResetWhileHigh) {
  m_encoder->SetIndexSource(*m_indexAnalogTriggerOutput,
                            Encoder::IndexingType::kResetWhileHigh);

  SetIndexLow();
  Simulate100QuadratureTicks();
  SetIndexHigh();
  EXPECT_EQ(0, m_encoder->Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(0, m_encoder->Get());
}

/**
 * Test that the encoder can reset when the index source goes from low to high
 */
TEST_F(FakeEncoderTest, TestResetOnRisingEdge) {
  m_encoder->SetIndexSource(*m_indexAnalogTriggerOutput,
                            Encoder::IndexingType::kResetOnRisingEdge);

  SetIndexLow();
  Simulate100QuadratureTicks();
  SetIndexHigh();
  EXPECT_EQ(0, m_encoder->Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(100, m_encoder->Get());
}

/**
 * Test that the encoder can stay reset while the index source is low
 */
TEST_F(FakeEncoderTest, TestResetWhileLow) {
  m_encoder->SetIndexSource(*m_indexAnalogTriggerOutput,
                            Encoder::IndexingType::kResetWhileLow);

  SetIndexHigh();
  Simulate100QuadratureTicks();
  SetIndexLow();
  EXPECT_EQ(0, m_encoder->Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(0, m_encoder->Get());
}

/**
 * Test that the encoder can reset when the index source goes from high to low
 */
TEST_F(FakeEncoderTest, TestResetOnFallingEdge) {
  m_encoder->SetIndexSource(*m_indexAnalogTriggerOutput,
                            Encoder::IndexingType::kResetOnFallingEdge);

  SetIndexHigh();
  Simulate100QuadratureTicks();
  SetIndexLow();
  EXPECT_EQ(0, m_encoder->Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(100, m_encoder->Get());
}
