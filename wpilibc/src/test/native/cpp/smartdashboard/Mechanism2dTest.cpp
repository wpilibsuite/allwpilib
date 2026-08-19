// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/Mechanism2d.hpp"

#include <memory>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/smartdashboard/MechanismLigament2d.hpp"
#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/util/Color8Bit.hpp"

struct Mechanism2dTest {
  Mechanism2dTest() {
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", mock);
  }

  ~Mechanism2dTest() { wpi::telemetry::TelemetryRegistry::Reset(); }

  std::shared_ptr<wpi::telemetry::MockTelemetryBackend> mock =
      std::make_shared<wpi::telemetry::MockTelemetryBackend>();
};

TEST_CASE_METHOD(Mechanism2dTest, "Mechanism2dTest Canvas",
                 "[wpilibc][smartdashboard]") {
  wpi::Mechanism2d mechanism{5, 10};

  wpi::telemetry::Log("mechanism", mechanism);
  {
    auto actions = mock->GetActions();
    REQUIRE(actions.size() == 3u);

    auto dims = mock->GetLastValue<std::vector<double>>("/mechanism/dims");
    REQUIRE(dims);
    REQUIRE(2u == dims->size());
    CHECK(5.0 == (*dims)[0]);
    CHECK(10.0 == (*dims)[1]);

    auto color = mock->GetLastValue<
        wpi::telemetry::MockTelemetryBackend::LogStringValue>(
        "/mechanism/backgroundColor");
    REQUIRE(color);
    CHECK("#000020" == color->value);
    mock->Clear();
  }

  mechanism.SetBackgroundColor({255, 255, 255});
  wpi::telemetry::Log("mechanism", mechanism);
  {
    auto color = mock->GetLastValue<
        wpi::telemetry::MockTelemetryBackend::LogStringValue>(
        "/mechanism/backgroundColor");
    REQUIRE(color);
    CHECK("#FFFFFF" == color->value);
  }
}

TEST_CASE_METHOD(Mechanism2dTest, "Mechanism2dTest Root",
                 "[wpilibc][smartdashboard]") {
  wpi::Mechanism2d mechanism{5, 10};
  wpi::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  wpi::telemetry::Log("mechanism", mechanism);
  {
    auto pos =
        mock->GetLastValue<std::vector<double>>("/mechanism/root/position");
    REQUIRE(pos);
    REQUIRE(2u == pos->size());
    CHECK(1.0 == (*pos)[0]);
    CHECK(2.0 == (*pos)[1]);
    mock->Clear();
  }
  root->SetPosition(2, 4);
  wpi::telemetry::Log("mechanism", mechanism);
  {
    auto pos =
        mock->GetLastValue<std::vector<double>>("/mechanism/root/position");
    REQUIRE(pos);
    REQUIRE(2u == pos->size());
    CHECK(2.0 == (*pos)[0]);
    CHECK(4.0 == (*pos)[1]);
  }
}

TEST_CASE_METHOD(Mechanism2dTest, "Mechanism2dTest Ligament",
                 "[wpilibc][smartdashboard]") {
  wpi::Mechanism2d mechanism{5, 10};
  wpi::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  wpi::MechanismLigament2d* ligament = root->Append<wpi::MechanismLigament2d>(
      "ligament", 3, wpi::units::degrees<>{90}, 1,
      wpi::util::Color8Bit{255, 255, 255});
  wpi::telemetry::Log("mechanism", mechanism);
  {
    auto angle = mock->GetLastValue<double>("/mechanism/root/ligament/angle");
    REQUIRE(angle);
    CHECK(ligament->GetAngle() == *angle);
    auto color = mock->GetLastValue<
        wpi::telemetry::MockTelemetryBackend::LogStringValue>(
        "/mechanism/root/ligament/color");
    REQUIRE(color);
    CHECK(ligament->GetColor().HexString() == color->value);
    auto length = mock->GetLastValue<double>("/mechanism/root/ligament/length");
    REQUIRE(length);
    CHECK(ligament->GetLength() == *length);
    auto weight = mock->GetLastValue<double>("/mechanism/root/ligament/weight");
    REQUIRE(weight);
    CHECK(ligament->GetLineWeight() == *weight);
    mock->Clear();
  }

  ligament->SetAngle(wpi::units::degrees<>{45});
  ligament->SetColor({0, 0, 0});
  ligament->SetLength(2);
  ligament->SetLineWeight(4);
  wpi::telemetry::Log("mechanism", mechanism);
  {
    auto angle = mock->GetLastValue<double>("/mechanism/root/ligament/angle");
    REQUIRE(angle);
    CHECK(ligament->GetAngle() == *angle);
    auto color = mock->GetLastValue<
        wpi::telemetry::MockTelemetryBackend::LogStringValue>(
        "/mechanism/root/ligament/color");
    REQUIRE(color);
    CHECK(ligament->GetColor().HexString() == color->value);
    auto length = mock->GetLastValue<double>("/mechanism/root/ligament/length");
    REQUIRE(length);
    CHECK(ligament->GetLength() == *length);
    auto weight = mock->GetLastValue<double>("/mechanism/root/ligament/weight");
    REQUIRE(weight);
    CHECK(ligament->GetLineWeight() == *weight);
  }
}
