// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Encoder.h"  // NOLINT(build/include_order)

#include <memory>

#include <gtest/gtest.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/AnalogOutput.h"
#include "frc/AnalogTrigger.h"
#include "frc/DigitalOutput.h"
#include "frc/Timer.h"

static constexpr auto kDelayTime = 1_ms;

class FakeEncoderTest : public testing::Test {
 protected:
  frc::DigitalOutput m_outputA{TestBench::kLoop2OutputChannel};
  frc::DigitalOutput m_outputB{TestBench::kLoop1OutputChannel};
  frc::AnalogOutput m_indexOutput{TestBench::kAnalogOutputChannel};

  frc::Encoder m_encoder{TestBench::kLoop1InputChannel,
                         TestBench::kLoop2InputChannel};
  frc::AnalogTrigger m_indexAnalogTrigger{TestBench::kFakeAnalogOutputChannel};
  std::shared_ptr<frc::AnalogTriggerOutput> m_indexAnalogTriggerOutput =
      m_indexAnalogTrigger.CreateOutput(frc::AnalogTriggerType::kState);

  FakeEncoderTest() {
    m_outputA.Set(false);
    m_outputB.Set(false);
    m_indexAnalogTrigger.SetLimitsVoltage(2.0, 3.0);
  }

  /**
   * Output pulses to the encoder's input channels to simulate a change of 100
   * ticks
   */
  void Simulate100QuadratureTicks() {
    for (int32_t i = 0; i < 100; i++) {
      m_outputA.Set(true);
      frc::Wait(kDelayTime);
      m_outputB.Set(true);
      frc::Wait(kDelayTime);
      m_outputA.Set(false);
      frc::Wait(kDelayTime);
      m_outputB.Set(false);
      frc::Wait(kDelayTime);
    }
  }

  void SetIndexHigh() {
    m_indexOutput.SetVoltage(5.0);
    frc::Wait(kDelayTime);
  }

  void SetIndexLow() {
    m_indexOutput.SetVoltage(0.0);
    frc::Wait(kDelayTime);
  }
};

/**
 * Test the encoder by resetting it to 0 and reading the value.
 */
TEST_F(FakeEncoderTest, DefaultState) {
  EXPECT_DOUBLE_EQ(0.0, m_encoder.Get()) << "The encoder did not start at 0.";
}

/**
 * Test the encoder by setting the digital outputs and reading the value.
 */
TEST_F(FakeEncoderTest, CountUp) {
  m_encoder.Reset();
  Simulate100QuadratureTicks();

  EXPECT_DOUBLE_EQ(100.0, m_encoder.Get()) << "Encoder did not count to 100.";
}

/**
 * Test that the encoder can stay reset while the index source is high
 */
TEST_F(FakeEncoderTest, ResetWhileHigh) {
  m_encoder.SetIndexSource(*m_indexAnalogTriggerOutput,
                           frc::Encoder::IndexingType::kResetWhileHigh);

  SetIndexLow();
  Simulate100QuadratureTicks();
  SetIndexHigh();
  EXPECT_EQ(0, m_encoder.Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(0, m_encoder.Get());
}

/**
 * Test that the encoder can reset when the index source goes from low to high
 */
TEST_F(FakeEncoderTest, ResetOnRisingEdge) {
  m_encoder.SetIndexSource(*m_indexAnalogTriggerOutput,
                           frc::Encoder::IndexingType::kResetOnRisingEdge);

  SetIndexLow();
  Simulate100QuadratureTicks();
  SetIndexHigh();
  EXPECT_EQ(0, m_encoder.Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(100, m_encoder.Get());
}

/**
 * Test that the encoder can stay reset while the index source is low
 */
TEST_F(FakeEncoderTest, ResetWhileLow) {
  m_encoder.SetIndexSource(*m_indexAnalogTriggerOutput,
                           frc::Encoder::IndexingType::kResetWhileLow);

  SetIndexHigh();
  Simulate100QuadratureTicks();
  SetIndexLow();
  EXPECT_EQ(0, m_encoder.Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(0, m_encoder.Get());
}

/**
 * Test that the encoder can reset when the index source goes from high to low
 */
TEST_F(FakeEncoderTest, ResetOnFallingEdge) {
  m_encoder.SetIndexSource(*m_indexAnalogTriggerOutput,
                           frc::Encoder::IndexingType::kResetOnFallingEdge);

  SetIndexHigh();
  Simulate100QuadratureTicks();
  SetIndexLow();
  EXPECT_EQ(0, m_encoder.Get());

  Simulate100QuadratureTicks();
  EXPECT_EQ(100, m_encoder.Get());
}
