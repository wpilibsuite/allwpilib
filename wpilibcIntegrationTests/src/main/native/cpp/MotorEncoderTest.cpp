// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>

#include <units/time.h>

#include "TestBench.h"
#include "frc/Encoder.h"
#include "frc/Notifier.h"
#include "frc/Timer.h"
#include "frc/controller/PIDController.h"
#include "frc/filter/LinearFilter.h"
#include "frc/motorcontrol/Jaguar.h"
#include "frc/motorcontrol/Talon.h"
#include "frc/motorcontrol/Victor.h"
#include "gtest/gtest.h"

enum MotorEncoderTestType { TEST_VICTOR, TEST_JAGUAR, TEST_TALON };

std::ostream& operator<<(std::ostream& os, MotorEncoderTestType const& type) {
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

static constexpr auto kMotorTime = 0.5_s;

/**
 * A fixture that includes a PWM motor controller and an encoder connected to
 * the same motor.
 */
class MotorEncoderTest : public testing::TestWithParam<MotorEncoderTestType> {
 protected:
  frc::MotorController* m_motorController;
  frc::Encoder* m_encoder;
  frc::LinearFilter<double>* m_filter;

  MotorEncoderTest() {
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
    m_filter = new frc::LinearFilter<double>(
        frc::LinearFilter<double>::MovingAverage(50));
  }

  ~MotorEncoderTest() {
    delete m_filter;
    delete m_encoder;
    delete m_motorController;
  }

  void Reset() {
    m_motorController->Set(0.0);
    m_encoder->Reset();
    m_filter->Reset();
  }
};

/**
 * Test if the encoder value increments after the motor drives forward
 */
TEST_P(MotorEncoderTest, Increment) {
  Reset();

  /* Drive the motor controller briefly to move the encoder */
  m_motorController->Set(0.2f);
  frc::Wait(kMotorTime);
  m_motorController->Set(0.0);

  /* The encoder should be positive now */
  EXPECT_GT(m_encoder->Get(), 0)
      << "Encoder should have incremented after the motor moved";
}

/**
 * Test if the encoder value decrements after the motor drives backwards
 */
TEST_P(MotorEncoderTest, Decrement) {
  Reset();

  /* Drive the motor controller briefly to move the encoder */
  m_motorController->Set(-0.2);
  frc::Wait(kMotorTime);
  m_motorController->Set(0.0);

  /* The encoder should be positive now */
  EXPECT_LT(m_encoder->Get(), 0.0)
      << "Encoder should have decremented after the motor moved";
}

/**
 * Test if motor speeds are clamped to [-1,1]
 */
TEST_P(MotorEncoderTest, ClampSpeed) {
  Reset();

  m_motorController->Set(2.0);
  frc::Wait(kMotorTime);

  EXPECT_FLOAT_EQ(1.0, m_motorController->Get());

  m_motorController->Set(-2.0);
  frc::Wait(kMotorTime);

  EXPECT_FLOAT_EQ(-1.0, m_motorController->Get());
}

/**
 * Test if position PID loop works
 */
TEST_P(MotorEncoderTest, PositionPIDController) {
  Reset();
  double goal = 1000;
  frc2::PIDController pidController(0.001, 0.01, 0.0);
  pidController.SetTolerance(50.0);
  pidController.SetIntegratorRange(-0.2, 0.2);
  pidController.SetSetpoint(goal);

  /* 10 seconds should be plenty time to get to the reference */
  frc::Notifier pidRunner{[this, &pidController] {
    auto speed = pidController.Calculate(m_encoder->GetDistance());
    m_motorController->Set(std::clamp(speed, -0.2, 0.2));
  }};
  pidRunner.StartPeriodic(pidController.GetPeriod());
  frc::Wait(10_s);
  pidRunner.Stop();

  RecordProperty("PIDError", pidController.GetPositionError());

  EXPECT_TRUE(pidController.AtSetpoint())
      << "PID loop did not converge within 10 seconds. Goal was: " << goal
      << " Error was: " << pidController.GetPositionError();
}

/**
 * Test if velocity PID loop works
 */
TEST_P(MotorEncoderTest, VelocityPIDController) {
  Reset();

  frc2::PIDController pidController(1e-5, 0.0, 0.0006);
  pidController.SetTolerance(200.0);
  pidController.SetSetpoint(600);

  /* 10 seconds should be plenty time to get to the reference */
  frc::Notifier pidRunner{[this, &pidController] {
    auto speed =
        pidController.Calculate(m_filter->Calculate(m_encoder->GetRate()));
    m_motorController->Set(std::clamp(speed, -0.3, 0.3));
  }};
  pidRunner.StartPeriodic(pidController.GetPeriod());
  frc::Wait(10_s);
  pidRunner.Stop();
  RecordProperty("PIDError", pidController.GetPositionError());

  EXPECT_TRUE(pidController.AtSetpoint())
      << "PID loop did not converge within 10 seconds. Goal was: " << 600
      << " Error was: " << pidController.GetPositionError();
}

/**
 * Test resetting encoders
 */
TEST_P(MotorEncoderTest, Reset) {
  Reset();

  EXPECT_EQ(0, m_encoder->Get()) << "Encoder did not reset to 0";
}

INSTANTIATE_TEST_SUITE_P(Tests, MotorEncoderTest,
                         testing::Values(TEST_VICTOR, TEST_JAGUAR, TEST_TALON));
