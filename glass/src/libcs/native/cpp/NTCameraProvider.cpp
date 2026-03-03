// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/NTCameraProvider.h"

#include <algorithm>

#include <fmt/format.h>
#include <imgui.h>
#include <cscore_cpp.h>
#include <wpi/StringExtras.h>

#include "cscore_oo.h"
#include "glass/camera/Camera.h"
#include "ntcore_c.h"

using namespace glass;

NTCameraProvider::NTCameraProvider(Storage& storage)
    : NTCameraProvider{storage, nt::NetworkTableInstance::GetDefault()} {}

NTCameraProvider::NTCameraProvider(Storage& storage,
                                   nt::NetworkTableInstance inst)
    : CameraProviderBase{storage}, m_inst{inst}, m_poller{inst} {
  m_listener =
      m_poller.AddListener({{"/CameraPublisher/"}},
                           nt::EventFlags::kTopic | nt::EventFlags::kValueAll);
}

void NTCameraProvider::DisplayMenu() {
  bool any = false;
  for (auto&& info : m_sourceInfo) {
    if (info->connected) {
      MenuItem(info->name.c_str(), info.get());
      any = true;
    }
  }
  if (!any) {
    ImGui::MenuItem("(None)", nullptr, false, false);
  }
}

void NTCameraProvider::Update() {
  for (nt::Event& event : m_poller.ReadQueue()) {
    if (nt::TopicInfo* topicInfo = event.GetTopicInfo()) {
      wpi::SmallVector<std::string_view, 4> parts;
      wpi::split(topicInfo->name, parts, '/', -1, false);
      if (parts.size() < 3) {
        continue;
      }

      // find/create NTSourceInfo by name
      std::string_view sourceName = parts[1];
      auto it = std::find_if(
          m_sourceInfo.begin(), m_sourceInfo.end(),
          [&](const auto& info) { return info->name == sourceName; });
      NTSourceInfo* info;
      if (it == m_sourceInfo.end()) {
        info =
            m_sourceInfo.emplace_back(std::make_unique<NTSourceInfo>()).get();
        info->name = sourceName;
        // keep sorted by name
        std::sort(
            m_sourceInfo.begin(), m_sourceInfo.end(),
            [](const auto& a, const auto& b) { return a->name < b->name; });
      } else {
        info = it->get();
      }

      // update NTSourceInfo
      std::string_view key = parts[2];
      if ((event.flags & NT_EVENT_PUBLISH) != 0) {
        if (key == "description" && topicInfo->type == NT_STRING) {
          info->descTopic = topicInfo->topic;
          m_topicMap.try_emplace(topicInfo->topic, info);
        } else if (key == "connected" && topicInfo->type == NT_BOOLEAN) {
          info->connTopic = topicInfo->topic;
          m_topicMap.try_emplace(topicInfo->topic, info);
        } else if (key == "streams" && topicInfo->type == NT_STRING_ARRAY) {
          info->streamsTopic = topicInfo->topic;
          m_topicMap.try_emplace(topicInfo->topic, info);
        }
      } else if ((event.flags & NT_EVENT_UNPUBLISH) != 0) {
        if (key == "description") {
          m_topicMap.erase(info->connTopic);
          info->descTopic = 0;
        } else if (key == "connected") {
          m_topicMap.erase(info->connTopic);
          info->connTopic = 0;
          info->connected = false;
        } else if (key == "streams") {
          m_topicMap.erase(info->connTopic);
          info->streamsTopic = 0;
        }
      }
    }

    if (nt::ValueEventData* valueData = event.GetValueEventData()) {
      auto it = m_topicMap.find(valueData->topic);
      if (it != m_topicMap.end()) {
        NTSourceInfo* info = it->getSecond();
        if (valueData->topic == info->descTopic &&
            valueData->value.IsString()) {
          info->description = valueData->value.GetString();
        } else if (valueData->topic == info->connTopic &&
                   valueData->value.IsBoolean()) {
          info->connected = valueData->value.GetBoolean();
        } else if (valueData->topic == info->streamsTopic &&
                   valueData->value.IsStringArray()) {
          info->streams.clear();
          for (auto&& stream : valueData->value.GetStringArray()) {
            if (wpi::starts_with(stream, "mjpg:")) {
              info->streams.emplace_back(wpi::drop_front(stream, 5));
            }
          }
          if (info->camera) {
            info->camera->SetUrls(info->streams);
          }
        }
      }
    }
  }
}

void NTCameraProvider::InitCamera(SourceInfo* info) {
  info->camera = CreateModel(info->name);
  info->camera->SetSource(cs::HttpCamera(
      fmt::format("glass::http::{}", info->name),
      static_cast<NTSourceInfo*>(info)->streams, cs::HttpCamera::kUnknown));
}
