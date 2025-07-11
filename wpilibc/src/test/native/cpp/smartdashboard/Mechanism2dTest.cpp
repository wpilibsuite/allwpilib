// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/smartdashboard/Mechanism2d.h>

#include <memory>

#include <gtest/gtest.h>
#include <units/angle.h>
#include <wpi/telemetry/MockTelemetryBackend.h>
#include <wpi/telemetry/Telemetry.h>
#include <wpi/telemetry/TelemetryRegistry.h>

#include "frc/smartdashboard/MechanismLigament2d.h"
#include "frc/util/Color8Bit.h"

struct Mechanism2dTest : public ::testing::Test {
  void SetUp() override {
    wpi::TelemetryRegistry::Reset();
    wpi::TelemetryRegistry::RegisterBackend("", mock);
  }

  void TearDown() override { wpi::TelemetryRegistry::Reset(); }

  std::shared_ptr<wpi::MockTelemetryBackend> mock =
      std::make_shared<wpi::MockTelemetryBackend>();
};

TEST_F(Mechanism2dTest, Canvas) {
  frc::Mechanism2d mechanism{5, 10};

  wpi::Telemetry::Log("mechanism", mechanism);
  {
    auto actions = mock->GetActions();
    ASSERT_EQ(actions.size(), 3u);

    auto dims = mock->GetLastValue<std::vector<double>>("/mechanism/dims");
    ASSERT_TRUE(dims);
    ASSERT_EQ(2u, dims->size());
    EXPECT_EQ(5.0, (*dims)[0]);
    EXPECT_EQ(10.0, (*dims)[1]);

    auto color = mock->GetLastValue<wpi::MockTelemetryBackend::LogStringValue>(
        "/mechanism/backgroundColor");
    ASSERT_TRUE(color);
    EXPECT_EQ("#000020", color->value);
    mock->Clear();
  }

  mechanism.SetBackgroundColor({255, 255, 255});
  wpi::Telemetry::Log("mechanism", mechanism);
  {
    auto actions = mock->GetActions();
    auto color = mock->GetLastValue<wpi::MockTelemetryBackend::LogStringValue>(
        "/mechanism/backgroundColor");
    ASSERT_TRUE(color);
    EXPECT_EQ("#FFFFFF", color->value);
  }
}

TEST_F(Mechanism2dTest, Root) {
  frc::Mechanism2d mechanism{5, 10};
  frc::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  wpi::Telemetry::Log("mechanism", mechanism);
  {
    auto pos =
        mock->GetLastValue<std::vector<double>>("/mechanism/root/position");
    ASSERT_EQ(2u, pos->size());
    EXPECT_EQ(1.0, (*pos)[0]);
    EXPECT_EQ(2.0, (*pos)[1]);
    mock->Clear();
  }
  root->SetPosition(2, 4);
  wpi::Telemetry::Log("mechanism", mechanism);
  {
    auto pos =
        mock->GetLastValue<std::vector<double>>("/mechanism/root/position");
    ASSERT_EQ(2u, pos->size());
    EXPECT_EQ(2.0, (*pos)[0]);
    EXPECT_EQ(4.0, (*pos)[1]);
  }
}

TEST_F(Mechanism2dTest, Ligament) {
  frc::Mechanism2d mechanism{5, 10};
  frc::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  frc::MechanismLigament2d* ligament = root->Append<frc::MechanismLigament2d>(
      "ligament", 3, units::degree_t{90}, 1, frc::Color8Bit{255, 255, 255});
  wpi::Telemetry::Log("mechanism", mechanism);
  {
    auto angle = mock->GetLastValue<double>("/mechanism/root/ligament/angle");
    ASSERT_TRUE(angle);
    EXPECT_EQ(ligament->GetAngle(), *angle);
    auto color = mock->GetLastValue<wpi::MockTelemetryBackend::LogStringValue>(
        "/mechanism/root/ligament/color");
    ASSERT_TRUE(color);
    EXPECT_EQ(ligament->GetColor().HexString(), color->value);
    auto length = mock->GetLastValue<double>("/mechanism/root/ligament/length");
    ASSERT_TRUE(length);
    EXPECT_EQ(ligament->GetLength(), *length);
    auto weight = mock->GetLastValue<double>("/mechanism/root/ligament/weight");
    ASSERT_TRUE(weight);
    EXPECT_EQ(ligament->GetLineWeight(), *weight);
    mock->Clear();
  }

  ligament->SetAngle(units::degree_t{45});
  ligament->SetColor({0, 0, 0});
  ligament->SetLength(2);
  ligament->SetLineWeight(4);
  wpi::Telemetry::Log("mechanism", mechanism);
  {
    auto angle = mock->GetLastValue<double>("/mechanism/root/ligament/angle");
    ASSERT_TRUE(angle);
    EXPECT_EQ(ligament->GetAngle(), *angle);
    auto color = mock->GetLastValue<wpi::MockTelemetryBackend::LogStringValue>(
        "/mechanism/root/ligament/color");
    ASSERT_TRUE(color);
    EXPECT_EQ(ligament->GetColor().HexString(), color->value);
    auto length = mock->GetLastValue<double>("/mechanism/root/ligament/length");
    ASSERT_TRUE(length);
    EXPECT_EQ(ligament->GetLength(), *length);
    auto weight = mock->GetLastValue<double>("/mechanism/root/ligament/weight");
    ASSERT_TRUE(weight);
    EXPECT_EQ(ligament->GetLineWeight(), *weight);
  }
}
