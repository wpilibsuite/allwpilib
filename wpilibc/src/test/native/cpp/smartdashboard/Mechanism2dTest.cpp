// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/smartdashboard/Mechanism2d.hpp"
#include "wpi/smartdashboard/MechanismLigament2d.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/util/Color8Bit.hpp"

class Mechanism2dTest;

TEST(Mechanism2dTest, Canvas) {
  frc::Mechanism2d mechanism{5, 10};
  auto dimsEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/dims");
  auto colorEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/backgroundColor");
  frc::SmartDashboard::PutData("mechanism", &mechanism);
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(5.0, dimsEntry.GetDoubleArray({})[0]);
  EXPECT_EQ(10.0, dimsEntry.GetDoubleArray({})[1]);
  EXPECT_EQ("#000020", colorEntry.GetString(""));
  mechanism.SetBackgroundColor({255, 255, 255});
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ("#FFFFFF", colorEntry.GetString(""));
}

TEST(Mechanism2dTest, Root) {
  frc::Mechanism2d mechanism{5, 10};
  auto xEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/x");
  auto yEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/y");
  frc::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  frc::SmartDashboard::PutData("mechanism", &mechanism);
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(1.0, xEntry.GetDouble(0.0));
  EXPECT_EQ(2.0, yEntry.GetDouble(0.0));
  root->SetPosition(2, 4);
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(2.0, xEntry.GetDouble(0.0));
  EXPECT_EQ(4.0, yEntry.GetDouble(0.0));
}

TEST(Mechanism2dTest, Ligament) {
  frc::Mechanism2d mechanism{5, 10};
  auto angleEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/angle");
  auto colorEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/color");
  auto lengthEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/length");
  auto weightEntry = nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/weight");
  frc::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  frc::MechanismLigament2d* ligament = root->Append<frc::MechanismLigament2d>(
      "ligament", 3, units::degree_t{90}, 1, frc::Color8Bit{255, 255, 255});
  frc::SmartDashboard::PutData("mechanism", &mechanism);
  EXPECT_EQ(ligament->GetAngle(), angleEntry.GetDouble(0.0));
  EXPECT_EQ(ligament->GetColor().HexString(), colorEntry.GetString(""));
  EXPECT_EQ(ligament->GetLength(), lengthEntry.GetDouble(0.0));
  EXPECT_EQ(ligament->GetLineWeight(), weightEntry.GetDouble(0.0));
  ligament->SetAngle(units::degree_t{45});
  ligament->SetColor({0, 0, 0});
  ligament->SetLength(2);
  ligament->SetLineWeight(4);
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(ligament->GetAngle(), angleEntry.GetDouble(0.0));
  EXPECT_EQ(ligament->GetColor().HexString(), colorEntry.GetString(""));
  EXPECT_EQ(ligament->GetLength(), lengthEntry.GetDouble(0.0));
  EXPECT_EQ(ligament->GetLineWeight(), weightEntry.GetDouble(0.0));
  angleEntry.SetDouble(22.5);
  colorEntry.SetString("#FF00FF");
  lengthEntry.SetDouble(4.0);
  weightEntry.SetDouble(6.0);
  frc::SmartDashboard::UpdateValues();
  EXPECT_EQ(ligament->GetAngle(), angleEntry.GetDouble(0.0));
  EXPECT_EQ(ligament->GetColor().HexString(), colorEntry.GetString(""));
  EXPECT_EQ(ligament->GetLength(), lengthEntry.GetDouble(0.0));
  EXPECT_EQ(ligament->GetLineWeight(), weightEntry.GetDouble(0.0));
}
