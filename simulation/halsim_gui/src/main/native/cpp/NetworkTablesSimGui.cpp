/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "NetworkTablesSimGui.h"

#include <glass/networktables/NetworkTables.h>

#include <wpigui.h>

#include "HALSimGui.h"

using namespace halsimgui;

static std::unique_ptr<glass::NetworkTablesModel> gNetworkTablesModel;
static std::unique_ptr<glass::NetworkTablesView> gNetworkTablesView;
static glass::Window* gNetworkTablesWindow;

void NetworkTablesSimGui::Initialize() {
  gNetworkTablesModel = std::make_unique<glass::NetworkTablesModel>();
  gNetworkTablesView =
      std::make_unique<glass::NetworkTablesView>(gNetworkTablesModel.get());
  wpi::gui::AddEarlyExecute([] { gNetworkTablesModel->Update(); });
  gNetworkTablesWindow = HALSimGui::ntProvider.AddWindow(
      "NetworkTables", [] { gNetworkTablesView->Display(); });
  if (gNetworkTablesWindow) {
    gNetworkTablesWindow->SetDefaultPos(250, 277);
    gNetworkTablesWindow->SetDefaultSize(750, 185);
    gNetworkTablesWindow->DisableRenamePopup();
  }
}

void NetworkTablesSimGui::DisplayMenu() {
  if (gNetworkTablesWindow) {
    gNetworkTablesWindow->DisplayMenuItem("NetworkTables View");
  }
}
