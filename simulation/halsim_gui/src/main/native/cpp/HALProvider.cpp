/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HALProvider.h"

#include <glass/Model.h>

#include <algorithm>
#include <string>

#include <hal/simulation/DriverStationData.h>

using namespace halsimgui;

static bool gDisableOutputsOnDSDisable = true;

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
    ImGui::MenuItem(viewEntry->name.c_str(), nullptr, &visible,
                    visible || exists);
    if (!wasVisible && visible) {
      Show(viewEntry.get(), viewEntry->window);
    } else if (wasVisible && !visible && viewEntry->window) {
      viewEntry->window->SetVisible(false);
    }
  }
}

void HALProvider::Update() {
  Provider::Update();

  // check for visible windows that need displays (typically this is due to
  // file loading)
  for (auto&& window : m_windows) {
    if (!window->IsVisible() || window->HasView()) continue;
    auto id = window->GetId();
    auto it = FindViewEntry(id);
    if (it == m_viewEntries.end() || (*it)->name != id) continue;
    Show(it->get(), window.get());
  }
}

glass::Model* HALProvider::GetModel(wpi::StringRef name) {
  auto it = FindModelEntry(name);
  if (it == m_modelEntries.end() || (*it)->name != name) return nullptr;
  auto entry = it->get();

  // get or create model
  if (!entry->model) entry->model = entry->createModel();
  return entry->model.get();
}

void HALProvider::Show(ViewEntry* entry, glass::Window* window) {
  // if there's already a window, just show it
  if (entry->window) {
    entry->window->SetVisible(true);
    return;
  }

  // get or create model
  if (!entry->modelEntry->model)
    entry->modelEntry->model = entry->modelEntry->createModel();
  if (!entry->modelEntry->model) return;

  // the window might exist and we're just not associated to it yet
  if (!window) window = GetOrAddWindow(entry->name, true);
  if (!window) return;
  entry->window = window;

  // create view
  auto view = entry->createView(window, entry->modelEntry->model.get());
  if (!view) return;
  window->SetView(std::move(view));

  entry->window->SetVisible(true);
}
