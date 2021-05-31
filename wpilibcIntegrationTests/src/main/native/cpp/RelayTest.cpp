// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Relay.h"  // NOLINT(build/include_order)

#include <units/time.h>

#include "TestBench.h"
#include "frc/DigitalInput.h"
#include "frc/Timer.h"
#include "gtest/gtest.h"

static constexpr auto kDelayTime = 10_ms;

TEST(RelayTest, BothDirections) {
  frc::Relay relay{TestBench::kRelayChannel};
  frc::DigitalInput forward{TestBench::kFakeRelayForward};
  frc::DigitalInput reverse{TestBench::kFakeRelayReverse};

  // Set the relay to forward
  relay.Set(frc::Relay::kForward);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(forward.Get()) << "Relay did not set forward";
  EXPECT_FALSE(reverse.Get()) << "Relay did not set forward";
  EXPECT_EQ(relay.Get(), frc::Relay::kForward);

  // Set the relay to reverse
  relay.Set(frc::Relay::kReverse);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(reverse.Get()) << "Relay did not set reverse";
  EXPECT_FALSE(forward.Get()) << "Relay did not set reverse";
  EXPECT_EQ(relay.Get(), frc::Relay::kReverse);

  // Set the relay to off
  relay.Set(frc::Relay::kOff);
  frc::Wait(kDelayTime);
  EXPECT_FALSE(forward.Get()) << "Relay did not set off";
  EXPECT_FALSE(reverse.Get()) << "Relay did not set off";
  EXPECT_EQ(relay.Get(), frc::Relay::kOff);

  // Set the relay to on
  relay.Set(frc::Relay::kOn);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(forward.Get()) << "Relay did not set on";
  EXPECT_TRUE(reverse.Get()) << "Relay did not set on";
  EXPECT_EQ(relay.Get(), frc::Relay::kOn);
}

TEST(RelayTest, ForwardOnly) {
  frc::Relay relay{TestBench::kRelayChannel, frc::Relay::kForwardOnly};
  frc::DigitalInput forward{TestBench::kFakeRelayForward};
  frc::DigitalInput reverse{TestBench::kFakeRelayReverse};

  relay.Set(frc::Relay::kOn);
  frc::Wait(kDelayTime);
  EXPECT_TRUE(forward.Get()) << "Relay did not set forward";
  EXPECT_FALSE(reverse.Get()) << "Relay did not set forward";
  EXPECT_EQ(relay.Get(), frc::Relay::kOn);
}

TEST(RelayTest, ReverseOnly) {
  frc::Relay relay{TestBench::kRelayChannel, frc::Relay::kReverseOnly};
  frc::DigitalInput forward{TestBench::kFakeRelayForward};
  frc::DigitalInput reverse{TestBench::kFakeRelayReverse};

  relay.Set(frc::Relay::kOn);
  frc::Wait(kDelayTime);
  EXPECT_FALSE(forward.Get()) << "Relay did not set reverse";
  EXPECT_TRUE(reverse.Get()) << "Relay did not set reverse";
  EXPECT_EQ(relay.Get(), frc::Relay::kOn);
}
