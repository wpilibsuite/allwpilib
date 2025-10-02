// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/camera/CameraProvider.h"

#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>

#include "glass/Storage.h"

using namespace glass;


static void DisplayCameraModelTable(Storage& root, int kinds) {
  if (!ImGui::BeginTable("cameras", 4,
                         ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV |
                             ImGuiTableFlags_SizingFixedFit)) {
    return;
  }

  ImGui::TableSetupColumn("Name");
  ImGui::TableSetupColumn("FPS", ImGuiTableColumnFlags_WidthFixed,
                          ImGui::GetFontSize() * 5);
  ImGui::TableSetupColumn("Data Rate", ImGuiTableColumnFlags_WidthFixed,
                          ImGui::GetFontSize() * 5);
  ImGui::TableSetupColumn("Actions");
  ImGui::TableHeadersRow();

  std::string toDelete;
  for (auto&& kv : root.GetChildren()) {
    CameraModel* model = kv.value().GetData<CameraModel>();
    if (!model) {
      continue;
    }
    if ((model->GetKind() & kinds) == 0) {
      continue;
    }

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Selectable(model->GetId().c_str());
    if (ImGui::BeginDragDropSource()) {
      ImGui::SetDragDropPayload("Camera", model->GetId().data(),
                                model->GetId().size());
      ImGui::Text("Camera: %s", model->GetId().c_str());
      ImGui::EndDragDropSource();
    }

    ImGui::TableNextColumn();
    char buf[64];
    VideoModeToString(buf, sizeof(buf), model->GetVideoMode());
    ImGui::TextUnformatted(buf);
    ImGui::SameLine();
    if (ImGui::SmallButton(">")) {
      ImGui::OpenPopup("mode");
    }
    if (ImGui::BeginPopup("mode")) {
      if (EditMode(model)) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(model->Exists() ? "Yes" : "No");

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(model->GetSource().IsEnabled() ? "Yes" : "No");

    ImGui::TableNextColumn();
    ImGui::Text("%2.1f FPS", model->GetActualFPS());

    ImGui::TableNextColumn();
    wpi::format_to_n_c_str(buf, sizeof(buf), "{:.1f} Mbps",
                           model->GetActualDataRate() * 8 / 1000000);
    ImGui::TextUnformatted(buf);

    ImGui::TableNextColumn();
    if (ImGui::SmallButton(model->GetSource().IsEnabled() ? "Stop" : "Start")) {
      if (model->Exists()) {
        model->Stop();
      } else {
        model->Start();
      }
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Delete")) {
      toDelete = kv.key();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Properties")) {
      ImGui::OpenPopup("properties");
    }
    if (ImGui::BeginPopup("properties")) {
      EditProperties(model->GetSource());
      ImGui::EndPopup();
    }
  }
  if (!toDelete.empty()) {
    root.Erase(toDelete);
  }

  ImGui::EndTable();
}

void DisplayCameraList() {
  if (glass::imm::BeginWindow(gCameraListWindow)) {
    DisplayCameraModelTable(cameras);
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

CameraProvider::CameraProvider(Storage& storage)
    : CameraProvider{storage, nt::NetworkTableInstance::GetDefault()} {}

CameraProvider::CameraProvider(Storage& storage,
                                   nt::NetworkTableInstance inst)
    : WindowManager{storage}, m_inst{inst}, m_poller{inst} {
  m_listener =
      m_poller.AddListener({{"/CameraPublisher/"}},
                           nt::EventFlags::kTopic | nt::EventFlags::kValueAll);
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
