// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <memory>
#include <numbers>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/controller/ProfiledPIDController.hpp"
#include "wpi/math/trajectory/struct/TrapezoidProfileStruct.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/time.hpp"

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput1", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-180_deg, 180_deg);

  static constexpr wpi::units::degrees<> SETPOINT{-179.0};
  static constexpr wpi::units::degrees<> MEASUREMENT{-179.0};
  static constexpr wpi::units::degrees<> GOAL{179.0};

  controller.Reset(SETPOINT);
  CHECK(controller.Calculate(MEASUREMENT, GOAL) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(wpi::units::abs(controller.GetSetpoint().position - MEASUREMENT) <
        180_deg);
}

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput2", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-wpi::units::radians<>{std::numbers::pi},
                                   wpi::units::radians<>{std::numbers::pi});

  static constexpr wpi::units::radians<> SETPOINT{-3.4826633343199735};
  static constexpr wpi::units::radians<> MEASUREMENT{-3.1352207333939606};
  static constexpr wpi::units::radians<> GOAL{-3.534162788601621};

  controller.Reset(SETPOINT);
  CHECK(controller.Calculate(MEASUREMENT, GOAL) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(wpi::units::abs(controller.GetSetpoint().position - MEASUREMENT) <
        wpi::units::radians<>{std::numbers::pi});
}

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput3", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-wpi::units::radians<>{std::numbers::pi},
                                   wpi::units::radians<>{std::numbers::pi});

  static constexpr wpi::units::radians<> SETPOINT{-3.5176604690006377};
  static constexpr wpi::units::radians<> MEASUREMENT{3.1191729343822456};
  static constexpr wpi::units::radians<> GOAL{2.709680418117445};

  controller.Reset(SETPOINT);
  CHECK(controller.Calculate(MEASUREMENT, GOAL) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(wpi::units::abs(controller.GetSetpoint().position - MEASUREMENT) <
        wpi::units::radians<>{std::numbers::pi});
}

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput4", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(
      0_rad, wpi::units::radians<>{2.0 * std::numbers::pi});

  static constexpr wpi::units::radians<> SETPOINT{2.78};
  static constexpr wpi::units::radians<> MEASUREMENT{3.12};
  static constexpr wpi::units::radians<> GOAL{2.71};

  controller.Reset(SETPOINT);
  CHECK(controller.Calculate(MEASUREMENT, GOAL) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(wpi::units::abs(controller.GetSetpoint().position - MEASUREMENT) <
        wpi::units::radians<>{std::numbers::pi});
}

TEST_CASE("ProfiledPIDInputOutputTest ProportionalGainOutput", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(4);

  CHECK_DOUBLE_EQ(-0.1, controller.Calculate(0.025_deg, 0_deg));
}

TEST_CASE("ProfiledPIDInputOutputTest IntegralGainOutput", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller.Calculate(0.025_deg, 0_deg);
  }

  CHECK_DOUBLE_EQ(-0.5 * controller.GetPeriod().value(), out);
}

TEST_CASE("ProfiledPIDInputOutputTest DerivativeGainOutput", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degrees_> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetD(4);

  controller.Calculate(0_deg, 0_deg);

  CHECK_DOUBLE_EQ(-10_ms / controller.GetPeriod(),
                  controller.Calculate(0.0025_deg, 0_deg));
}

TEST_CASE("ProfiledPIDInputOutputTest TunedConstraintsRebuildProfile",
          "[wpimath]") {
  using Controller = wpi::math::ProfiledPIDController<wpi::units::radians_>;

  wpi::tunables::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);

  Controller controller{0.0, 0.0, 0.0, {1_rad_per_s, 1_rad_per_s_sq}};
  wpi::tunables::Publish("profiled", controller);

  auto constraintsUid = backend->GetUid("/profiled/constraints");
  REQUIRE(constraintsUid.has_value());
  auto constraintsInfo =
      wpi::tunables::TunableRegistry::GetTunable(*constraintsUid);
  REQUIRE(static_cast<bool>(constraintsInfo));
  REQUIRE(constraintsInfo.config != nullptr);
  CHECK(constraintsInfo.config->polling ==
        wpi::tunables::TunableConfig::Polling::GET_ON_CHANGE);
  CHECK_FALSE(constraintsInfo.IsChanged());

  controller.SetConstraints({2_rad_per_s, 2_rad_per_s_sq});
  constraintsInfo = wpi::tunables::TunableRegistry::GetTunable(*constraintsUid);
  CHECK(constraintsInfo.IsChanged());

  backend->SetStruct("/profiled/constraints",
                     Controller::Constraints{10_rad_per_s, 10_rad_per_s_sq});
  wpi::tunables::TunableRegistry::Update();

  CHECK(10_rad_per_s == controller.GetConstraints().maxVelocity);
  CHECK(10_rad_per_s_sq == controller.GetConstraints().maxAcceleration);

  controller.Reset(0_rad);
  controller.Calculate(0_rad, 10_rad);
  CHECK(std::abs(0.2 - controller.GetSetpoint().velocity.value()) <= 1e-9);

  wpi::tunables::TunableRegistry::Reset();
}

TEST_CASE("ProfiledPIDInputOutputTest TunedNestedControllerGainUpdatesState",
          "[wpimath]") {
  using Controller = wpi::math::ProfiledPIDController<wpi::units::radians_>;

  wpi::tunables::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);

  Controller controller{0.0, 0.0, 0.0, {1_rad_per_s, 1_rad_per_s_sq}};
  wpi::tunables::Publish("profiled", controller);

  auto pUid = backend->GetUid("/profiled/controller/p");
  REQUIRE(pUid.has_value());
  CHECK_FALSE(wpi::tunables::TunableRegistry::GetTunable(*pUid).IsChanged());

  backend->SetDouble("/profiled/controller/p", 2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(controller.GetP() == 2.0);

  controller.SetP(3.0);
  CHECK(wpi::tunables::TunableRegistry::GetTunable(*pUid).IsChanged());

  wpi::tunables::TunableRegistry::Reset();
}

TEST_CASE("ProfiledPIDInputOutputTest TunedGoalUpdatesGoal", "[wpimath]") {
  using Controller = wpi::math::ProfiledPIDController<wpi::units::radians_>;

  wpi::tunables::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::tunables::MockTunableBackend>();
  wpi::tunables::TunableRegistry::RegisterBackend("", backend);

  Controller controller{0.0, 0.0, 0.0, {1_rad_per_s, 1_rad_per_s_sq}};
  wpi::tunables::Publish("profiled", controller);

  auto goalUid = backend->GetUid("/profiled/goal");
  REQUIRE(goalUid.has_value());
  auto goalInfo = wpi::tunables::TunableRegistry::GetTunable(*goalUid);
  REQUIRE(static_cast<bool>(goalInfo));
  REQUIRE(goalInfo.config != nullptr);
  CHECK(goalInfo.config->polling ==
        wpi::tunables::TunableConfig::Polling::GET_ON_CHANGE);
  CHECK_FALSE(goalInfo.IsChanged());

  backend->SetDouble("/profiled/goal", 2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(2_rad == controller.GetGoal().position);
  CHECK(0_rad_per_s == controller.GetGoal().velocity);

  controller.SetGoal(3_rad);
  CHECK(wpi::tunables::TunableRegistry::GetTunable(*goalUid).IsChanged());
  wpi::tunables::TunableRegistry::Update();

  CHECK(backend->GetDouble("/profiled/goal") == 3.0);

  wpi::tunables::TunableRegistry::Reset();
}
