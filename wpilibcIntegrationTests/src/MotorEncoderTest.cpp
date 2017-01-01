/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Encoder.h"
#include "Jaguar.h"
#include "PIDController.h"
#include "Talon.h"
#include "TestBench.h"
#include "Timer.h"
#include "Victor.h"
#include "gtest/gtest.h"

using namespace frc;

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

static constexpr double kMotorTime = 0.5;

/**
 * A fixture that includes a PWM speed controller and an encoder connected to
 * the same motor.
 */
class MotorEncoderTest : public testing::TestWithParam<MotorEncoderTestType> {
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
    m_speedController->Set(0.0);
    m_encoder->Reset();
  }
};

/**
 * Test if the encoder value increments after the motor drives forward
 */
TEST_P(MotorEncoderTest, Increment) {
  Reset();

  /* Drive the speed controller briefly to move the encoder */
  m_speedController->Set(0.2f);
  Wait(kMotorTime);
  m_speedController->Set(0.0);

  /* The encoder should be positive now */
  EXPECT_GT(m_encoder->Get(), 0)
      << "Encoder should have incremented after the motor moved";
}

/**
 * Test if the encoder value decrements after the motor drives backwards
 */
TEST_P(MotorEncoderTest, Decrement) {
  Reset();

  /* Drive the speed controller briefly to move the encoder */
  m_speedController->Set(-0.2);
  Wait(kMotorTime);
  m_speedController->Set(0.0);

  /* The encoder should be positive now */
  EXPECT_LT(m_encoder->Get(), 0.0)
      << "Encoder should have decremented after the motor moved";
}

/**
 * Test if motor speeds are clamped to [-1,1]
 */
TEST_P(MotorEncoderTest, ClampSpeed) {
  Reset();

  m_speedController->Set(2.0);
  Wait(kMotorTime);

  EXPECT_FLOAT_EQ(1.0, m_speedController->Get());

  m_speedController->Set(-2.0);
  Wait(kMotorTime);

  EXPECT_FLOAT_EQ(-1.0, m_speedController->Get());
}

/**
 * Test if position PID loop works
 */
TEST_P(MotorEncoderTest, PositionPIDController) {
  Reset();
  double goal = 1000;
  m_encoder->SetPIDSourceType(PIDSourceType::kDisplacement);
  PIDController pid(0.001, 0.0005, 0.0, m_encoder, m_speedController);
  pid.SetAbsoluteTolerance(50.0);
  pid.SetOutputRange(-0.2, 0.2);
  pid.SetSetpoint(goal);

  /* 10 seconds should be plenty time to get to the setpoint */
  pid.Enable();
  Wait(10.0);
  pid.Disable();

  RecordProperty("PIDError", pid.GetError());

  EXPECT_TRUE(pid.OnTarget())
      << "PID loop did not converge within 10 seconds. Goal was: " << goal
      << " Error was: " << pid.GetError();
}

/**
 * Test if velocity PID loop works
 */
TEST_P(MotorEncoderTest, VelocityPIDController) {
  Reset();

  m_encoder->SetPIDSourceType(PIDSourceType::kRate);
  PIDController pid(1e-5, 0.0, 3e-5, 8e-5, m_encoder, m_speedController);
  pid.SetAbsoluteTolerance(200.0);
  pid.SetToleranceBuffer(50);
  pid.SetOutputRange(-0.3, 0.3);
  pid.SetSetpoint(600);

  /* 10 seconds should be plenty time to get to the setpoint */
  pid.Enable();
  Wait(10.0);
  pid.Disable();
  RecordProperty("PIDError", pid.GetError());

  EXPECT_TRUE(pid.OnTarget())
      << "PID loop did not converge within 10 seconds. Goal was: " << 600
      << " Error was: " << pid.GetError();
}

/**
 * Test resetting encoders
 */
TEST_P(MotorEncoderTest, Reset) {
  Reset();

  EXPECT_EQ(0, m_encoder->Get()) << "Encoder did not reset to 0";
}

INSTANTIATE_TEST_CASE_P(Test, MotorEncoderTest,
                        testing::Values(TEST_VICTOR, TEST_JAGUAR, TEST_TALON));
