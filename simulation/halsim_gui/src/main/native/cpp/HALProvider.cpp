// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALProvider.h"

#include <string>
#include <utility>

#include <glass/Model.h>
#include <glass/Storage.h>
#include <hal/simulation/DriverStationData.h>

using namespace halsimgui;

static bool gDisableOutputsOnDSDisable = true;

HALProvider::HALProvider(glass::Storage& storage) : Provider{storage} {
  storage.SetCustomApply([this] {
    for (auto&& childIt : m_storage.GetChildren()) {
      auto it = FindViewEntry(childIt.key());
      if (it != m_viewEntries.end() && (*it)->name == childIt.key()) {
        Show(it->get(), nullptr);
      }
    }
    for (auto&& entry : m_viewEntries) {
      if (entry->showDefault) {
        Show(entry.get(), entry->window);
      }
    }
  });
  storage.SetCustomClear([this, &storage] {
    for (auto&& entry : m_viewEntries) {
      entry->window = nullptr;
    }
    m_windows.clear();
    storage.ClearValues();
  });
}

bool HALProvider::AreOutputsDisabled() {
  return gDisableOutputsOnDSDisable && !HALSIM_GetDriverStationEnabled();
}

void HALProvider::DisplayMenu() {
  ImGui::MenuItem("Disable outputs on DS disable", nullptr,
                  &gDisableOutputsOnDSDisable, true);
  ImGui::Separator();

  for (auto&& viewEntry : m_viewEntries) {
    bool visible = viewEntry->window && viewEntry->window->IsVisible();
    bool wasVisible = visible;
    bool exists = viewEntry->modelEntry->exists();
    if (ImGui::MenuItem(viewEntry->name.c_str(), nullptr, &visible,
                        visible || exists)) {
      if (!wasVisible && visible) {
        Show(viewEntry.get(), viewEntry->window);
        if (viewEntry->window) {
          viewEntry->window->SetVisible(true);
        }
      } else if (wasVisible && !visible && viewEntry->window) {
        viewEntry->window->SetVisible(false);
      }
    }
  }
}

glass::Model* HALProvider::GetModel(std::string_view name) {
  auto it = FindModelEntry(name);
  if (it == m_modelEntries.end() || (*it)->name != name) {
    return nullptr;
  }
  auto entry = it->get();

  // get or create model
  if (!entry->model) {
    entry->model = entry->createModel();
  }
  return entry->model.get();
}

void HALProvider::Show(ViewEntry* entry, glass::Window* window) {
  // if there's already a window, we're done
  if (entry->window) {
    return;
  }

  // get or create model
  if (!entry->modelEntry->model) {
    entry->modelEntry->model = entry->modelEntry->createModel();
  }
  if (!entry->modelEntry->model) {
    return;
  }

  // the window might exist and we're just not associated to it yet
  if (!window) {
    window = GetOrAddWindow(
        entry->name, true,
        entry->showDefault ? glass::Window::kShow : glass::Window::kHide);
  }
  if (!window) {
    return;
  }
  entry->window = window;

  // create view
  auto view = entry->createView(window, entry->modelEntry->model.get());
  if (!view) {
    return;
  }
  window->SetView(std::move(view));
}
