// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/PIDController.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"

static constexpr double SETPOINT = 50.0;
static constexpr double RANGE = 200;
static constexpr double TOLERANCE = 10.0;

TEST_CASE("PIDToleranceTest InitialTolerance", "[wpimath]") {
  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-RANGE / 2, RANGE / 2);

  CHECK_FALSE(controller.AtSetpoint());
}

TEST_CASE("PIDToleranceTest TunedSetpointUpdatesSetpointState", "[wpimath]") {
  wpi::tunables::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);

  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  wpi::tunables::Publish("pid", controller);

  CHECK_FALSE(controller.AtSetpoint());

  auto setpointUid = backend->GetUid("/pid/setpoint");
  REQUIRE(setpointUid.has_value());
  auto setpointInfo = wpi::tunables::TunableRegistry::GetTunable(*setpointUid);
  REQUIRE(static_cast<bool>(setpointInfo));
  REQUIRE(setpointInfo.config != nullptr);
  CHECK(setpointInfo.config->polling ==
        wpi::tunables::TunableConfig::Polling::ALWAYS_GET);

  controller.SetSetpoint(1.0);
  CHECK_FALSE(
      wpi::tunables::TunableRegistry::GetTunable(*setpointUid).IsChanged());
  wpi::tunables::TunableRegistry::Update();

  backend->SetDouble("/pid/setpoint", SETPOINT);
  wpi::tunables::TunableRegistry::Update();

  CHECK(controller.GetSetpoint() == SETPOINT);
  CHECK(controller.GetError() == SETPOINT);

  controller.Calculate(SETPOINT);

  CHECK(controller.AtSetpoint());

  wpi::tunables::TunableRegistry::Reset();
}

TEST_CASE("PIDToleranceTest TunedGainsUpdateMemberState", "[wpimath]") {
  wpi::tunables::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);

  wpi::math::PIDController controller{0.5, 0.1, 0.01};
  wpi::tunables::Publish("pid", controller);

  auto pUid = backend->GetUid("/pid/p");
  auto iUid = backend->GetUid("/pid/i");
  auto dUid = backend->GetUid("/pid/d");
  auto iZoneUid = backend->GetUid("/pid/izone");
  REQUIRE(pUid.has_value());
  REQUIRE(iUid.has_value());
  REQUIRE(dUid.has_value());
  REQUIRE(iZoneUid.has_value());
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*pUid).IsChanged());

  backend->SetDouble("/pid/p", 1.0);
  backend->SetDouble("/pid/i", 2.0);
  backend->SetDouble("/pid/d", 3.0);
  backend->SetDouble("/pid/izone", 4.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(controller.GetP() == 1.0);
  CHECK(controller.GetI() == 2.0);
  CHECK(controller.GetD() == 3.0);
  CHECK(controller.GetIZone() == 4.0);

  controller.SetPID(5.0, 6.0, 7.0);
  controller.SetIZone(8.0);

  CHECK(wpi::tunables::TunableRegistry::GetTunable(*pUid).IsChanged());
  CHECK(wpi::tunables::TunableRegistry::GetTunable(*iUid).IsChanged());
  CHECK(wpi::tunables::TunableRegistry::GetTunable(*dUid).IsChanged());
  CHECK(wpi::tunables::TunableRegistry::GetTunable(*iZoneUid).IsChanged());

  wpi::tunables::TunableRegistry::Reset();
}

TEST_CASE("PIDToleranceTest AbsoluteTolerance", "[wpimath]") {
  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-RANGE / 2, RANGE / 2);

  CHECK_FALSE(controller.AtSetpoint());

  controller.SetTolerance(TOLERANCE);
  controller.SetSetpoint(SETPOINT);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());

  controller.Calculate(0.0);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());

  controller.Calculate(SETPOINT + TOLERANCE / 2);

  UNSCOPED_INFO(
      "Error was not in tolerance when it should have been. Error was "
      << controller.GetError());
  CHECK(controller.AtSetpoint());

  controller.Calculate(SETPOINT + 10 * TOLERANCE);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());
}
