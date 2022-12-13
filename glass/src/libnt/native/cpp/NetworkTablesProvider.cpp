// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NetworkTablesProvider.h"

#include <algorithm>

#include <fmt/format.h>
#include <ntcore_cpp.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpigui.h>

#include "glass/Storage.h"

using namespace glass;

NetworkTablesProvider::NetworkTablesProvider(Storage& storage)
    : NetworkTablesProvider{storage, nt::NetworkTableInstance::GetDefault()} {}

NetworkTablesProvider::NetworkTablesProvider(Storage& storage,
                                             nt::NetworkTableInstance inst)
    : Provider{storage.GetChild("windows")},
      m_inst{inst},
      m_poller{inst},
      m_typeCache{storage.GetChild("types")} {
  storage.SetCustomApply([this] {
    m_listener = m_poller.AddListener({{""}}, nt::EventFlags::kTopic);
    for (auto&& childIt : m_storage.GetChildren()) {
      auto id = childIt.key();
      auto typePtr = m_typeCache.FindValue(id);
      if (!typePtr || typePtr->type != Storage::Value::kString) {
        continue;
      }

      // only handle ones where we have a builder
      auto builderIt = m_typeMap.find(typePtr->stringVal);
      if (builderIt == m_typeMap.end()) {
        continue;
      }

      auto entry = GetOrCreateView(
          builderIt->second, m_inst.GetTopic(fmt::format("{}/.type", id)), id);
      if (entry) {
        Show(entry, nullptr);
      }
    }
  });
  storage.SetCustomClear([this, &storage] {
    m_poller.RemoveListener(m_listener);
    m_listener = 0;
    for (auto&& modelEntry : m_modelEntries) {
      modelEntry->model.reset();
    }
    m_viewEntries.clear();
    m_windows.clear();
    m_typeCache.EraseAll();
    storage.ClearValues();
  });
}

void NetworkTablesProvider::DisplayMenu() {
  wpi::SmallVector<std::string_view, 6> path;
  wpi::SmallString<64> name;
  for (auto&& entry : m_viewEntries) {
    path.clear();
    wpi::split(entry->name, path, '/', -1, false);

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

    for (; depth > 0; --depth) {
      ImGui::EndMenu();
    }
  }
}

void NetworkTablesProvider::Update() {
  Provider::Update();

  for (auto&& event : m_poller.ReadQueue()) {
    if (auto info = event.GetTopicInfo()) {
      // add/remove entries from NT changes
      // look for .type fields
      if (!wpi::ends_with(info->name, "/.type") || info->type != NT_STRING ||
          info->type_str != "string") {
        continue;
      }

      if (event.flags & nt::EventFlags::kUnpublish) {
        auto it = m_topicMap.find(info->topic);
        if (it != m_topicMap.end()) {
          m_poller.RemoveListener(it->second.listener);
          m_topicMap.erase(it);
        }

        auto it2 = std::find_if(
            m_viewEntries.begin(), m_viewEntries.end(), [&](const auto& elem) {
              return static_cast<Entry*>(elem->modelEntry)
                         ->typeTopic.GetHandle() == info->topic;
            });
        if (it2 != m_viewEntries.end()) {
          m_viewEntries.erase(it2);
        }
      } else if (event.flags & nt::EventFlags::kPublish) {
        // subscribe to it; use a subscriber so we only get string values
        SubListener sublistener;
        sublistener.subscriber = nt::StringTopic{info->topic}.Subscribe("");
        sublistener.listener = m_poller.AddListener(
            sublistener.subscriber,
            nt::EventFlags::kValueAll | nt::EventFlags::kImmediate);
        m_topicMap.try_emplace(info->topic, std::move(sublistener));
      }
    } else if (auto valueData = event.GetValueEventData()) {
      // handle actual .type strings
      if (!valueData->value.IsString()) {
        continue;
      }

      // only handle ones where we have a builder
      auto builderIt = m_typeMap.find(valueData->value.GetString());
      if (builderIt == m_typeMap.end()) {
        continue;
      }

      auto topicName = nt::GetTopicName(valueData->topic);
      auto tableName = wpi::drop_back(topicName, 6);

      GetOrCreateView(builderIt->second, nt::Topic{valueData->topic},
                      tableName);
      // cache the type
      m_typeCache.SetString(tableName, valueData->value.GetString());
    }
  }
}

void NetworkTablesProvider::Register(std::string_view typeName,
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
  if (!entry->modelEntry->model) {
    entry->modelEntry->model =
        entry->modelEntry->createModel(m_inst, entry->name.c_str());
  }
  if (!entry->modelEntry->model) {
    return;
  }

  // the window might exist and we're just not associated to it yet
  if (!window) {
    window = GetOrAddWindow(entry->name, true, Window::kHide);
  }
  if (!window) {
    return;
  }
  if (wpi::starts_with(entry->name, "/SmartDashboard/")) {
    window->SetDefaultName(
        fmt::format("{} (SmartDashboard)", wpi::drop_front(entry->name, 16)));
  }
  entry->window = window;

  // create view
  auto view = entry->createView(window, entry->modelEntry->model.get(),
                                entry->name.c_str());
  if (!view) {
    return;
  }
  window->SetView(std::move(view));

  entry->window->SetVisible(true);
}

NetworkTablesProvider::ViewEntry* NetworkTablesProvider::GetOrCreateView(
    const Builder& builder, nt::Topic typeTopic, std::string_view name) {
  // get view entry if it already exists
  auto viewIt = FindViewEntry(name);
  if (viewIt != m_viewEntries.end() && (*viewIt)->name == name) {
    // make sure typeEntry is set in model
    static_cast<Entry*>((*viewIt)->modelEntry)->typeTopic = typeTopic;
    return viewIt->get();
  }

  // get or create model entry
  auto modelIt = FindModelEntry(name);
  if (modelIt != m_modelEntries.end() && (*modelIt)->name == name) {
    static_cast<Entry*>(modelIt->get())->typeTopic = typeTopic;
  } else {
    modelIt = m_modelEntries.emplace(
        modelIt, std::make_unique<Entry>(typeTopic, name, builder));
  }

  // create new view entry
  viewIt = m_viewEntries.emplace(
      viewIt,
      std::make_unique<ViewEntry>(
          name, modelIt->get(), [](Model*, const char*) { return true; },
          builder.createView));

  return viewIt->get();
}
