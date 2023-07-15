// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <units/time.h>

#include "TestBench.h"
#include "frc/Encoder.h"
#include "frc/Timer.h"
#include "frc/motorcontrol/Jaguar.h"
#include "frc/motorcontrol/Talon.h"
#include "frc/motorcontrol/Victor.h"
#include "gtest/gtest.h"

enum MotorInvertingTestType { TEST_VICTOR, TEST_JAGUAR, TEST_TALON };

static constexpr double kMotorSpeed = 0.15;
static constexpr auto kDelayTime = 0.5_s;

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
  frc::MotorController* m_motorController;
  frc::Encoder* m_encoder;

  MotorInvertingTest() {
    switch (GetParam()) {
      case TEST_VICTOR:
        m_motorController = new frc::Victor(TestBench::kVictorChannel);
        m_encoder = new frc::Encoder(TestBench::kVictorEncoderChannelA,
                                     TestBench::kVictorEncoderChannelB);
        break;

      case TEST_JAGUAR:
        m_motorController = new frc::Jaguar(TestBench::kJaguarChannel);
        m_encoder = new frc::Encoder(TestBench::kJaguarEncoderChannelA,
                                     TestBench::kJaguarEncoderChannelB);
        break;

      case TEST_TALON:
        m_motorController = new frc::Talon(TestBench::kTalonChannel);
        m_encoder = new frc::Encoder(TestBench::kTalonEncoderChannelA,
                                     TestBench::kTalonEncoderChannelB);
        break;
    }
  }

  ~MotorInvertingTest() {
    delete m_encoder;
    delete m_motorController;
  }

  void Reset() {
    m_motorController->SetInverted(false);
    m_motorController->Set(0.0);
    m_encoder->Reset();
  }
};

TEST_P(MotorInvertingTest, InvertingPositive) {
  Reset();

  m_motorController->Set(kMotorSpeed);

  frc::Wait(kDelayTime);

  bool initDirection = m_encoder->GetDirection();
  m_motorController->SetInverted(true);
  m_motorController->Set(kMotorSpeed);

  frc::Wait(kDelayTime);

  EXPECT_TRUE(m_encoder->GetDirection() != initDirection)
      << "Inverting with Positive value does not change direction";

  Reset();
}

TEST_P(MotorInvertingTest, InvertingNegative) {
  Reset();

  m_motorController->SetInverted(false);
  m_motorController->Set(-kMotorSpeed);

  frc::Wait(kDelayTime);

  bool initDirection = m_encoder->GetDirection();
  m_motorController->SetInverted(true);
  m_motorController->Set(-kMotorSpeed);

  frc::Wait(kDelayTime);

  EXPECT_TRUE(m_encoder->GetDirection() != initDirection)
      << "Inverting with Negative value does not change direction";

  Reset();
}

TEST_P(MotorInvertingTest, InvertingSwitchingPosToNeg) {
  Reset();

  m_motorController->SetInverted(false);
  m_motorController->Set(kMotorSpeed);

  frc::Wait(kDelayTime);

  bool initDirection = m_encoder->GetDirection();
  m_motorController->SetInverted(true);
  m_motorController->Set(-kMotorSpeed);

  frc::Wait(kDelayTime);

  EXPECT_TRUE(m_encoder->GetDirection() == initDirection)
      << "Inverting with Switching value does change direction";

  Reset();
}

TEST_P(MotorInvertingTest, InvertingSwitchingNegToPos) {
  Reset();

  m_motorController->SetInverted(false);
  m_motorController->Set(-kMotorSpeed);

  frc::Wait(kDelayTime);

  bool initDirection = m_encoder->GetDirection();
  m_motorController->SetInverted(true);
  m_motorController->Set(kMotorSpeed);

  frc::Wait(kDelayTime);

  EXPECT_TRUE(m_encoder->GetDirection() == initDirection)
      << "Inverting with Switching value does change direction";

  Reset();
}

INSTANTIATE_TEST_SUITE_P(Tests, MotorInvertingTest,
                         testing::Values(TEST_VICTOR, TEST_JAGUAR, TEST_TALON));
