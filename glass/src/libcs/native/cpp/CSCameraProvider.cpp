// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/CSCameraProvider.h"

#include <algorithm>

#include <cscore_cpp.h>
#include <imgui.h>

using namespace glass;

CSCameraProvider::CSCameraProvider(Storage& storage)
    : CameraProviderBase{storage}, m_poller{cs::CreateListenerPoller()} {
  CS_Status status = 0;
  cs::AddPolledListener(m_poller,
                        CS_SOURCE_CREATED | CS_SOURCE_DESTROYED |
                            CS_SOURCE_CONNECTED | CS_SOURCE_DISCONNECTED,
                        true, &status);
}

CSCameraProvider::~CSCameraProvider() {
  cs::DestroyListenerPoller(m_poller);
}

void CSCameraProvider::DisplayMenu() {
  bool any = false;
  for (auto&& info : m_sourceInfo) {
    ImGui::MenuItem(info.name.c_str());
    any = true;
  }
  if (!any) {
    ImGui::MenuItem("(None)", nullptr, false, false);
  }
}

void CSCameraProvider::Update() {
  bool timedOut = false;
  for (auto&& event : cs::PollListener(m_poller, 0, &timedOut)) {
    if (event.kind == cs::RawEvent::kSourceCreated) {
      CS_Status status = 0;
      CSSourceInfo info;
      info.name = event.name;
      info.handle = event.sourceHandle;
      info.kind = cs::GetSourceKind(event.sourceHandle, &status);
      info.connected = cs::IsSourceConnected(event.sourceHandle, &status);
      m_sourceInfo.emplace_back(std::move(info));
      continue;
    }

    auto it = std::find_if(
        m_sourceInfo.begin(), m_sourceInfo.end(),
        [&](const auto& info) { return info.handle == event.sourceHandle; });
    if (it == m_sourceInfo.end()) {
      continue;
    }
    switch (event.kind) {
      case cs::RawEvent::kSourceDestroyed:
        m_sourceInfo.erase(it);
        break;
      case cs::RawEvent::kSourceConnected:
        it->connected = true;
        break;
      case cs::RawEvent::kSourceDisconnected:
        it->connected = false;
        break;
      default:
        break;
    }
  }
#if 0
  // check for visible windows that need displays (typically this is due to
  // file loading)
  for (auto&& window : m_windows) {
    if (!window->IsVisible() || window->HasView()) {
      continue;
    }
    auto id = window->GetId();

    // only handle ones where we have a builder
    auto builderIt = m_typeMap.find(typeIt->second.GetName());
    if (builderIt == m_typeMap.end()) {
      continue;
    }

    auto entry = GetOrCreateView(
        builderIt->second, nt::GetEntry(m_nt.GetInstance(), id + "/.type"), id);
    if (entry) {
      Show(entry, window.get());
    }
  }
#endif
}
