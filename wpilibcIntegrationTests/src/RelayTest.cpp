/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Relay.h"  // NOLINT(build/include_order)

#include "DigitalInput.h"
#include "TestBench.h"
#include "Timer.h"
#include "gtest/gtest.h"

static const double kDelayTime = 0.01;

class RelayTest : public testing::Test {
 protected:
  frc::Relay* m_relay;
  frc::DigitalInput* m_forward;
  frc::DigitalInput* m_reverse;

  void SetUp() override {
    m_relay = new frc::Relay(TestBench::kRelayChannel);
    m_forward = new frc::DigitalInput(TestBench::kFakeRelayForward);
    m_reverse = new frc::DigitalInput(TestBench::kFakeRelayReverse);
  }

  void TearDown() override {
    delete m_relay;
    delete m_forward;
    delete m_reverse;
  }

  void Reset() { m_relay->Set(frc::Relay::kOff); }
};

/**
 * Test the relay by setting it forward, reverse, off, and on.
 */
TEST_F(RelayTest, Relay) {
  Reset();

  // set the relay to forward
  m_relay->Set(frc::Relay::kForward);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(m_forward->Get()) << "Relay did not set forward";
  EXPECT_FALSE(m_reverse->Get()) << "Relay did not set forward";
  EXPECT_EQ(m_relay->Get(), frc::Relay::kForward);

  // set the relay to reverse
  m_relay->Set(frc::Relay::kReverse);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(m_reverse->Get()) << "Relay did not set reverse";
  EXPECT_FALSE(m_forward->Get()) << "Relay did not set reverse";
  EXPECT_EQ(m_relay->Get(), frc::Relay::kReverse);

  // set the relay to off
  m_relay->Set(frc::Relay::kOff);
  frc::Wait(kDelayTime);
  EXPECT_FALSE(m_forward->Get()) << "Relay did not set off";
  EXPECT_FALSE(m_reverse->Get()) << "Relay did not set off";
  EXPECT_EQ(m_relay->Get(), frc::Relay::kOff);

  // set the relay to on
  m_relay->Set(frc::Relay::kOn);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(m_forward->Get()) << "Relay did not set on";
  EXPECT_TRUE(m_reverse->Get()) << "Relay did not set on";
  EXPECT_EQ(m_relay->Get(), frc::Relay::kOn);

  // test forward direction
  delete m_relay;
  m_relay = new frc::Relay(TestBench::kRelayChannel, frc::Relay::kForwardOnly);

  m_relay->Set(frc::Relay::kOn);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(m_forward->Get()) << "Relay did not set forward";
  EXPECT_FALSE(m_reverse->Get()) << "Relay did not set forward";
  EXPECT_EQ(m_relay->Get(), frc::Relay::kOn);

  // test reverse direction
  delete m_relay;
  m_relay = new frc::Relay(TestBench::kRelayChannel, frc::Relay::kReverseOnly);

  m_relay->Set(frc::Relay::kOn);
  frc::Wait(kDelayTime);
  EXPECT_FALSE(m_forward->Get()) << "Relay did not set reverse";
  EXPECT_TRUE(m_reverse->Get()) << "Relay did not set reverse";
  EXPECT_EQ(m_relay->Get(), frc::Relay::kOn);
}
