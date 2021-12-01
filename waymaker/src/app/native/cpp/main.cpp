// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>

#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <imgui.h>
#include <wpigui.h>
#include <string_view>
#include <iostream>
#include "glass/Context.h"
#include "glass/Model.h"
#include "glass/View.h"
#include "glass/other/Log.h"
#include "glass/other/Field2D.h"
#include "glass/other/WMField2D.h"

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
  glass::WMField2DModel* fieldModel = new glass::WMField2DModel("Test Field");
  glass::Field2DView* fieldView = new glass::Field2DView(fieldModel);

  gui::ConfigurePlatformSaveFile("waymaker.ini");

  gui::AddInit([] {
    glass::ResetTime();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
  });



  gui::AddLateExecute([&] {
      ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                                   ImGuiDockNodeFlags_PassthruCentralNode);
      ImGui::Begin("Test");
      ImGui::End();
    ImGui::BeginMainMenuBar();
    gui::EmitViewMenu();
    if (ImGui::BeginMenu("View")) {
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
    ImGui::Begin("Field");
    fieldView->Display();
    ImGui::End();
    ImGui::ShowMetricsWindow();
    ImGui::Begin("Exporter");
      fieldModel->ForEachFieldObject([&](auto& objModel, auto name) {
        if (!objModel.Exists()) {
          return;
        } else {
          ImGui::Text(name.data());
          auto poseList = objModel.GetPoses();
          for (int i = 0; i < poseList.size();
                i++) {
            ImGui::Text(fmt::format("pose: {}, {}, {}\n", poseList[i].X(),
                                   poseList[i].Y(),
                                   poseList[i].Rotation().Radians()).data());
          }
          
        }
      }
      );
    ImGui::End();
    
  });

  gui::Initialize("WayMaker", 1024, 768);
  gui::Main();

  glass::DestroyContext();
  gui::DestroyContext();
}
