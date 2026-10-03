// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/GenericHID.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/simulation/DriverStationSim.hpp"
#include "wpi/simulation/GenericHIDSim.hpp"

using namespace wpi;
using RumbleType = GenericHID::RumbleType;
static constexpr double EPSILON = 0.0001;
TEST_CASE("GenericHIDTest RumbleRange", "[wpilibc]") {
  GenericHID hid = internal::DriverStationBackend::ConstructGenericHID(0);
  sim::GenericHIDSim sim{0};

  for (int i = 0; i <= 100; i++) {
    double rumbleValue = i / 100.0;
    hid.SetRumble(RumbleType::LEFT_RUMBLE, rumbleValue);
    CHECK_THAT(rumbleValue,
               Catch::Matchers::WithinAbs(
                   sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON));

    hid.SetRumble(RumbleType::RIGHT_RUMBLE, rumbleValue);
    CHECK_THAT(rumbleValue,
               Catch::Matchers::WithinAbs(
                   sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON));

    hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, rumbleValue);
    CHECK_THAT(rumbleValue,
               Catch::Matchers::WithinAbs(
                   sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), EPSILON));

    hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, rumbleValue);
    CHECK_THAT(rumbleValue,
               Catch::Matchers::WithinAbs(
                   sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), EPSILON));
  }
}

TEST_CASE("GenericHIDTest RawRumbleRange", "[wpilibc]") {
  GenericHID hid = internal::DriverStationBackend::ConstructGenericHID(0);

  hid.SetRawRumble(RumbleType::LEFT_RUMBLE, 0x1234);
  CHECK(sim::DriverStationSim::GetJoystickRumble(0, 0) == 0x1234);

  hid.SetRawRumble(RumbleType::RIGHT_RUMBLE, 0x5678);
  CHECK(sim::DriverStationSim::GetJoystickRumble(0, 1) == 0x5678);

  hid.SetRawRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 0x9ABC);
  CHECK(sim::DriverStationSim::GetJoystickRumble(0, 2) == 0x9ABC);

  hid.SetRawRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 0xDEF0);
  CHECK(sim::DriverStationSim::GetJoystickRumble(0, 3) == 0xDEF0);

  hid.SetRawRumble(RumbleType::LEFT_RUMBLE, -1);
  CHECK(sim::DriverStationSim::GetJoystickRumble(0, 0) == 0);

  hid.SetRawRumble(RumbleType::LEFT_RUMBLE, 65536);
  CHECK(sim::DriverStationSim::GetJoystickRumble(0, 0) == 65535);
}

TEST_CASE("GenericHIDTest RumbleTypes", "[wpilibc]") {
  GenericHID hid = internal::DriverStationBackend::ConstructGenericHID(0);
  sim::GenericHIDSim sim{0};

  // Make sure all are off
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 0);
  hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 0);
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 0);
  hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 0);
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), EPSILON));

  // test left only
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 1);
  CHECK_THAT(1, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), EPSILON));
  hid.SetRumble(RumbleType::LEFT_RUMBLE, 0);

  // test right only
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 1);
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON));
  CHECK_THAT(1, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), EPSILON));
  hid.SetRumble(RumbleType::RIGHT_RUMBLE, 0);

  // test left trigger only
  hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 1);
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON));
  CHECK_THAT(1, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), EPSILON));
  hid.SetRumble(RumbleType::LEFT_TRIGGER_RUMBLE, 0);

  // test right trigger only
  hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 1);
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_RUMBLE), EPSILON));
  CHECK_THAT(0, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::LEFT_TRIGGER_RUMBLE), EPSILON));
  CHECK_THAT(1, Catch::Matchers::WithinAbs(
                    sim.GetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE), EPSILON));
  hid.SetRumble(RumbleType::RIGHT_TRIGGER_RUMBLE, 0);
}
