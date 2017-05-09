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

using namespace frc;

static const double kDelayTime = 0.01;

class RelayTest : public testing::Test {
 protected:
  Relay* m_relay;
  DigitalInput* m_forward;
  DigitalInput* m_reverse;

  void SetUp() override {
    m_relay = new Relay(TestBench::kRelayChannel);
    m_forward = new DigitalInput(TestBench::kFakeRelayForward);
    m_reverse = new DigitalInput(TestBench::kFakeRelayReverse);
  }

  void TearDown() override {
    delete m_relay;
    delete m_forward;
    delete m_reverse;
  }

  void Reset() { m_relay->Set(Relay::kOff); }
};
/**
 * Test the relay by setting it forward, reverse, off, and on.
 */
TEST_F(RelayTest, Relay) {
  Reset();

  // set the relay to forward
  m_relay->Set(Relay::kForward);
  Wait(kDelayTime);
  EXPECT_TRUE(m_forward->Get()) << "Relay did not set forward";
  EXPECT_FALSE(m_reverse->Get()) << "Relay did not set forward";
  EXPECT_EQ(m_relay->Get(), Relay::kForward);

  // set the relay to reverse
  m_relay->Set(Relay::kReverse);
  Wait(kDelayTime);
  EXPECT_TRUE(m_reverse->Get()) << "Relay did not set reverse";
  EXPECT_FALSE(m_forward->Get()) << "Relay did not set reverse";
  EXPECT_EQ(m_relay->Get(), Relay::kReverse);

  // set the relay to off
  m_relay->Set(Relay::kOff);
  Wait(kDelayTime);
  EXPECT_FALSE(m_forward->Get()) << "Relay did not set off";
  EXPECT_FALSE(m_reverse->Get()) << "Relay did not set off";
  EXPECT_EQ(m_relay->Get(), Relay::kOff);

  // set the relay to on
  m_relay->Set(Relay::kOn);
  Wait(kDelayTime);
  EXPECT_TRUE(m_forward->Get()) << "Relay did not set on";
  EXPECT_TRUE(m_reverse->Get()) << "Relay did not set on";
  EXPECT_EQ(m_relay->Get(), Relay::kOn);

  // test forward direction
  delete m_relay;
  m_relay = new Relay(TestBench::kRelayChannel, Relay::kForwardOnly);

  m_relay->Set(Relay::kOn);
  Wait(kDelayTime);
  EXPECT_TRUE(m_forward->Get()) << "Relay did not set forward";
  EXPECT_FALSE(m_reverse->Get()) << "Relay did not set forward";
  EXPECT_EQ(m_relay->Get(), Relay::kOn);

  // test reverse direction
  delete m_relay;
  m_relay = new Relay(TestBench::kRelayChannel, Relay::kReverseOnly);

  m_relay->Set(Relay::kOn);
  Wait(kDelayTime);
  EXPECT_FALSE(m_forward->Get()) << "Relay did not set reverse";
  EXPECT_TRUE(m_reverse->Get()) << "Relay did not set reverse";
  EXPECT_EQ(m_relay->Get(), Relay::kOn);
}
