/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <glass/Context.h>
#include <glass/other/Plot.h>

#include <hal/Extensions.h>
#include <hal/Main.h>
#include <imgui.h>
#include <wpi/StringRef.h>
#include <wpi/raw_ostream.h>
#include <wpigui.h>

#include "AccelerometerSimGui.h"
#include "AddressableLEDGui.h"
#include "AnalogGyroSimGui.h"
#include "AnalogInputSimGui.h"
#include "AnalogOutputSimGui.h"
#include "DIOSimGui.h"
#include "DriverStationGui.h"
#include "EncoderSimGui.h"
#include "HALSimGui.h"
#include "Mechanism2D.h"
#include "NetworkTablesSimGui.h"
#include "PCMSimGui.h"
#include "PDPSimGui.h"
#include "PWMSimGui.h"
#include "RelaySimGui.h"
#include "RoboRioSimGui.h"
#include "SimDeviceGui.h"
#include "TimingGui.h"

using namespace halsimgui;

namespace gui = wpi::gui;

static glass::PlotProvider gPlotProvider{"Plot"};

extern "C" {
#if defined(WIN32) || defined(_WIN32)
__declspec(dllexport)
#endif
    int HALSIM_InitExtension(void) {
  wpi::outs() << "Simulator GUI Initializing.\n";

  gui::CreateContext();
  glass::CreateContext();
  HALSimGui::GlobalInit();
  DriverStationGui::GlobalInit();
  gPlotProvider.GlobalInit();

  // These need to initialize first
  gui::AddInit(EncoderSimGui::Initialize);
  gui::AddInit(SimDeviceGui::Initialize);

  gui::AddInit(AccelerometerSimGui::Initialize);
  gui::AddInit(AddressableLEDGui::Initialize);
  gui::AddInit(AnalogGyroSimGui::Initialize);
  gui::AddInit(AnalogInputSimGui::Initialize);
  gui::AddInit(AnalogOutputSimGui::Initialize);
  gui::AddInit(DIOSimGui::Initialize);
  gui::AddInit(Mechanism2D::Initialize);
  gui::AddInit(NetworkTablesSimGui::Initialize);
  gui::AddInit(PCMSimGui::Initialize);
  gui::AddInit(PDPSimGui::Initialize);
  gui::AddInit(PWMSimGui::Initialize);
  gui::AddInit(RelaySimGui::Initialize);
  gui::AddInit(RoboRioSimGui::Initialize);
  gui::AddInit(TimingGui::Initialize);

  HALSimGui::mainMenu.AddMainMenu([] {
    if (ImGui::BeginMenu("Hardware")) {
      HALSimGui::halProvider.DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("NetworkTables")) {
      NetworkTablesSimGui::DisplayMenu();
      ImGui::Separator();
      HALSimGui::ntProvider.DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("DS")) {
      DriverStationGui::dsManager.DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Plot")) {
      bool paused = gPlotProvider.IsPaused();
      if (ImGui::MenuItem("Pause All Plots", nullptr, &paused)) {
        gPlotProvider.SetPaused(paused);
      }
      ImGui::Separator();
      gPlotProvider.DisplayMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Window")) {
      HALSimGui::manager.DisplayMenu();
      ImGui::EndMenu();
    }
  });

  if (!gui::Initialize("Robot Simulation", 1280, 720)) return 0;
  HAL_RegisterExtensionListener(
      nullptr, [](void*, const char* name, void* data) {
        if (wpi::StringRef{name} == "ds_socket") {
          DriverStationGui::SetDSSocketExtension(data);
        }
      });
  HAL_SetMain(
      nullptr,
      [](void*) {
        gui::Main();
        glass::DestroyContext();
        gui::DestroyContext();
      },
      [](void*) { gui::Exit(); });
  wpi::outs() << "Simulator GUI Initialized!\n";

  return 0;
}
}  // extern "C"
