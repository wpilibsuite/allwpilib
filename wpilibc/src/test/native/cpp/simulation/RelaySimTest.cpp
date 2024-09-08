// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/RelaySim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/Relay.h"

namespace frc::sim {

TEST(RelaySimTest, InitializationBidirectional) {
  HAL_Initialize(500, 0);

  RelaySim sim(0);
  sim.ResetData();

  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  EXPECT_FALSE(sim.GetInitializedForward());
  EXPECT_FALSE(sim.GetInitializedReverse());

  auto fwdCb = sim.RegisterInitializedForwardCallback(
      forwardCallback.GetCallback(), false);
  auto revCb = sim.RegisterInitializedReverseCallback(
      reverseCallback.GetCallback(), false);
  Relay relay{0};

  EXPECT_TRUE(sim.GetInitializedForward());
  EXPECT_TRUE(sim.GetInitializedReverse());

  EXPECT_TRUE(forwardCallback.WasTriggered());
  EXPECT_TRUE(forwardCallback.GetLastValue());
  EXPECT_TRUE(reverseCallback.WasTriggered());
  EXPECT_TRUE(reverseCallback.GetLastValue());
}

TEST(RelaySimTest, InitializationForwardOnly) {
  HAL_Initialize(500, 0);

  RelaySim sim{0};
  sim.ResetData();

  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  EXPECT_FALSE(sim.GetInitializedForward());
  EXPECT_FALSE(sim.GetInitializedReverse());

  auto fwdCb = sim.RegisterInitializedForwardCallback(
      forwardCallback.GetCallback(), false);
  auto revCb = sim.RegisterInitializedReverseCallback(
      reverseCallback.GetCallback(), false);
  Relay relay(0, Relay::kForwardOnly);

  EXPECT_TRUE(sim.GetInitializedForward());
  EXPECT_FALSE(sim.GetInitializedReverse());

  EXPECT_TRUE(forwardCallback.WasTriggered());
  EXPECT_TRUE(forwardCallback.GetLastValue());
  EXPECT_FALSE(reverseCallback.WasTriggered());
}

TEST(RelaySimTest, InitializationReverseOnly) {
  HAL_Initialize(500, 0);

  RelaySim sim{0};
  sim.ResetData();

  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  EXPECT_FALSE(sim.GetInitializedForward());
  EXPECT_FALSE(sim.GetInitializedReverse());

  auto fwdCb = sim.RegisterInitializedForwardCallback(
      forwardCallback.GetCallback(), false);
  auto revCb = sim.RegisterInitializedReverseCallback(
      reverseCallback.GetCallback(), false);
  Relay relay(0, Relay::kReverseOnly);

  EXPECT_FALSE(sim.GetInitializedForward());
  EXPECT_TRUE(sim.GetInitializedReverse());

  EXPECT_FALSE(forwardCallback.WasTriggered());
  EXPECT_TRUE(reverseCallback.WasTriggered());
  EXPECT_TRUE(reverseCallback.GetLastValue());
}

TEST(RelaySimTest, BidirectionalSetForward) {
  HAL_Initialize(500, 0);

  RelaySim sim{0};
  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  Relay relay{0};
  auto fwdCb =
      sim.RegisterForwardCallback(forwardCallback.GetCallback(), false);
  auto revCb =
      sim.RegisterReverseCallback(reverseCallback.GetCallback(), false);

  relay.Set(Relay::kForward);
  EXPECT_EQ(Relay::kForward, relay.Get());
  EXPECT_TRUE(sim.GetForward());
  EXPECT_FALSE(sim.GetReverse());

  EXPECT_TRUE(forwardCallback.WasTriggered());
  EXPECT_TRUE(forwardCallback.GetLastValue());
  EXPECT_FALSE(reverseCallback.WasTriggered());
}

TEST(RelaySimTest, BidirectionalSetReverse) {
  HAL_Initialize(500, 0);

  RelaySim sim{0};
  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  Relay relay{0};
  auto fwdCb =
      sim.RegisterForwardCallback(forwardCallback.GetCallback(), false);
  auto revCb =
      sim.RegisterReverseCallback(reverseCallback.GetCallback(), false);

  relay.Set(Relay::kReverse);
  EXPECT_EQ(Relay::kReverse, relay.Get());
  EXPECT_FALSE(sim.GetForward());
  EXPECT_TRUE(sim.GetReverse());

  EXPECT_FALSE(forwardCallback.WasTriggered());
  EXPECT_TRUE(reverseCallback.WasTriggered());
  EXPECT_TRUE(reverseCallback.GetLastValue());
}

TEST(RelaySimTest, BidirectionalSetOn) {
  HAL_Initialize(500, 0);

  RelaySim sim{0};
  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  Relay relay{0};
  auto fwdCb =
      sim.RegisterForwardCallback(forwardCallback.GetCallback(), false);
  auto revCb =
      sim.RegisterReverseCallback(reverseCallback.GetCallback(), false);

  relay.Set(Relay::kOn);
  EXPECT_EQ(Relay::kOn, relay.Get());
  EXPECT_TRUE(sim.GetForward());
  EXPECT_TRUE(sim.GetReverse());

  EXPECT_TRUE(forwardCallback.WasTriggered());
  EXPECT_TRUE(forwardCallback.GetLastValue());
  EXPECT_TRUE(reverseCallback.WasTriggered());
  EXPECT_TRUE(reverseCallback.GetLastValue());
}

TEST(RelaySimTest, BidirectionalSetOff) {
  HAL_Initialize(500, 0);

  RelaySim sim{0};
  BooleanCallback forwardCallback;
  BooleanCallback reverseCallback;

  Relay relay{0};
  auto fwdCb =
      sim.RegisterForwardCallback(forwardCallback.GetCallback(), false);
  auto revCb =
      sim.RegisterReverseCallback(reverseCallback.GetCallback(), false);

  // Bootstrap into a non-off state to verify the callbacks
  relay.Set(Relay::kOn);
  forwardCallback.Reset();
  reverseCallback.Reset();

  relay.Set(Relay::kOff);
  EXPECT_EQ(Relay::kOff, relay.Get());
  EXPECT_FALSE(sim.GetForward());
  EXPECT_FALSE(sim.GetReverse());

  EXPECT_TRUE(forwardCallback.WasTriggered());
  EXPECT_FALSE(forwardCallback.GetLastValue());
  EXPECT_TRUE(reverseCallback.WasTriggered());
  EXPECT_FALSE(reverseCallback.GetLastValue());
}

TEST(RelaySimTest, StopMotor) {
  Relay relay{0};
  RelaySim sim(relay);

  // Bootstrap into non-off state
  relay.Set(Relay::kOn);

  relay.StopMotor();
  EXPECT_EQ(Relay::kOff, relay.Get());
}

}  // namespace frc::sim
