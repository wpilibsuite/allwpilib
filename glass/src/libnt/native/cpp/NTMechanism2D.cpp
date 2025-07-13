// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTMechanism2D.h"

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <imgui.h>
#include <ntcore_cpp.h>
#include <wpi/StringExtras.h>

#include "glass/other/Mechanism2D.h"

using namespace glass;

// Convert "#RRGGBB" hex color to ImU32 color
static void ConvertColor(std::string_view in, ImU32* out) {
  if (in.size() != 7 || in[0] != '#') {
    return;
  }
  if (auto v = wpi::parse_integer<ImU32>(wpi::drop_front(in), 16)) {
    ImU32 val = v.value();
    *out = IM_COL32((val >> 16) & 0xff, (val >> 8) & 0xff, val & 0xff, 255);
  }
}

void NTMechanism2DModel::NTMechanismGroupImpl::ForEachObject(
    wpi::function_ref<void(MechanismObjectModel& model)> func) {
  for (auto&& obj : m_objects) {
    func(*obj);
  }
}

void NTMechanism2DModel::NTMechanismGroupImpl::NTUpdate(const nt::Event& event,
                                                        std::string_view name) {
  if (name.empty()) {
    return;
  }
  std::string_view childName;
  std::tie(name, childName) = wpi::split(name, '/');
  if (childName.empty()) {
    return;
  }

  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), name,
      [](const auto& e, std::string_view name) { return e->GetName() < name; });
  bool match = it != m_objects.end() && (*it)->GetName() == name;

  if (event.GetTopicInfo()) {
    if (event.flags & nt::EventFlags::kPublish) {
      if (!match) {
        it = m_objects.emplace(
            it, std::make_unique<NTMechanismObjectModel>(
                    m_inst, fmt::format("{}/{}", m_path, name), name));
        match = true;
      }
    }

    if (match) {
      if ((*it)->NTUpdate(event, childName)) {
        m_objects.erase(it);
      }
    }
  } else if (event.GetValueEventData()) {
    if (match) {
      (*it)->NTUpdate(event, childName);
    }
  }
}

bool NTMechanism2DModel::NTMechanismObjectModel::NTUpdate(
    const nt::Event& event, std::string_view childName) {
  if (auto info = event.GetTopicInfo()) {
    if (info->topic == m_typeTopic.GetHandle()) {
      if (event.flags & nt::EventFlags::kUnpublish) {
        return true;
      }
    } else if (info->topic != m_colorTopic.GetHandle() &&
               info->topic != m_weightTopic.GetHandle() &&
               info->topic != m_angleTopic.GetHandle() &&
               info->topic != m_lengthTopic.GetHandle()) {
      m_group.NTUpdate(event, childName);
    }
  } else if (auto valueData = event.GetValueEventData()) {
    if (valueData->topic == m_typeTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsString()) {
        m_typeValue = valueData->value.GetString();
      }
    } else if (valueData->topic == m_colorTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsString()) {
        ConvertColor(valueData->value.GetString(), &m_colorValue);
      }
    } else if (valueData->topic == m_weightTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsDouble()) {
        m_weightValue = valueData->value.GetDouble();
      }
    } else if (valueData->topic == m_angleTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsDouble()) {
        m_angleValue = units::degree_t{valueData->value.GetDouble()};
      }
    } else if (valueData->topic == m_lengthTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsDouble()) {
        m_lengthValue = units::meter_t{valueData->value.GetDouble()};
      }
    } else {
      m_group.NTUpdate(event, childName);
    }
  }
  return false;
}

bool NTMechanism2DModel::RootModel::NTUpdate(const nt::Event& event,
                                             std::string_view childName) {
  if (auto info = event.GetTopicInfo()) {
    if (info->topic == m_xTopic.GetHandle() ||
        info->topic == m_yTopic.GetHandle()) {
      if (event.flags & nt::EventFlags::kUnpublish) {
        return true;
      }
    } else {
      m_group.NTUpdate(event, childName);
    }
  } else if (auto valueData = event.GetValueEventData()) {
    if (valueData->topic == m_xTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsDouble()) {
        m_pos = frc::Translation2d{units::meter_t{valueData->value.GetDouble()},
                                   m_pos.Y()};
      }
    } else if (valueData->topic == m_yTopic.GetHandle()) {
      if (valueData->value && valueData->value.IsDouble()) {
        m_pos = frc::Translation2d{
            m_pos.X(), units::meter_t{valueData->value.GetDouble()}};
      }
    } else {
      m_group.NTUpdate(event, childName);
    }
  }
  return false;
}

