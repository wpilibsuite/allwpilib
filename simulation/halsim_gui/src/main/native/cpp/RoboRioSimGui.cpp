// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RoboRioSimGui.h"

#include <memory>

#include <glass/hardware/RoboRio.h>
#include <hal/simulation/RoboRioData.h>

#include "HALDataSource.h"
#include "HALSimGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioFPGAButton, "Rio User Button");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioVInVoltage, "Rio Input Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioVInCurrent, "Rio Input Current");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage6V, "Rio 6V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent6V, "Rio 6V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive6V, "Rio 6V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults6V, "Rio 6V Faults");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage5V, "Rio 5V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent5V, "Rio 5V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive5V, "Rio 5V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults5V, "Rio 5V Faults");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserVoltage3V3, "Rio 3.3V Voltage");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioUserCurrent3V3, "Rio 3.3V Current");
HALSIMGUI_DATASOURCE_BOOLEAN(RoboRioUserActive3V3, "Rio 3.3V Active");
HALSIMGUI_DATASOURCE_INT(RoboRioUserFaults3V3, "Rio 3.3V Faults");
HALSIMGUI_DATASOURCE_DOUBLE(RoboRioBrownoutVoltage, "Rio Brownout Voltage");

class RoboRioUser6VRailSimModel : public glass::RoboRioRailModel {
 public:
  void Update() override {}
  bool Exists() override { return true; }
  glass::DataSource* GetVoltageData() override { return &m_voltage; }
  glass::DataSource* GetCurrentData() override { return &m_current; }
  glass::DataSource* GetActiveData() override { return &m_active; }
  glass::DataSource* GetFaultsData() override { return &m_faults; }

  void SetVoltage(double val) override { HALSIM_SetRoboRioUserVoltage6V(val); }
  void SetCurrent(double val) override { HALSIM_SetRoboRioUserCurrent6V(val); }
  void SetActive(bool val) override { HALSIM_SetRoboRioUserActive6V(val); }
  void SetFaults(int val) override { HALSIM_SetRoboRioUserFaults6V(val); }

 private:
  RoboRioUserVoltage6VSource m_voltage;
  RoboRioUserCurrent6VSource m_current;
  RoboRioUserActive6VSource m_active;
  RoboRioUserFaults6VSource m_faults;
};

class RoboRioUser5VRailSimModel : public glass::RoboRioRailModel {
 public:
  void Update() override {}
  bool Exists() override { return true; }
  glass::DataSource* GetVoltageData() override { return &m_voltage; }
  glass::DataSource* GetCurrentData() override { return &m_current; }
  glass::DataSource* GetActiveData() override { return &m_active; }
  glass::DataSource* GetFaultsData() override { return &m_faults; }

  void SetVoltage(double val) override { HALSIM_SetRoboRioUserVoltage5V(val); }
  void SetCurrent(double val) override { HALSIM_SetRoboRioUserCurrent5V(val); }
  void SetActive(bool val) override { HALSIM_SetRoboRioUserActive5V(val); }
  void SetFaults(int val) override { HALSIM_SetRoboRioUserFaults5V(val); }

 private:
  RoboRioUserVoltage5VSource m_voltage;
  RoboRioUserCurrent5VSource m_current;
  RoboRioUserActive5VSource m_active;
  RoboRioUserFaults5VSource m_faults;
};

class RoboRioUser3V3RailSimModel : public glass::RoboRioRailModel {
 public:
  void Update() override {}
  bool Exists() override { return true; }
  glass::DataSource* GetVoltageData() override { return &m_voltage; }
  glass::DataSource* GetCurrentData() override { return &m_current; }
  glass::DataSource* GetActiveData() override { return &m_active; }
  glass::DataSource* GetFaultsData() override { return &m_faults; }

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

  glass::RoboRioRailModel* GetUser6VRail() override { return &m_user6VRail; }
  glass::RoboRioRailModel* GetUser5VRail() override { return &m_user5VRail; }
  glass::RoboRioRailModel* GetUser3V3Rail() override { return &m_user3V3Rail; }

  glass::DataSource* GetUserButton() override { return &m_userButton; }
  glass::DataSource* GetVInVoltageData() override { return &m_vInVoltage; }
  glass::DataSource* GetVInCurrentData() override { return &m_vInCurrent; }
  glass::DataSource* GetBrownoutVoltage() override {
    return &m_brownoutVoltage;
  }

  void SetUserButton(bool val) override { HALSIM_SetRoboRioFPGAButton(val); }
  void SetVInVoltage(double val) override { HALSIM_SetRoboRioVInVoltage(val); }
  void SetVInCurrent(double val) override { HALSIM_SetRoboRioVInCurrent(val); }
  void SetBrownoutVoltage(double val) override {
    HALSIM_SetRoboRioBrownoutVoltage(val);
  }

 private:
  RoboRioFPGAButtonSource m_userButton;
  RoboRioVInVoltageSource m_vInVoltage;
  RoboRioVInCurrentSource m_vInCurrent;
  RoboRioUser6VRailSimModel m_user6VRail;
  RoboRioUser5VRailSimModel m_user5VRail;
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
