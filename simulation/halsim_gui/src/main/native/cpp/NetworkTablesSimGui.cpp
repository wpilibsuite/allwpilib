// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkTablesSimGui.h"

#include <glass/Context.h>
#include <glass/Storage.h>
#include <glass/networktables/NetworkTables.h>

#include <wpigui.h>

#include "HALSimGui.h"

using namespace halsimgui;

static std::unique_ptr<glass::NetworkTablesModel> gNetworkTablesModel;
static glass::Window* gNetworkTablesWindow;
static glass::Window* gNetworkTablesInfoWindow;

void NetworkTablesSimGui::Initialize() {
  gNetworkTablesModel = std::make_unique<glass::NetworkTablesModel>();
  wpi::gui::AddEarlyExecute([] { gNetworkTablesModel->Update(); });

  gNetworkTablesWindow = glass::imm::GetOrAddWindow("NetworkTables View");
  gNetworkTablesWindow->SetView(
      std::make_unique<glass::NetworkTablesView>(gNetworkTablesModel.get()));
  gNetworkTablesWindow->SetDefaultPos(250, 277);
  gNetworkTablesWindow->SetDefaultSize(750, 185);

  // NetworkTables info window
  gNetworkTablesInfoWindow = glass::imm::GetOrAddWindow(
      "NetworkTables Info", false, glass::Window::kHide);
  gNetworkTablesInfoWindow->SetView(glass::MakeFunctionView(
      [&] { glass::DisplayNetworkTablesInfo(gNetworkTablesModel.get()); }));
  gNetworkTablesInfoWindow->SetDefaultPos(250, 130);
  gNetworkTablesInfoWindow->SetDefaultSize(750, 145);

  wpi::gui::AddLateExecute([] {
    if (glass::imm::BeginWindow(gNetworkTablesWindow)) {
      if (glass::imm::BeginWindowSettingsPopup()) {
        gNetworkTablesWindow->GetView()->Settings();
        ImGui::EndPopup();
      }
      gNetworkTablesWindow->GetView()->Display();
    }
    glass::imm::EndWindow();

    if (glass::imm::BeginWindow(gNetworkTablesInfoWindow)) {
      glass::DisplayNetworkTablesInfo(gNetworkTablesModel.get());
    }
    glass::imm::EndWindow();
  });

  wpi::gui::AddWindowScaler([](float scale) {
    // scale default window positions
    gNetworkTablesWindow->ScaleDefault(scale);
    gNetworkTablesInfoWindow->ScaleDefault(scale);
  });
}

void NetworkTablesSimGui::DisplayMenu() {
  if (gNetworkTablesWindow) {
    gNetworkTablesWindow->DisplayMenuItem("NetworkTables View");
  }
  if (gNetworkTablesInfoWindow) {
    gNetworkTablesInfoWindow->DisplayMenuItem("NetworkTables Info");
  }
}
