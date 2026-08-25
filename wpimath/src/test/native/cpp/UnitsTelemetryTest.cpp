// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <variant>

#include <catch2/catch_test_macros.hpp>

#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/telemetry/TelemetryTable.hpp"
#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/area.hpp"
#include "wpi/units/core.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/power.hpp"
#include "wpi/units/tunable.hpp"
#include "wpi/units/velocity.hpp"

class UnitTelemetry {
 public:
  UnitTelemetry() {
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", mock);
  }

  ~UnitTelemetry() { wpi::telemetry::TelemetryRegistry::Reset(); }

  std::shared_ptr<wpi::telemetry::MockTelemetryBackend> mock =
      std::make_shared<wpi::telemetry::MockTelemetryBackend>();
};

static_assert(
    wpi::units::detail::ComplexAbbrev<wpi::units::dimensionless<>>() == "" &&
    wpi::units::detail::ComplexAbbrev<wpi::units::radians<>>() == "rad" &&
    wpi::units::detail::ComplexAbbrev<wpi::units::meters<>>() == "m" &&
    wpi::units::detail::ComplexAbbrev<wpi::units::square_meters<>>() == "m^2" &&
    wpi::units::detail::ComplexAbbrev<wpi::units::meters_per_second<>>() ==
        "m s^-1" &&
    wpi::units::detail::ComplexAbbrev<decltype(4_mps / 1_m)>() == "s^-1");
static_assert(wpi::telemetry::SupportsTelemetryValue<wpi::units::meters<>>);

TEST_CASE_METHOD(UnitTelemetry, "UnitTelemetry Log", "[wpimath]") {
  wpi::telemetry::TelemetryTable& table =
      wpi::telemetry::TelemetryRegistry::GetTable("/");
  table.Log("testmeter", wpi::units::meters<>(5.0));
  table.Log("testsquaremeter", wpi::units::square_meters<>(3.0));
  table.Log("testwatt", wpi::units::watts<>(3.0));
  auto actions = mock->GetActions();
  REQUIRE(actions.size() == 6u);

  REQUIRE(actions[0].path == "/testmeter");
  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
      actions[0].value));
  REQUIRE(std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
              actions[0].value)
              .key == "unit");
  REQUIRE(std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
              actions[0].value)
              .value == "\"m\"");

  REQUIRE(actions[1].path == "/testmeter");
  REQUIRE(std::holds_alternative<double>(actions[1].value));
  REQUIRE(std::get<double>(actions[1].value) == 5);

  REQUIRE(actions[2].path == "/testsquaremeter");
  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
      actions[2].value));
  REQUIRE(std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
              actions[2].value)
              .key == "unit");
  // This is m2, not m^2 because area.hpp overrides the logging to use its own
  // abbreviations
  REQUIRE(std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
              actions[2].value)
              .value == "\"m2\"");

  REQUIRE(actions[3].path == "/testsquaremeter");
  REQUIRE(std::holds_alternative<double>(actions[3].value));
  REQUIRE(std::get<double>(actions[3].value) == 3);

  REQUIRE(actions[4].path == "/testwatt");
  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
      actions[4].value));
  REQUIRE(std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
              actions[4].value)
              .key == "unit");
  REQUIRE(std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
              actions[4].value)
              .value == "\"W\"");

  REQUIRE(actions[5].path == "/testwatt");
  REQUIRE(std::holds_alternative<double>(actions[5].value));
  REQUIRE(std::get<double>(actions[5].value) == 3);
}
