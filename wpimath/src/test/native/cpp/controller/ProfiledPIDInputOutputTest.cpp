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
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput1", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-180_deg, 180_deg);

  static constexpr wpi::units::degree_t kSetpoint{-179.0};
  static constexpr wpi::units::degree_t kMeasurement{-179.0};
  static constexpr wpi::units::degree_t kGoal{179.0};

  controller.Reset(kSetpoint);
  CHECK(controller.Calculate(kMeasurement, kGoal) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(wpi::units::math::abs(controller.GetSetpoint().position -
                              kMeasurement) < 180_deg);
}

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput2", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-wpi::units::radian_t{std::numbers::pi},
                                   wpi::units::radian_t{std::numbers::pi});

  static constexpr wpi::units::radian_t kSetpoint{-3.4826633343199735};
  static constexpr wpi::units::radian_t kMeasurement{-3.1352207333939606};
  static constexpr wpi::units::radian_t kGoal{-3.534162788601621};

  controller.Reset(kSetpoint);
  CHECK(controller.Calculate(kMeasurement, kGoal) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement) <
      wpi::units::radian_t{std::numbers::pi});
}

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput3", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(-wpi::units::radian_t{std::numbers::pi},
                                   wpi::units::radian_t{std::numbers::pi});

  static constexpr wpi::units::radian_t kSetpoint{-3.5176604690006377};
  static constexpr wpi::units::radian_t kMeasurement{3.1191729343822456};
  static constexpr wpi::units::radian_t kGoal{2.709680418117445};

  controller.Reset(kSetpoint);
  CHECK(controller.Calculate(kMeasurement, kGoal) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement) <
      wpi::units::radian_t{std::numbers::pi});
}

TEST_CASE("ProfiledPIDInputOutputTest ContinuousInput4", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(1);
  controller.EnableContinuousInput(
      0_rad, wpi::units::radian_t{2.0 * std::numbers::pi});

  static constexpr wpi::units::radian_t kSetpoint{2.78};
  static constexpr wpi::units::radian_t kMeasurement{3.12};
  static constexpr wpi::units::radian_t kGoal{2.71};

  controller.Reset(kSetpoint);
  CHECK(controller.Calculate(kMeasurement, kGoal) < 0.0);

  // Error must be less than half the input range at all times
  CHECK(
      wpi::units::math::abs(controller.GetSetpoint().position - kMeasurement) <
      wpi::units::radian_t{std::numbers::pi});
}

TEST_CASE("ProfiledPIDInputOutputTest ProportionalGainOutput", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetP(4);

  CHECK_DOUBLE_EQ(-0.1, controller.Calculate(0.025_deg, 0_deg));
}

TEST_CASE("ProfiledPIDInputOutputTest IntegralGainOutput", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetI(4);

  double out = 0;

  for (int i = 0; i < 5; i++) {
    out = controller.Calculate(0.025_deg, 0_deg);
  }

  CHECK_DOUBLE_EQ(-0.5 * controller.GetPeriod().value(), out);
}

TEST_CASE("ProfiledPIDInputOutputTest DerivativeGainOutput", "[wpimath]") {
  wpi::math::ProfiledPIDController<wpi::units::degree> controller{
      0.0, 0.0, 0.0, {360_deg_per_s, 180_deg_per_s_sq}};

  controller.SetD(4);

  controller.Calculate(0_deg, 0_deg);

  CHECK_DOUBLE_EQ(-10_ms / controller.GetPeriod(),
                  controller.Calculate(0.0025_deg, 0_deg));
}

TEST_CASE("ProfiledPIDInputOutputTest TunedConstraintsRebuildProfile",
          "[wpimath]") {
  using Controller = wpi::math::ProfiledPIDController<wpi::units::radian>;

  wpi::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::MockTunableBackend>();
  wpi::TunableRegistry::RegisterBackend("", backend);

  Controller controller{0.0, 0.0, 0.0, {1_rad_per_s, 1_rad_per_s_sq}};
  wpi::Tunables::Publish("profiled", controller);

  auto constraintsUid = backend->GetUid("/profiled/constraints");
  REQUIRE(constraintsUid.has_value());
  auto constraintsInfo = wpi::TunableRegistry::GetTunable(*constraintsUid);
  REQUIRE(static_cast<bool>(constraintsInfo));
  REQUIRE(constraintsInfo.config != nullptr);
  CHECK(constraintsInfo.config->polling ==
        wpi::TunableConfig::Polling::GET_ON_CHANGE);
  CHECK_FALSE(constraintsInfo.IsChanged());

  controller.SetConstraints({2_rad_per_s, 2_rad_per_s_sq});
  constraintsInfo = wpi::TunableRegistry::GetTunable(*constraintsUid);
  CHECK(constraintsInfo.IsChanged());

  backend->SetStruct("/profiled/constraints",
                     Controller::Constraints{10_rad_per_s, 10_rad_per_s_sq});
  wpi::TunableRegistry::Update();

  CHECK(10_rad_per_s == controller.GetConstraints().maxVelocity);
  CHECK(10_rad_per_s_sq == controller.GetConstraints().maxAcceleration);

  controller.Reset(0_rad);
  controller.Calculate(0_rad, 10_rad);
  CHECK(std::abs(0.2 - controller.GetSetpoint().velocity.value()) <= 1e-9);

  wpi::TunableRegistry::Reset();
}

TEST_CASE("ProfiledPIDInputOutputTest TunedGoalUpdatesGoal", "[wpimath]") {
  using Controller = wpi::math::ProfiledPIDController<wpi::units::radian>;

  wpi::TunableRegistry::Reset();
  auto backend = std::make_shared<wpi::MockTunableBackend>();
  wpi::TunableRegistry::RegisterBackend("", backend);

  Controller controller{0.0, 0.0, 0.0, {1_rad_per_s, 1_rad_per_s_sq}};
  wpi::Tunables::Publish("profiled", controller);

  auto goalUid = backend->GetUid("/profiled/goal");
  REQUIRE(goalUid.has_value());
  auto goalInfo = wpi::TunableRegistry::GetTunable(*goalUid);
  REQUIRE(static_cast<bool>(goalInfo));
  REQUIRE(goalInfo.config != nullptr);
  CHECK(goalInfo.config->polling == wpi::TunableConfig::Polling::GET_ON_CHANGE);
  CHECK_FALSE(goalInfo.IsChanged());

  backend->SetDouble("/profiled/goal", 2.0);
  wpi::TunableRegistry::Update();

  CHECK(2_rad == controller.GetGoal().position);
  CHECK(0_rad_per_s == controller.GetGoal().velocity);

  controller.SetGoal(3_rad);
  CHECK(wpi::TunableRegistry::GetTunable(*goalUid).IsChanged());
  wpi::TunableRegistry::Update();

  CHECK(backend->GetDouble("/profiled/goal") == 3.0);

  wpi::TunableRegistry::Reset();
}
