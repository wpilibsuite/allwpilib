// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/BangBangController.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"

TEST_CASE("BangBangToleranceTest InTolerance", "[wpimath]") {
  wpi::math::BangBangController controller{0.1};

  controller.SetSetpoint(1);
  controller.Calculate(1);
  CHECK(controller.AtSetpoint());
}

TEST_CASE("BangBangToleranceTest OutOfTolerance", "[wpimath]") {
  wpi::math::BangBangController controller{0.1};

  controller.SetSetpoint(1);
  controller.Calculate(0);
  CHECK_FALSE(controller.AtSetpoint());
}

TEST_CASE("BangBangToleranceTest TunedMembersUpdateState", "[wpimath]") {
  wpi::tunables::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);

  wpi::math::BangBangController controller{0.1};
  wpi::tunables::Publish("bangbang", controller);

  auto toleranceUid = backend->GetUid("/bangbang/tolerance");
  auto setpointUid = backend->GetUid("/bangbang/setpoint");
  REQUIRE(toleranceUid.has_value());
  REQUIRE(setpointUid.has_value());
  CHECK_FALSE(
      wpi::tunables::TunableRegistry::GetTunable(*toleranceUid).IsChanged());

  backend->SetDouble("/bangbang/tolerance", 0.25);
  backend->SetDouble("/bangbang/setpoint", 4.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(controller.GetTolerance() == 0.25);
  CHECK(controller.GetSetpoint() == 4.0);

  controller.SetTolerance(0.5);
  controller.SetSetpoint(6.0);

  CHECK(wpi::tunables::TunableRegistry::GetTunable(*toleranceUid).IsChanged());
  CHECK(wpi::tunables::TunableRegistry::GetTunable(*setpointUid).IsChanged());

  wpi::tunables::TunableRegistry::Reset();
}