NTMechanism2DModel::NTMechanism2DModel(std::string_view path)
    : NTMechanism2DModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTMechanism2DModel::NTMechanism2DModel(nt::NetworkTableInstance inst,
                                       std::string_view path)
    : m_inst{inst},
      m_path{fmt::format("{}/", path)},
      m_tableSub{inst, {{m_path}}, {.periodic = 0.05, .sendAll = true}},
      m_nameTopic{m_inst.GetTopic(fmt::format("{}/.name", path))},
      m_dimensionsTopic{m_inst.GetTopic(fmt::format("{}/dims", path))},
      m_bgColorTopic{m_inst.GetTopic(fmt::format("{}/backgroundColor", path))},
      m_poller{m_inst},
      m_dimensionsValue{1_m, 1_m} {
  m_poller.AddListener(m_tableSub, nt::EventFlags::kTopic |
                                       nt::EventFlags::kValueAll |
                                       nt::EventFlags::kImmediate);
}

NTMechanism2DModel::~NTMechanism2DModel() = default;

void NTMechanism2DModel::Update() {
  for (auto&& event : m_poller.ReadQueue()) {
    if (auto info = event.GetTopicInfo()) {
      auto name = wpi::remove_prefix(info->name, m_path).value_or("");
      if (name.empty() || name[0] == '.') {
        continue;
      }
      std::string_view childName;
      std::tie(name, childName) = wpi::split(name, '/');
      if (childName.empty()) {
        continue;
      }

      auto it = std::lower_bound(m_roots.begin(), m_roots.end(), name,
                                 [](const auto& e, std::string_view name) {
                                   return e->GetName() < name;
                                 });
      bool match = it != m_roots.end() && (*it)->GetName() == name;

      if (event.flags & nt::EventFlags::kPublish) {
        if (!match) {
          it = m_roots.emplace(
              it, std::make_unique<RootModel>(
                      m_inst, fmt::format("{}{}", m_path, name), name));
          match = true;
        }
      }
      if (match) {
        if ((*it)->NTUpdate(event, childName)) {
          m_roots.erase(it);
        }
      }
    } else if (auto valueData = event.GetValueEventData()) {
      if (valueData->topic == m_nameTopic.GetHandle()) {
        // .name
        if (valueData->value && valueData->value.IsString()) {
          m_nameValue = valueData->value.GetString();
        }
      } else if (valueData->topic == m_dimensionsTopic.GetHandle()) {
        // dims
        if (valueData->value && valueData->value.IsDoubleArray()) {
          auto arr = valueData->value.GetDoubleArray();
          if (arr.size() == 2) {
            m_dimensionsValue = frc::Translation2d{units::meter_t{arr[0]},
                                                   units::meter_t{arr[1]}};
          }
        }
      } else if (valueData->topic == m_bgColorTopic.GetHandle()) {
        // backgroundColor
        if (valueData->value && valueData->value.IsString()) {
          ConvertColor(valueData->value.GetString(), &m_bgColorValue);
        }
      } else {
        auto fullName = nt::Topic{valueData->topic}.GetName();
        auto name = wpi::remove_prefix(fullName, m_path).value_or("");
        if (name.empty() || name[0] == '.') {
          continue;
        }
        std::string_view childName;
        std::tie(name, childName) = wpi::split(name, '/');
        if (childName.empty()) {
          continue;
        }

        auto it = std::lower_bound(m_roots.begin(), m_roots.end(), name,
                                   [](const auto& e, std::string_view name) {
                                     return e->GetName() < name;
                                   });
        if (it != m_roots.end() && (*it)->GetName() == name) {
          if ((*it)->NTUpdate(event, childName)) {
            m_roots.erase(it);
          }
        }
      }
    }
  }
}

bool NTMechanism2DModel::Exists() {
  return m_nameTopic.Exists();
}

bool NTMechanism2DModel::IsReadOnly() {
  return false;
}

void NTMechanism2DModel::ForEachRoot(
    wpi::function_ref<void(MechanismRootModel& model)> func) {
  for (auto&& obj : m_roots) {
    func(*obj);
  }
}
