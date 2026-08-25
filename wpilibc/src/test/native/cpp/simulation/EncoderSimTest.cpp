// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/EncoderSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/deprecated.hpp"

namespace wpi::sim {

namespace {
constexpr double DEFAULT_DISTANCE_PER_PULSE = .0005;
}  // namespace

TEST_CASE("EncoderSimTest Initialize", "[wpilibc][simulation]") {
  HAL_Initialize();

  EncoderSim sim = EncoderSim::CreateForIndex(0);
  sim.ResetData();

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  Encoder encoder(0, 1);

  CHECK(sim.GetInitialized());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("EncoderSimTest Rate", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

  DoubleCallback callback;
  auto cb = sim.RegisterRateCallback(callback.GetCallback(), false);
  sim.SetRate(1.91);
  CHECK(1.91 == sim.GetRate());
  CHECK(callback.WasTriggered());
  CHECK(1.91 == callback.GetLastValue());
}

TEST_CASE("EncoderSimTest SetRateWindow", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);

  CHECK_NOTHROW(encoder.SetRateWindow(wpi::units::milliseconds<>{5}));
  CHECK_NOTHROW(encoder.SetRateWindow(wpi::units::milliseconds<>{255}));
  CHECK_THROWS(encoder.SetRateWindow(wpi::units::milliseconds<>{4}));
  CHECK_THROWS(encoder.SetRateWindow(wpi::units::milliseconds<>{256}));
}

TEST_CASE("EncoderSimTest ResetDataClearsRateCallbacks",
          "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  DoubleCallback callback;
  auto cb = sim.RegisterRateCallback(callback.GetCallback(), false);
  sim.SetRate(1.91);
  REQUIRE(callback.WasTriggered());

  callback.Reset();
  sim.ResetData();
  sim.SetRate(2.53);
  CHECK_FALSE(callback.WasTriggered());
}

TEST_CASE("EncoderSimTest Count", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

  IntCallback callback;
  auto cb = sim.RegisterCountCallback(callback.GetCallback(), false);
  sim.SetCount(3504);
  CHECK(3504 == sim.GetCount());

  CHECK(callback.WasTriggered());
  CHECK(3504 == encoder.Get());
  CHECK(3504 == callback.GetLastValue());
}

TEST_CASE("EncoderSimTest Distance", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

  sim.SetDistance(229.174);
  CHECK(229.174 == sim.GetDistance());
  CHECK(229.174 == encoder.GetDistance());
}

TEST_CASE("EncoderSimTest SetDirection", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

  BooleanCallback callback;
  auto cb = sim.RegisterDirectionCallback(callback.GetCallback(), false);

  sim.SetDirection(true);
  CHECK(sim.GetDirection());
  CHECK(encoder.GetDirection());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());

  sim.SetDirection(false);
  CHECK_FALSE(sim.GetDirection());
  CHECK_FALSE(encoder.GetDirection());
  CHECK(callback.WasTriggered());
  CHECK_FALSE(callback.GetLastValue());
}

TEST_CASE("EncoderSimTest SetReverseDirection", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

  BooleanCallback callback;
  auto cb = sim.RegisterReverseDirectionCallback(callback.GetCallback(), false);

  encoder.SetReverseDirection(true);
  CHECK(sim.GetReverseDirection());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());

  encoder.SetReverseDirection(false);
  CHECK_FALSE(sim.GetReverseDirection());
  CHECK(callback.WasTriggered());
  CHECK_FALSE(callback.GetLastValue());
}

TEST_CASE("EncoderSimTest SetDistancePerPulse", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  DoubleCallback callback;
  auto cb = sim.RegisterDistancePerPulseCallback(callback.GetCallback(), false);

  sim.SetDistancePerPulse(.03405);
  CHECK(.03405 == sim.GetDistancePerPulse());
  CHECK(.03405 == encoder.GetDistancePerPulse());
  CHECK(callback.WasTriggered());
  CHECK(.03405 == callback.GetLastValue());
}

TEST_CASE("EncoderSimTest Reset", "[wpilibc][simulation]") {
  HAL_Initialize();

  Encoder encoder(0, 1);
  EncoderSim sim(encoder);
  sim.ResetData();

  encoder.SetDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

  BooleanCallback callback;
  auto cb = sim.RegisterResetCallback(callback.GetCallback(), false);

  sim.SetCount(3504);
  sim.SetDistance(229.191);
  sim.SetDirection(true);

  encoder.Reset();
  CHECK(sim.GetReset());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());

  CHECK(0 == sim.GetCount());
  CHECK(0 == encoder.Get());
  CHECK(0 == sim.GetDistance());
  CHECK(0 == encoder.GetDistance());
  CHECK(sim.GetDirection());
  CHECK(encoder.GetDirection());
}

}  // namespace wpi::sim
