// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALSimGui.h"

#include <imgui.h>
#include <wpigui.h>

using namespace halsimgui;

glass::MainMenuBar HALSimGui::mainMenu;
glass::WindowManager HALSimGui::manager{"SimWindow"};
HALProvider HALSimGui::halProvider{"HALProvider"};
glass::NetworkTablesProvider HALSimGui::ntProvider{"NTProvider"};

void HALSimGui::GlobalInit() {
  manager.GlobalInit();
  halProvider.GlobalInit();
  ntProvider.GlobalInit();

  wpi::gui::AddLateExecute([] { mainMenu.Display(); });

  glass::AddStandardNetworkTablesViews(ntProvider);
}
