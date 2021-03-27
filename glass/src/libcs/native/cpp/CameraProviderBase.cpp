// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/CameraProviderBase.h"

#include <algorithm>

#include <wpigui.h>

#include "glass/camera/Camera.h"

using namespace glass;

CameraProviderBase::CameraProviderBase(const wpi::Twine& iniName)
    : WindowManager{iniName} {}

CameraProviderBase::~CameraProviderBase() = default;

void CameraProviderBase::GlobalInit() {
  WindowManager::GlobalInit();
  wpi::gui::AddEarlyExecute([this] {
    Update();
    UpdateModels();
  });
}

void CameraProviderBase::MenuItem(const char* label, SourceInfo* info) {
  bool visible = info->window && info->window->IsVisible();
  bool wasVisible = visible;
  ImGui::MenuItem(label, nullptr, &visible);
  if (!wasVisible && visible) {
    Show(info, info->window);
  } else if (wasVisible && !visible && info->window) {
    info->window->SetVisible(false);
  }
}

void CameraProviderBase::Show(SourceInfo* info, Window* window) {
  // if there's already a window, just show it
  if (info->window) {
    info->window->SetVisible(true);
    return;
  }

  // get or create model
  if (!info->camera) {
    InitCamera(info);
  }
  if (!info->camera) {
    return;
  }

  // the window might exist and we're just not associated to it yet
  if (!window) {
    window = GetOrAddWindow(info->name, true);
  }
  if (!window) {
    return;
  }
  info->window = window;

  // create view
  auto view = std::make_unique<CameraView>(info->camera);
  if (!view) {
    return;
  }
  window->SetView(std::move(view));

  info->window->SetVisible(true);
}

void CameraProviderBase::DisplayWindows() {
  WindowManager::DisplayWindows();
}

CameraModel* CameraProviderBase::CreateModel(const wpi::Twine& name) {
  m_models.emplace_back(std::make_unique<CameraModel>(name));
  return m_models.back().get();
}

void CameraProviderBase::UpdateModels() {
  for (auto&& model : m_models) {
    model->Update();
  }
}

CameraProviderBase::IniSaver::IniSaver(const wpi::Twine& typeName,
                                       CameraProviderBase* provider)
    : IniSaverBase{typeName}, m_provider{provider} {}

void* CameraProviderBase::IniSaver::IniReadOpen(const char* name) {
  // get or create window
  auto win = m_provider->GetOrAddWindow(name, true);
  if (!win) {
    return nullptr;
  }

  // get or create view
  auto view = static_cast<CameraView*>(win->GetView());
  if (!view) {
    win->SetView(std::make_unique<CameraView>(m_provider));
    view = static_cast<CameraView*>(win->GetView());
  }
}

void CameraProviderBase::IniSaver::IniReadLine(void* entry,
                                               const char* lineStr) {
  auto [name, value] = wpi::StringRef{lineStr}.split('=');
  name = name.trim();
  value = value.trim();
  static_cast<SourceInfo*>(entry)->ReadIni(name, value);
}

void CameraProviderBase::IniSaver::IniWriteAll(ImGuiTextBuffer* out_buf) {
  m_provider->IniWriteAll(out_buf);
}
