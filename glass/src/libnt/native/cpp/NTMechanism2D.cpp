// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTMechanism2D.h"

#include <algorithm>
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

namespace {

class NTMechanismObjectModel;

class NTMechanismGroupImpl final {
 public:
  NTMechanismGroupImpl(nt::NetworkTableInstance inst, std::string_view path,
                       std::string_view name)
      : m_inst{inst}, m_path{path}, m_name{name} {}

  const char* GetName() const { return m_name.c_str(); }
  void ForEachObject(wpi::function_ref<void(MechanismObjectModel& model)> func);

  void NTUpdate(const nt::TopicNotification& event, std::string_view name);
  void NTUpdate(const nt::ValueNotification& event, std::string_view name);

 protected:
  nt::NetworkTableInstance m_inst;
  std::string m_path;
  std::string m_name;
  std::vector<std::unique_ptr<NTMechanismObjectModel>> m_objects;
};

class NTMechanismObjectModel final : public MechanismObjectModel {
 public:
  NTMechanismObjectModel(nt::NetworkTableInstance inst, std::string_view path,
                         std::string_view name)
      : m_group{inst, path, name},
        m_typeTopic{inst.GetTopic(fmt::format("{}/.type", path))},
        m_colorTopic{inst.GetTopic(fmt::format("{}/color", path))},
        m_weightTopic{inst.GetTopic(fmt::format("{}/weight", path))},
        m_angleTopic{inst.GetTopic(fmt::format("{}/angle", path))},
        m_lengthTopic{inst.GetTopic(fmt::format("{}/length", path))} {}

  const char* GetName() const final { return m_group.GetName(); }
  void ForEachObject(
      wpi::function_ref<void(MechanismObjectModel& model)> func) final {
    m_group.ForEachObject(func);
  }

  const char* GetType() const final { return m_typeValue.c_str(); }
  ImU32 GetColor() const final { return m_colorValue; }
  double GetWeight() const final { return m_weightValue; }
  frc::Rotation2d GetAngle() const final { return m_angleValue; }
  units::meter_t GetLength() const final { return m_lengthValue; }

  bool NTUpdate(const nt::TopicNotification& event, std::string_view name);
  void NTUpdate(const nt::ValueNotification& event, std::string_view name);

 private:
  NTMechanismGroupImpl m_group;

  nt::Topic m_typeTopic;
  nt::Topic m_colorTopic;
  nt::Topic m_weightTopic;
  nt::Topic m_angleTopic;
  nt::Topic m_lengthTopic;

  std::string m_typeValue;
  ImU32 m_colorValue = IM_COL32_WHITE;
  double m_weightValue = 1.0;
  frc::Rotation2d m_angleValue;
  units::meter_t m_lengthValue = 0.0_m;
};

}  // namespace

void NTMechanismGroupImpl::ForEachObject(
    wpi::function_ref<void(MechanismObjectModel& model)> func) {
  for (auto&& obj : m_objects) {
    func(*obj);
  }
}

