// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/EncoderSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>
#include <wpi/deprecated.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/Encoder.h"

namespace frc::sim {

namespace {
constexpr double kDefaultDistancePerPulse = .0005;
}  // namespace

TEST(EncoderSimTest, Initialize) {
  HAL_Initialize(500, 0);

  EncoderSim sim = EncoderSim::CreateForIndex(0);
  sim.ResetData();

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  Encoder encoder(0, 1);

  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(EncoderSimTest, Rate) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  sim.SetRate(1.91);
  EXPECT_EQ(1.91, sim.GetRate());
}

TEST(EncoderSimTest, Count) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  IntCallback callback;
  auto cb = sim.RegisterCountCallback(callback.GetCallback(), false);
  sim.SetCount(3504);
  EXPECT_EQ(3504, sim.GetCount());

  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, encoder.Get());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(EncoderSimTest, Distance) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  sim.SetDistance(229.174);
  EXPECT_EQ(229.174, sim.GetDistance());
  EXPECT_EQ(229.174, encoder.GetDistance());
}

TEST(EncoderSimTest, Period) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  DoubleCallback callback;
  auto cb = sim.RegisterPeriodCallback(callback.GetCallback(), false);
  sim.SetPeriod(123.456);
  EXPECT_EQ(123.456, sim.GetPeriod());
  WPI_IGNORE_DEPRECATED
  EXPECT_EQ(123.456, encoder.GetPeriod().value());
  WPI_UNIGNORE_DEPRECATED
  EXPECT_EQ(kDefaultDistancePerPulse / 123.456, encoder.GetRate());

  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(123.456, callback.GetLastValue());
}

TEST(EncoderSimTest, SetMaxPeriod) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  DoubleCallback callback;
  auto cb = sim.RegisterMaxPeriodCallback(callback.GetCallback(), false);

  WPI_IGNORE_DEPRECATED
  encoder.SetMaxPeriod(123.456_s);
  WPI_UNIGNORE_DEPRECATED
  EXPECT_EQ(123.456, sim.GetMaxPeriod());

  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(123.456, callback.GetLastValue());
}

TEST(EncoderSimTest, SetDirection) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  BooleanCallback callback;
  auto cb = sim.RegisterDirectionCallback(callback.GetCallback(), false);

  sim.SetDirection(true);
  EXPECT_TRUE(sim.GetDirection());
  EXPECT_TRUE(encoder.GetDirection());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  sim.SetDirection(false);
  EXPECT_FALSE(sim.GetDirection());
  EXPECT_FALSE(encoder.GetDirection());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(EncoderSimTest, SetReverseDirection) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  BooleanCallback callback;
  auto cb = sim.RegisterReverseDirectionCallback(callback.GetCallback(), false);

  encoder.SetReverseDirection(true);
  EXPECT_TRUE(sim.GetReverseDirection());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  encoder.SetReverseDirection(false);
  EXPECT_FALSE(sim.GetReverseDirection());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(EncoderSimTest, SetSamplesToAverage) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  IntCallback callback;
  auto cb = sim.RegisterSamplesToAverageCallback(callback.GetCallback(), false);

  encoder.SetSamplesToAverage(57);
  EXPECT_EQ(57, sim.GetSamplesToAverage());
  EXPECT_EQ(57, encoder.GetSamplesToAverage());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(57, callback.GetLastValue());
}

TEST(EncoderSimTest, SetDistancePerPulse) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  DoubleCallback callback;
  auto cb = sim.RegisterDistancePerPulseCallback(callback.GetCallback(), false);

  sim.SetDistancePerPulse(.03405);
  EXPECT_EQ(.03405, sim.GetDistancePerPulse());
  EXPECT_EQ(.03405, encoder.GetDistancePerPulse());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(.03405, callback.GetLastValue());
}

TEST(EncoderSimTest, Reset) {
  HAL_Initialize(500, 0);

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(kDefaultDistancePerPulse);

  BooleanCallback callback;
  auto cb = sim.RegisterResetCallback(callback.GetCallback(), false);

  sim.SetCount(3504);
  sim.SetDistance(229.191);

  encoder.Reset();
  EXPECT_TRUE(sim.GetReset());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  EXPECT_EQ(0, sim.GetCount());
  EXPECT_EQ(0, encoder.Get());
  EXPECT_EQ(0, sim.GetDistance());
  EXPECT_EQ(0, encoder.GetDistance());
}

}  // namespace frc::sim
