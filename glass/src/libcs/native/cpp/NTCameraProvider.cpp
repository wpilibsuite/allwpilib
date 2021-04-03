// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/NTCameraProvider.h"

#include <algorithm>

#include <imgui.h>
#include <ntcore_cpp.h>

#include "cscore_c.h"
#include "cscore_cpp.h"
#include "glass/camera/Camera.h"

using namespace glass;

NTCameraProvider::NTCameraProvider(const wpi::Twine& iniName)
    : NTCameraProvider{iniName, nt::GetDefaultInstance()} {}

NTCameraProvider::NTCameraProvider(const wpi::Twine& iniName, NT_Inst inst)
    : CameraProviderBase{iniName}, m_helper{inst} {
  m_helper.AddListener("/CameraPublisher/");
}

void NTCameraProvider::DisplayMenu() {
  bool any = false;
  for (auto&& info : m_sourceInfo) {
    if (info.connected) {
      MenuItem(info.name.c_str(), &info);
      any = true;
    }
  }
  if (!any) {
    ImGui::MenuItem("(None)", nullptr, false, false);
  }
}

void NTCameraProvider::Update() {
  for (auto&& event : m_helper.PollListener()) {
    wpi::SmallVector<wpi::StringRef, 4> parts;
    wpi::StringRef{event.name}.split(parts, '/', -1, false);
    if (parts.size() < 3) {
      continue;
    }

    // find/create NTSourceInfo by name
    wpi::StringRef sourceName = parts[1];
    auto it =
        std::find_if(m_sourceInfo.begin(), m_sourceInfo.end(),
                     [&](const auto& info) { return info.name == sourceName; });
    NTSourceInfo* info;
    if (it == m_sourceInfo.end()) {
      info = &m_sourceInfo.emplace_back();
      info->name = sourceName;
      // keep sorted by name
      std::sort(m_sourceInfo.begin(), m_sourceInfo.end(),
                [](const auto& a, const auto& b) { return a.name < b.name; });
    } else {
      info = &(*it);
    }

    // update NTSourceInfo
    wpi::StringRef key = parts[2];
    if (event.value &&
        (event.flags & (NT_NOTIFY_NEW | NT_NOTIFY_UPDATE)) != 0) {
      if (key == "description" && event.value->IsString()) {
        info->description = event.value->GetString();
      } else if (key == "connected" && event.value->IsBoolean()) {
        info->connected = event.value->GetBoolean();
      } else if (key == "streams" && event.value->IsStringArray()) {
        info->streams.clear();
        for (auto&& stream : event.value->GetStringArray()) {
          if (wpi::StringRef{stream}.startswith("mjpg:")) {
            info->streams.emplace_back(wpi::StringRef{stream}.drop_front(5));
          }
        }
        if (info->camera) {
          info->camera->SetUrls(info->streams);
        }
      }
    } else if ((event.flags & NT_NOTIFY_DELETE) != 0) {
      if (key == "connected") {
        info->connected = false;
      }
    }
  }
}

void NTCameraProvider::InitCamera(SourceInfo* info) {
  info->camera = CreateModel(info->name);
  CS_Status status = 0;
  auto source = cs::CreateHttpCamera(
      wpi::StringRef{"glass::http::"} + info->name,
      static_cast<NTSourceInfo*>(info)->streams, CS_HTTP_UNKNOWN, &status);
  info->camera->SetSource(source);
}
