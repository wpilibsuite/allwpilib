/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
