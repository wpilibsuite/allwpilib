// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkTablesSimGui.hpp"

#include <memory>

#include "wpi/glass/Context.hpp"
#include "wpi/glass/Storage.hpp"
#include "wpi/glass/networktables/NetworkTables.hpp"
#include "wpi/gui/wpigui.hpp"
#include "wpi/halsim/gui/HALSimGui.hpp"

using namespace halsimgui;

static std::unique_ptr<wpi::glass::NetworkTablesModel> gNetworkTablesModel;
static std::unique_ptr<wpi::glass::Window> gNetworkTablesWindow;
static std::unique_ptr<wpi::glass::Window> gNetworkTablesInfoWindow;

void NetworkTablesSimGui::Initialize() {
  gNetworkTablesModel = std::make_unique<wpi::glass::NetworkTablesModel>();
  wpi::gui::AddEarlyExecute([] { gNetworkTablesModel->Update(); });

  gNetworkTablesWindow = std::make_unique<wpi::glass::Window>(
      wpi::glass::GetStorageRoot().GetChild("NetworkTables View"), "NetworkTables");
  gNetworkTablesWindow->SetView(
      std::make_unique<wpi::glass::NetworkTablesView>(gNetworkTablesModel.get()));
  gNetworkTablesWindow->SetDefaultPos(250, 277);
  gNetworkTablesWindow->SetDefaultSize(750, 185);
  gNetworkTablesWindow->DisableRenamePopup();
  wpi::gui::AddLateExecute([] { gNetworkTablesWindow->Display(); });

  // NetworkTables info window
  gNetworkTablesInfoWindow = std::make_unique<wpi::glass::Window>(
      wpi::glass::GetStorageRoot().GetChild("NetworkTables Info"),
      "NetworkTables Info");
  gNetworkTablesInfoWindow->SetView(wpi::glass::MakeFunctionView(
      [&] { wpi::glass::DisplayNetworkTablesInfo(gNetworkTablesModel.get()); }));
  gNetworkTablesInfoWindow->SetDefaultPos(250, 130);
  gNetworkTablesInfoWindow->SetDefaultSize(750, 145);
  gNetworkTablesInfoWindow->SetDefaultVisibility(wpi::glass::Window::kHide);
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
