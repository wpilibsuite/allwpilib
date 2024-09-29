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
static std::unique_ptr<glass::Window> gNetworkTablesWindow;
static std::unique_ptr<glass::Window> gNetworkTablesInfoWindow;

void NetworkTablesSimGui::Initialize() {
  gNetworkTablesModel = std::make_unique<glass::NetworkTablesModel>();
  wpi::gui::AddEarlyExecute([] { gNetworkTablesModel->Update(); });

  gNetworkTablesWindow = std::make_unique<glass::Window>(
      glass::GetStorageRoot().GetChild("NetworkTables View"), "NetworkTables");
  gNetworkTablesWindow->SetView(
      std::make_unique<glass::NetworkTablesView>(gNetworkTablesModel.get()));
  gNetworkTablesWindow->SetDefaultPos(250, 277);
  gNetworkTablesWindow->SetDefaultSize(750, 185);
  gNetworkTablesWindow->DisableRenamePopup();
  wpi::gui::AddLateExecute([] { gNetworkTablesWindow->Display(); });

  // NetworkTables info window
  gNetworkTablesInfoWindow = std::make_unique<glass::Window>(
      glass::GetStorageRoot().GetChild("NetworkTables Info"),
      "NetworkTables Info");
  gNetworkTablesInfoWindow->SetView(glass::MakeFunctionView(
      [&] { glass::DisplayNetworkTablesInfo(gNetworkTablesModel.get()); }));
  gNetworkTablesInfoWindow->SetDefaultPos(250, 130);
  gNetworkTablesInfoWindow->SetDefaultSize(750, 145);
  gNetworkTablesInfoWindow->SetDefaultVisibility(glass::Window::kHide);
  gNetworkTablesInfoWindow->DisableRenamePopup();
  wpi::gui::AddLateExecute([] { gNetworkTablesInfoWindow->Display(); });

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
