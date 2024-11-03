// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/CameraProvider.h"

#include "glass/Storage.h"

using namespace glass;

void DisplayCameraList() {
  if (glass::imm::BeginWindow(gCameraListWindow)) {
    glass::DisplayCameraModelTable(cameras);
    if (ImGui::Button("Add USB")) {
      ImGui::OpenPopup("Add USB Camera");
    }
    if (ImGui::BeginPopup("Add USB Camera")) {
      std::string path = gUsbCameraList->DisplayMenu();
      if (!path.empty()) {
        auto id = fmt::format("glass::usb::{}", path);
        glass::CameraModel* model = glass::GetOrNewCameraModel(cameras, id);
        if (!model->GetSource()) {
          model->SetSource(cs::UsbCamera(id, path));
        }
        model->Start();
      }
      ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Button("Add HTTP");
    ImGui::SameLine();
    ImGui::Button("Add CameraServer");
  }
  glass::imm::EndWindow();
}

#if 0
    for (auto&& kv :
         glass::GetStorageRoot().GetChild("camera views").GetChildren()) {
      glass::PushStorageStack(kv.value());
      if (!glass::imm::GetWindow()) {
        glass::imm::CreateWindow(
            glass::GetStorageRoot().GetChild("camera views"), kv.key());
      }
      if (glass::imm::BeginWindow()) {
        if (glass::CameraModel* model = glass::GetCameraModel(
                cameras, glass::GetStorage().GetString("camera"))) {
          if (glass::imm::BeginWindowSettingsPopup()) {
            glass::imm::GetWindow()->EditName();
            glass::DisplayCameraSettings(model);
            ImGui::EndPopup();
          }
          glass::DisplayCameraWindow(model);
        }
      }
      glass::imm::EndWindow();
      glass::PopStorageStack();
    }
#endif
CameraProvider::CameraProvider(Storage& storage) : WindowManager{storage} {
  storage.SetCustomApply([this] {
    // loop over windows
    for (auto&& windowkv : m_storage.GetChildren()) {
      // get or create window
      auto win = GetOrAddWindow(windowkv.key(), true);
      if (!win) {
        continue;
      }

      // get or create view
      auto view = static_cast<CameraView*>(win->GetView());
      if (!view) {
        win->SetView(std::make_unique<CameraView>(this, windowkv.value()));
        view = static_cast<CameraView*>(win->GetView());
      }
    }
  });
  storage.SetCustomClear([this] {
    EraseWindows();
    m_storage.EraseChildren();
  });
}
