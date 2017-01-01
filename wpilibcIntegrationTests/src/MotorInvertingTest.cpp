/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "Jaguar.h"
#include "Talon.h"
#include "TestBench.h"
#include "Timer.h"
#include "Victor.h"
#include "gtest/gtest.h"

using namespace frc;

enum MotorInvertingTestType { TEST_VICTOR, TEST_JAGUAR, TEST_TALON };
static const double motorSpeed = 0.15;
static const double delayTime = 0.5;
std::ostream& operator<<(std::ostream& os, MotorInvertingTestType const& type) {
  switch (type) {
    case TEST_VICTOR:
      os << "Victor";
      break;
    case TEST_JAGUAR:
      os << "Jaguar";
      break;
    case TEST_TALON:
      os << "Talon";
      break;
  }

  return os;
}
class MotorInvertingTest
    : public testing::TestWithParam<MotorInvertingTestType> {
 protected:
  SpeedController* m_speedController;
  Encoder* m_encoder;
  void SetUp() override {
    switch (GetParam()) {
      case TEST_VICTOR:
        m_speedController = new Victor(TestBench::kVictorChannel);
        m_encoder = new Encoder(TestBench::kVictorEncoderChannelA,
                                TestBench::kVictorEncoderChannelB);
        break;

      case TEST_JAGUAR:
        m_speedController = new Jaguar(TestBench::kJaguarChannel);
        m_encoder = new Encoder(TestBench::kJaguarEncoderChannelA,
                                TestBench::kJaguarEncoderChannelB);
        break;

      case TEST_TALON:
        m_speedController = new Talon(TestBench::kTalonChannel);
        m_encoder = new Encoder(TestBench::kTalonEncoderChannelA,
                                TestBench::kTalonEncoderChannelB);
        break;
    }
  }
  void TearDown() override {
    delete m_speedController;
    delete m_encoder;
  }

  void Reset() {
    m_speedController->SetInverted(false);
    m_speedController->Set(0.0);
    m_encoder->Reset();
  }
};

TEST_P(MotorInvertingTest, InvertingPositive) {
  Reset();
  m_speedController->Set(motorSpeed);
  Wait(delayTime);
  bool initDirection = m_encoder->GetDirection();
  m_speedController->SetInverted(true);
  m_speedController->Set(motorSpeed);
  Wait(delayTime);
  EXPECT_TRUE(m_encoder->GetDirection() != initDirection)
      << "Inverting with Positive value does not change direction";
  Reset();
}
TEST_P(MotorInvertingTest, InvertingNegative) {
  Reset();
  m_speedController->SetInverted(false);
  m_speedController->Set(-motorSpeed);
  Wait(delayTime);
  bool initDirection = m_encoder->GetDirection();
  m_speedController->SetInverted(true);
  m_speedController->Set(-motorSpeed);
  Wait(delayTime);
  EXPECT_TRUE(m_encoder->GetDirection() != initDirection)
      << "Inverting with Negative value does not change direction";
  Reset();
}
TEST_P(MotorInvertingTest, InvertingSwitchingPosToNeg) {
  Reset();
  m_speedController->SetInverted(false);
  m_speedController->Set(motorSpeed);
  Wait(delayTime);
  bool initDirection = m_encoder->GetDirection();
  m_speedController->SetInverted(true);
  m_speedController->Set(-motorSpeed);
  Wait(delayTime);
  EXPECT_TRUE(m_encoder->GetDirection() == initDirection)
      << "Inverting with Switching value does change direction";
  Reset();
}
TEST_P(MotorInvertingTest, InvertingSwitchingNegToPos) {
  Reset();
  m_speedController->SetInverted(false);
  m_speedController->Set(-motorSpeed);
  Wait(delayTime);
  bool initDirection = m_encoder->GetDirection();
  m_speedController->SetInverted(true);
  m_speedController->Set(motorSpeed);
  Wait(delayTime);
  EXPECT_TRUE(m_encoder->GetDirection() == initDirection)
      << "Inverting with Switching value does change direction";
  Reset();
}
INSTANTIATE_TEST_CASE_P(Test, MotorInvertingTest,
                        testing::Values(TEST_VICTOR, TEST_JAGUAR, TEST_TALON));
