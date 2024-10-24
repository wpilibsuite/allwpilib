// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkTablesSimGui.h"

#include <memory>

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

  gNetworkTablesWindow = glass::imm::CreateWindow("NetworkTables View");
  gNetworkTablesWindow->SetDefaultPos(250, 277);
  gNetworkTablesWindow->SetDefaultSize(750, 185);

  // NetworkTables info window
  gNetworkTablesInfoWindow = glass::imm::CreateWindow(
      "NetworkTables Info", false, glass::Window::kHide);
  gNetworkTablesInfoWindow->SetDefaultPos(250, 130);
  gNetworkTablesInfoWindow->SetDefaultSize(750, 145);

  wpi::gui::AddLateExecute([] {
    if (glass::imm::BeginWindow(gNetworkTablesWindow)) {
      auto& settings =
          glass::GetStorage().GetOrNewData<glass::NetworkTablesFlagsSettings>();
      if (glass::imm::BeginWindowSettingsPopup()) {
        settings.DisplayMenu();
        glass::DisplayNetworkTablesAddMenu(gNetworkTablesModel.get(), {},
                                           settings.GetFlags());
        ImGui::EndPopup();
      }
      DisplayNetworkTables(gNetworkTablesModel.get(), settings.GetFlags());
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
