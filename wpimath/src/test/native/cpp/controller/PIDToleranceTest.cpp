// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/PIDController.hpp"
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

static constexpr double kSetpoint = 50.0;
static constexpr double kRange = 200;
static constexpr double kTolerance = 10.0;

TEST_CASE("PIDToleranceTest InitialTolerance", "[wpimath]") {
  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  CHECK_FALSE(controller.AtSetpoint());
}

TEST_CASE("PIDToleranceTest TunedSetpointUpdatesSetpointState", "[wpimath]") {
  wpi::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::MockTunableBackend>();
  wpi::TunableRegistry::RegisterBackend("", backend);

  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  wpi::Tunables::Publish("pid", controller);

  CHECK_FALSE(controller.AtSetpoint());

  auto setpointUid = backend->GetUid("/pid/setpoint");
  REQUIRE(setpointUid.has_value());
  auto setpointInfo = wpi::TunableRegistry::GetTunable(*setpointUid);
  REQUIRE(static_cast<bool>(setpointInfo));
  REQUIRE(setpointInfo.config != nullptr);
  CHECK(setpointInfo.config->polling ==
        wpi::TunableConfig::Polling::GET_ON_CHANGE);

  controller.SetSetpoint(1.0);
  CHECK(wpi::TunableRegistry::GetTunable(*setpointUid).IsChanged());
  wpi::TunableRegistry::Update();

  backend->SetDouble("/pid/setpoint", kSetpoint);
  wpi::TunableRegistry::Update();

  CHECK(controller.GetSetpoint() == kSetpoint);
  CHECK(controller.GetError() == kSetpoint);

  controller.Calculate(kSetpoint);

  CHECK(controller.AtSetpoint());

  wpi::TunableRegistry::Reset();
}

TEST_CASE("PIDToleranceTest AbsoluteTolerance", "[wpimath]") {
  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  CHECK_FALSE(controller.AtSetpoint());

  controller.SetTolerance(kTolerance);
  controller.SetSetpoint(kSetpoint);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());

  controller.Calculate(0.0);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());

  controller.Calculate(kSetpoint + kTolerance / 2);

  UNSCOPED_INFO(
      "Error was not in tolerance when it should have been. Error was "
      << controller.GetError());
  CHECK(controller.AtSetpoint());

  controller.Calculate(kSetpoint + 10 * kTolerance);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());
}
