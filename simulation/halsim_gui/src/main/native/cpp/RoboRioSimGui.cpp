// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RoboRioSimGui.hpp"

#include <memory>

#include "wpi/glass/hardware/RoboRio.hpp"
#include "wpi/hal/simulation/RoboRioData.h"

#include "wpi/halsim/gui/HALDataSource.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioVInVoltage, "Rio Input Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage3V3, "Rio 3.3V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent3V3, "Rio 3.3V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive3V3, "Rio 3.3V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults3V3, "Rio 3.3V Faults");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioBrownoutVoltage, "Rio Brownout Voltage");

class RoboRioUser3V3RailSimModel : public glass::RoboRioRailModel {
 public:
  void Update() override {}
  bool Exists() override { return true; }
  glass::DoubleSource* GetVoltageData() override { return &m_voltage; }
  glass::DoubleSource* GetCurrentData() override { return &m_current; }
  glass::BooleanSource* GetActiveData() override { return &m_active; }
  glass::IntegerSource* GetFaultsData() override { return &m_faults; }

  void SetVoltage(double val) override { HALSIM_SetRoboRioUserVoltage3V3(val); }
  void SetCurrent(double val) override { HALSIM_SetRoboRioUserCurrent3V3(val); }
  void SetActive(bool val) override { HALSIM_SetRoboRioUserActive3V3(val); }
  void SetFaults(int val) override { HALSIM_SetRoboRioUserFaults3V3(val); }

 private:
  RoboRioUserVoltage3V3Source m_voltage;
  RoboRioUserCurrent3V3Source m_current;
  RoboRioUserActive3V3Source m_active;
  RoboRioUserFaults3V3Source m_faults;
};

class RoboRioSimModel : public glass::RoboRioModel {
 public:
  void Update() override {}

  bool Exists() override { return true; }

  glass::RoboRioRailModel* GetUser3V3Rail() override { return &m_user3V3Rail; }

  glass::DoubleSource* GetVInVoltageData() override { return &m_vInVoltage; }
  glass::DoubleSource* GetBrownoutVoltage() override {
    return &m_brownoutVoltage;
  }

  void SetVInVoltage(double val) override { HALSIM_SetRoboRioVInVoltage(val); }
  void SetBrownoutVoltage(double val) override {
    HALSIM_SetRoboRioBrownoutVoltage(val);
  }

 private:
  RoboRioVInVoltageSource m_vInVoltage;
  RoboRioUser3V3RailSimModel m_user3V3Rail;
  RoboRioBrownoutVoltageSource m_brownoutVoltage;
};
}  // namespace

void RoboRioSimGui::Initialize() {
  HALSimGui::halProvider->Register(
      "RoboRIO", [] { return true; },
      [] { return std::make_unique<RoboRioSimModel>(); },
      [](glass::Window* win, glass::Model* model) {
        win->SetFlags(ImGuiWindowFlags_AlwaysAutoResize);
        win->SetDefaultPos(5, 125);
        return glass::MakeFunctionView(
            [=] { DisplayRoboRio(static_cast<RoboRioSimModel*>(model)); });
      });
}
