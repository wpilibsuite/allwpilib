// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <wpigui.h>
#include <string_view>
#include "glass/Context.h"
#include "glass/Model.h"
#include "glass/View.h"
#include "glass/other/Log.h"
#include "glass/other/Field2D.h"

namespace gui = wpi::gui;

const char* GetWPILibVersion();



#ifdef _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine,
                      int nCmdShow) {
#else
int main() {
#endif
  gui::CreateContext();
  glass::CreateContext();


  gui::ConfigurePlatformSaveFile("waymaker.ini");

  gui::AddInit([] { glass::ResetTime(); });



  gui::AddLateExecute([] {
    ImGui::BeginMainMenuBar();
    gui::EmitViewMenu();
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Reset Time")) {
        glass::ResetTime();
      }
      ImGui::EndMenu();
    }
 

    bool about = false;
    if (ImGui::BeginMenu("Info")) {
      if (ImGui::MenuItem("About")) {
        about = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    if (about) {
      ImGui::OpenPopup("About");
      about = false;
    }
    if (ImGui::BeginPopupModal("About")) {
      ImGui::Text("Glass: A different kind of dashboard");
      ImGui::Separator();
      ImGui::Text("v%s", GetWPILibVersion());
      if (ImGui::Button("Close")) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  });

  gui::Initialize("Glass - DISCONNECTED", 1024, 768);
  gui::Main();

  glass::DestroyContext();
  gui::DestroyContext();
}
