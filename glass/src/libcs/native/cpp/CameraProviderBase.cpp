// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/CameraProviderBase.h"

#include <algorithm>

#include <wpigui.h>

#include "glass/camera/Camera.h"
#include "glass/Storage.h"

using namespace glass;

CameraProviderBase::CameraProviderBase(Storage& storage)
    : WindowManager{storage.GetChild("windows")},
      m_modelStorage{storage.GetChildArray("cameras")} {
  for (auto&& v : m_modelStorage) {
    m_models.emplace_back(
        std::make_unique<CameraModel>(*v, v->ReadString("id")));
  }
}

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

CameraModel* CameraProviderBase::CreateModel(std::string_view id) {
  m_modelStorage.emplace_back(std::make_unique<Storage>());
  m_models.emplace_back(
      std::make_unique<CameraModel>(*m_modelStorage.back(), id));
  return m_models.back().get();
}

void CameraProviderBase::UpdateModels() {
  for (auto&& model : m_models) {
    model->Update();
  }
}
