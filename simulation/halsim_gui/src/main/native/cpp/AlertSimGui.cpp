// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AlertSimGui.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/Window.hpp"
#include "wpi/glass/other/Alerts.hpp"
#include "wpi/gui/wpigui.hpp"
#include "wpi/util/Alert.h"
#include "wpi/util/string.hpp"

using namespace halsimgui;

namespace {

class AlertSimModel : public wpi::glass::AlertsModel {
 public:
  const std::vector<wpi::glass::AlertData>& GetAlerts() override {
    return m_alerts;
  }

  void Update() override;
  bool Exists() override { return true; }

 private:
  static wpi::glass::AlertData::Level GetLevel(int32_t level);

  std::vector<wpi::glass::AlertData> m_alerts;
};

std::unique_ptr<AlertSimModel> gAlertModel;
std::unique_ptr<wpi::glass::Window> gAlertWindow;

}  // namespace

void AlertSimModel::Update() {
  m_alerts.clear();

  int32_t allocLen = WPI_GetNumAlerts();
  if (allocLen <= 0) {
    return;
  }

  std::vector<WPI_AlertInfo> alerts(allocLen);
  int32_t len = WPI_GetAlerts(alerts.data(), allocLen);
  if (len <= 0) {
    return;
  }

  int32_t count = std::min(len, allocLen);
  m_alerts.reserve(count);
  for (int32_t i = 0; i < count; ++i) {
    const auto& alert = alerts[i];
    if (alert.activeStartTime != 0) {
      m_alerts.emplace_back(wpi::util::to_string_view(&alert.group),
                            wpi::util::to_string_view(&alert.id),
                            wpi::util::to_string_view(&alert.text),
                            alert.activeStartTime, GetLevel(alert.level));
    }
  }

  WPI_FreeAlerts(alerts.data(), count);
}

wpi::glass::AlertData::Level AlertSimModel::GetLevel(int32_t level) {
  switch (level) {
    case WPI_ALERT_HIGH:
      return wpi::glass::AlertData::Level::HIGH;
    case WPI_ALERT_MEDIUM:
      return wpi::glass::AlertData::Level::MEDIUM;
    case WPI_ALERT_LOW:
      return wpi::glass::AlertData::Level::LOW;
    default:
      return wpi::glass::AlertData::Level::LOW;
  }
}

void AlertSimGui::Initialize() {
  gAlertModel = std::make_unique<AlertSimModel>();
  wpi::gui::AddEarlyExecute([] { gAlertModel->Update(); });

  gAlertWindow = std::make_unique<wpi::glass::Window>(
      wpi::glass::GetStorageRoot().GetChild("Alerts"), "Alerts",
      wpi::glass::Window::HIDE);
  gAlertWindow->SetView(std::make_unique<wpi::glass::AlertsView>(
      gAlertModel.get(), gAlertWindow->GetStorage()));
  gAlertWindow->SetDefaultPos(250, 130);
  gAlertWindow->SetDefaultSize(400, 150);
  gAlertWindow->DisableRenamePopup();
  wpi::gui::AddLateExecute([] { gAlertWindow->Display(); });

  wpi::gui::AddWindowScaler(
      [](float scale) { gAlertWindow->ScaleDefault(scale); });
}

void AlertSimGui::DisplayMenu() {
  if (gAlertWindow) {
    gAlertWindow->DisplayMenuItem();
  }
}