void NTMechanismGroupImpl::NTUpdate(const nt::TopicNotification& event,
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

  if (event.flags & NT_TOPIC_NOTIFY_PUBLISH) {
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
}

void NTMechanismGroupImpl::NTUpdate(const nt::ValueNotification& event,
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
  if (it != m_objects.end() && (*it)->GetName() == name) {
    (*it)->NTUpdate(event, childName);
  }
}

bool NTMechanismObjectModel::NTUpdate(const nt::TopicNotification& event,
                                      std::string_view childName) {
  if (event.info.topic == m_typeTopic.GetHandle()) {
    if (event.flags & NT_TOPIC_NOTIFY_UNPUBLISH) {
      return true;
    }
  } else if (event.info.topic != m_colorTopic.GetHandle() &&
             event.info.topic != m_weightTopic.GetHandle() &&
             event.info.topic != m_angleTopic.GetHandle() &&
             event.info.topic != m_lengthTopic.GetHandle()) {
    m_group.NTUpdate(event, childName);
  }
  return false;
}

void NTMechanismObjectModel::NTUpdate(const nt::ValueNotification& event,
                                      std::string_view childName) {
  if (event.topic == m_typeTopic.GetHandle()) {
    if (event.value && event.value.IsString()) {
      m_typeValue = event.value.GetString();
    }
  } else if (event.topic == m_colorTopic.GetHandle()) {
    if (event.value && event.value.IsString()) {
      ConvertColor(event.value.GetString(), &m_colorValue);
    }
  } else if (event.topic == m_weightTopic.GetHandle()) {
    if (event.value && event.value.IsDouble()) {
      m_weightValue = event.value.GetDouble();
    }
  } else if (event.topic == m_angleTopic.GetHandle()) {
    if (event.value && event.value.IsDouble()) {
      m_angleValue = units::degree_t{event.value.GetDouble()};
    }
  } else if (event.topic == m_lengthTopic.GetHandle()) {
    if (event.value && event.value.IsDouble()) {
      m_lengthValue = units::meter_t{event.value.GetDouble()};
    }
  } else {
    m_group.NTUpdate(event, childName);
  }
}

class NTMechanism2DModel::RootModel final : public MechanismRootModel {
 public:
  RootModel(nt::NetworkTableInstance inst, std::string_view path,
            std::string_view name)
      : m_group{inst, path, name},
        m_xTopic{inst.GetTopic(fmt::format("{}/x", path))},
        m_yTopic{inst.GetTopic(fmt::format("{}/y", path))} {}

  const char* GetName() const final { return m_group.GetName(); }
  void ForEachObject(
      wpi::function_ref<void(MechanismObjectModel& model)> func) final {
    m_group.ForEachObject(func);
  }

  bool NTUpdate(const nt::TopicNotification& event, std::string_view childName);
  void NTUpdate(const nt::ValueNotification& event, std::string_view childName);

  frc::Translation2d GetPosition() const override { return m_pos; };

 private:
  NTMechanismGroupImpl m_group;
  nt::Topic m_xTopic;
  nt::Topic m_yTopic;
  frc::Translation2d m_pos;
};

bool NTMechanism2DModel::RootModel::NTUpdate(const nt::TopicNotification& event,
                                             std::string_view childName) {
  if (event.info.topic == m_xTopic.GetHandle() ||
      event.info.topic == m_yTopic.GetHandle()) {
    if (event.flags & NT_TOPIC_NOTIFY_UNPUBLISH) {
      return true;
    }
  } else {
    m_group.NTUpdate(event, childName);
  }
  return false;
}

void NTMechanism2DModel::RootModel::NTUpdate(const nt::ValueNotification& event,
                                             std::string_view childName) {
  if (event.topic == m_xTopic.GetHandle()) {
    if (event.value && event.value.IsDouble()) {
      m_pos = frc::Translation2d{units::meter_t{event.value.GetDouble()},
                                 m_pos.Y()};
    }
  } else if (event.topic == m_yTopic.GetHandle()) {
    if (event.value && event.value.IsDouble()) {
      m_pos = frc::Translation2d{m_pos.X(),
                                 units::meter_t{event.value.GetDouble()}};
    }
  } else {
    m_group.NTUpdate(event, childName);
  }
}

NTMechanism2DModel::NTMechanism2DModel(std::string_view path)
    : NTMechanism2DModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTMechanism2DModel::NTMechanism2DModel(nt::NetworkTableInstance inst,
                                       std::string_view path)
    : m_inst{inst},
      m_path{fmt::format("{}/", path)},
      m_tableSub{inst,
                 {{m_path}},
                 {{nt::PubSubOption::SendAll(true),
                   nt::PubSubOption::Periodic(0.05)}}},
      m_nameTopic{m_inst.GetTopic(fmt::format("{}/.name", path))},
      m_dimensionsTopic{m_inst.GetTopic(fmt::format("{}/dims", path))},
      m_bgColorTopic{m_inst.GetTopic(fmt::format("{}/backgroundColor", path))},
      m_topicListener{m_inst},
      m_valueListener{m_inst},
      m_dimensionsValue{1_m, 1_m} {
  m_topicListener.Add(m_tableSub, NT_TOPIC_NOTIFY_PUBLISH |
                                      NT_TOPIC_NOTIFY_UNPUBLISH |
                                      NT_TOPIC_NOTIFY_IMMEDIATE);
  m_valueListener.Add(m_tableSub,
                      NT_VALUE_NOTIFY_IMMEDIATE | NT_VALUE_NOTIFY_LOCAL);
}

NTMechanism2DModel::~NTMechanism2DModel() = default;

void NTMechanism2DModel::Update() {
  for (auto&& event : m_topicListener.ReadQueue()) {
    auto name = wpi::drop_front(event.info.name, m_path.size());
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

    if (event.flags & NT_TOPIC_NOTIFY_PUBLISH) {
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
  }

  for (auto&& event : m_valueListener.ReadQueue()) {
    // .name
    if (event.topic == m_nameTopic.GetHandle()) {
      if (event.value && event.value.IsString()) {
        m_nameValue = event.value.GetString();
      }
      continue;
    }

    // dims
    if (event.topic == m_dimensionsTopic.GetHandle()) {
      if (event.value && event.value.IsDoubleArray()) {
        auto arr = event.value.GetDoubleArray();
        if (arr.size() == 2) {
          m_dimensionsValue = frc::Translation2d{units::meter_t{arr[0]},
                                                 units::meter_t{arr[1]}};
        }
      }
    }

    // backgroundColor
    if (event.topic == m_bgColorTopic.GetHandle()) {
      if (event.value && event.value.IsString()) {
        ConvertColor(event.value.GetString(), &m_bgColorValue);
      }
    }
  }
}

bool NTMechanism2DModel::Exists() {
  return m_inst.IsConnected() && m_nameTopic.Exists();
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
