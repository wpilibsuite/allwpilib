// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/smartdashboard/Mechanism2d.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/smartdashboard/MechanismLigament2d.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/util/Color8Bit.hpp"

class Mechanism2dTest;

TEST_CASE("Mechanism2dTest Canvas", "[wpilibc][smartdashboard]") {
  wpi::Mechanism2d mechanism{5, 10};
  auto dimsEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/dims");
  auto colorEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/backgroundColor");
  wpi::SmartDashboard::PutData("mechanism", &mechanism);
  wpi::SmartDashboard::UpdateValues();
  CHECK(5.0 == dimsEntry.GetDoubleArray({})[0]);
  CHECK(10.0 == dimsEntry.GetDoubleArray({})[1]);
  CHECK("#000020" == colorEntry.GetString(""));
  mechanism.SetBackgroundColor({255, 255, 255});
  wpi::SmartDashboard::UpdateValues();
  CHECK("#FFFFFF" == colorEntry.GetString(""));
}

TEST_CASE("Mechanism2dTest Root", "[wpilibc][smartdashboard]") {
  wpi::Mechanism2d mechanism{5, 10};
  auto xEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/x");
  auto yEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/y");
  wpi::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  wpi::SmartDashboard::PutData("mechanism", &mechanism);
  wpi::SmartDashboard::UpdateValues();
  CHECK(1.0 == xEntry.GetDouble(0.0));
  CHECK(2.0 == yEntry.GetDouble(0.0));
  root->SetPosition(2, 4);
  wpi::SmartDashboard::UpdateValues();
  CHECK(2.0 == xEntry.GetDouble(0.0));
  CHECK(4.0 == yEntry.GetDouble(0.0));
}

TEST_CASE("Mechanism2dTest Ligament", "[wpilibc][smartdashboard]") {
  wpi::Mechanism2d mechanism{5, 10};
  auto angleEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/angle");
  auto colorEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/color");
  auto lengthEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/length");
  auto weightEntry = wpi::nt::NetworkTableInstance::GetDefault().GetEntry(
      "/SmartDashboard/mechanism/root/ligament/weight");
  wpi::MechanismRoot2d* root = mechanism.GetRoot("root", 1, 2);
  wpi::MechanismLigament2d* ligament = root->Append<wpi::MechanismLigament2d>(
      "ligament", 3, wpi::units::degree_t{90}, 1,
      wpi::util::Color8Bit{255, 255, 255});
  wpi::SmartDashboard::PutData("mechanism", &mechanism);
  CHECK(ligament->GetAngle() == angleEntry.GetDouble(0.0));
  CHECK(ligament->GetColor().HexString() == colorEntry.GetString(""));
  CHECK(ligament->GetLength() == lengthEntry.GetDouble(0.0));
  CHECK(ligament->GetLineWeight() == weightEntry.GetDouble(0.0));
  ligament->SetAngle(wpi::units::degree_t{45});
  ligament->SetColor({0, 0, 0});
  ligament->SetLength(2);
  ligament->SetLineWeight(4);
  wpi::SmartDashboard::UpdateValues();
  CHECK(ligament->GetAngle() == angleEntry.GetDouble(0.0));
  CHECK(ligament->GetColor().HexString() == colorEntry.GetString(""));
  CHECK(ligament->GetLength() == lengthEntry.GetDouble(0.0));
  CHECK(ligament->GetLineWeight() == weightEntry.GetDouble(0.0));
  angleEntry.SetDouble(22.5);
  colorEntry.SetString("#FF00FF");
  lengthEntry.SetDouble(4.0);
  weightEntry.SetDouble(6.0);
  wpi::SmartDashboard::UpdateValues();
  CHECK(ligament->GetAngle() == angleEntry.GetDouble(0.0));
  CHECK(ligament->GetColor().HexString() == colorEntry.GetString(""));
  CHECK(ligament->GetLength() == lengthEntry.GetDouble(0.0));
  CHECK(ligament->GetLineWeight() == weightEntry.GetDouble(0.0));
}
