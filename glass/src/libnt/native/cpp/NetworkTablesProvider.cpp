/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NetworkTablesProvider.h"

#include <algorithm>

#include <ntcore_cpp.h>
#include <wpi/SmallString.h>
#include <wpigui.h>

using namespace glass;

NetworkTablesProvider::NetworkTablesProvider(const wpi::Twine& iniName)
    : NetworkTablesProvider{iniName, nt::GetDefaultInstance()} {}

NetworkTablesProvider::NetworkTablesProvider(const wpi::Twine& iniName,
                                             NT_Inst inst)
    : Provider{iniName + "Window"}, m_nt{inst}, m_typeCache{iniName} {
  m_nt.AddListener("", NT_NOTIFY_LOCAL | NT_NOTIFY_NEW | NT_NOTIFY_DELETE |
                           NT_NOTIFY_IMMEDIATE);
}

void NetworkTablesProvider::GlobalInit() {
  Provider::GlobalInit();
  wpi::gui::AddInit([this] { m_typeCache.Initialize(); });
}

void NetworkTablesProvider::DisplayMenu() {
  wpi::SmallVector<wpi::StringRef, 6> path;
  wpi::SmallString<64> name;
  for (auto&& entry : m_viewEntries) {
    path.clear();
    wpi::StringRef{entry->name}.split(path, '/', -1, false);

    bool fullDepth = true;
    int depth = 0;
    for (; depth < (static_cast<int>(path.size()) - 1); ++depth) {
      name = path[depth];
      if (!ImGui::BeginMenu(name.c_str())) {
        fullDepth = false;
        break;
      }
    }

    if (fullDepth) {
      bool visible = entry->window && entry->window->IsVisible();
      bool wasVisible = visible;
      // FIXME: enabled?
      // data is the last item, so is guaranteed to be null-terminated
      ImGui::MenuItem(path.back().data(), nullptr, &visible, true);
      if (!wasVisible && visible) {
        Show(entry.get(), entry->window);
      } else if (wasVisible && !visible && entry->window) {
        entry->window->SetVisible(false);
      }
    }

    for (; depth > 0; --depth) ImGui::EndMenu();
  }
}

void NetworkTablesProvider::Update() {
  Provider::Update();

  // add/remove entries from NT changes
  for (auto&& event : m_nt.PollListener()) {
    // look for .type fields
    wpi::StringRef eventName{event.name};
    if (!eventName.endswith("/.type") || !event.value ||
        !event.value->IsString())
      continue;
    auto tableName = eventName.drop_back(6);

    // only handle ones where we have a builder
    auto builderIt = m_typeMap.find(event.value->GetString());
    if (builderIt == m_typeMap.end()) continue;

    if (event.flags & NT_NOTIFY_DELETE) {
      auto it = std::find_if(
          m_viewEntries.begin(), m_viewEntries.end(), [&](const auto& elem) {
            return static_cast<Entry*>(elem->modelEntry)->typeEntry ==
                   event.entry;
          });
      if (it != m_viewEntries.end()) {
        m_viewEntries.erase(it);
      }
    } else if (event.flags & NT_NOTIFY_NEW) {
      GetOrCreateView(builderIt->second, event.entry, tableName);
      // cache the type
      m_typeCache[tableName].SetName(event.value->GetString());
    }
  }

  // check for visible windows that need displays (typically this is due to
  // file loading)
  for (auto&& window : m_windows) {
    if (!window->IsVisible() || window->HasView()) continue;
    auto id = window->GetId();
    auto typeIt = m_typeCache.find(id);
    if (typeIt == m_typeCache.end()) continue;

    // only handle ones where we have a builder
    auto builderIt = m_typeMap.find(typeIt->second.GetName());
    if (builderIt == m_typeMap.end()) continue;

    auto entry = GetOrCreateView(
        builderIt->second, nt::GetEntry(m_nt.GetInstance(), id + "/.type"), id);
    if (entry) Show(entry, window.get());
  }
}

void NetworkTablesProvider::Register(wpi::StringRef typeName,
                                     CreateModelFunc createModel,
                                     CreateViewFunc createView) {
  m_typeMap[typeName] = Builder{std::move(createModel), std::move(createView)};
}

void NetworkTablesProvider::Show(ViewEntry* entry, Window* window) {
  // if there's already a window, just show it
  if (entry->window) {
    entry->window->SetVisible(true);
    return;
  }

  // get or create model
  if (!entry->modelEntry->model)
    entry->modelEntry->model =
        entry->modelEntry->createModel(m_nt.GetInstance(), entry->name.c_str());
  if (!entry->modelEntry->model) return;

  // the window might exist and we're just not associated to it yet
  if (!window) window = GetOrAddWindow(entry->name, true);
  if (!window) return;
  entry->window = window;

  // create view
  auto view = entry->createView(window, entry->modelEntry->model.get(),
                                entry->name.c_str());
  if (!view) return;
  window->SetView(std::move(view));

  entry->window->SetVisible(true);
}

NetworkTablesProvider::ViewEntry* NetworkTablesProvider::GetOrCreateView(
    const Builder& builder, NT_Entry typeEntry, wpi::StringRef name) {
  // get view entry if it already exists
  auto viewIt = FindViewEntry(name);
  if (viewIt != m_viewEntries.end() && (*viewIt)->name == name) {
    // make sure typeEntry is set in model
    static_cast<Entry*>((*viewIt)->modelEntry)->typeEntry = typeEntry;
    return viewIt->get();
  }

  // get or create model entry
  auto modelIt = FindModelEntry(name);
  if (modelIt != m_modelEntries.end() && (*modelIt)->name == name) {
    static_cast<Entry*>(modelIt->get())->typeEntry = typeEntry;
  } else {
    modelIt = m_modelEntries.emplace(
        modelIt, std::make_unique<Entry>(typeEntry, name, builder));
  }

  // create new view entry
  viewIt = m_viewEntries.emplace(
      viewIt,
      std::make_unique<ViewEntry>(
          name, modelIt->get(), [](Model*, const char*) { return true; },
          builder.createView));

  return viewIt->get();
}
