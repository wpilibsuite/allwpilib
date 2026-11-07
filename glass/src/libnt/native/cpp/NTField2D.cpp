// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTField2D.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "wpi/glass/networktables/NTTunableTopic.hpp"
#include "wpi/nt/MultiSubscriber.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/struct/Struct.hpp"

using namespace wpi::glass;

namespace {
constexpr std::string_view POSE2D_TYPE = "struct:Pose2d";
constexpr std::string_view POSE2D_ARRAY_TYPE = "struct:Pose2d[]";

enum class TopicKind { POSE, POSE_ARRAY };

wpi::util::json GetMutableProperties() {
  auto properties = wpi::util::json::object();
  properties["mutable"] = true;
  return properties;
}

std::optional<TopicKind> GetTopicKind(std::string_view typeStr) {
  if (typeStr == POSE2D_TYPE) {
    return TopicKind::POSE;
  }
  if (typeStr == POSE2D_ARRAY_TYPE) {
    return TopicKind::POSE_ARRAY;
  }
  return std::nullopt;
}

std::optional<TopicKind> GetTopicKind(const wpi::nt::TopicInfo& info) {
  if (info.type != NT_RAW) {
    return std::nullopt;
  }
  return GetTopicKind(info.type_str);
}
}  // namespace

class NTField2DModel::ObjectModel : public FieldObjectModel {
 public:
  ObjectModel(std::string_view name, std::string_view topicName,
              wpi::nt::NetworkTableInstance inst, TopicKind kind,
              bool allowLocalPublish = false)
      : m_name{name},
        m_topicName{topicName},
        m_kind{kind},
        m_allowLocalPublish{allowLocalPublish} {
    if (m_kind == TopicKind::POSE) {
      m_poseTopic = inst.GetStructTopic<wpi::math::Pose2d>(m_topicName);
    } else {
      m_poseArrayTopic =
          inst.GetStructArrayTopic<wpi::math::Pose2d>(m_topicName);
    }
  }

  const char* GetName() const override { return m_name.c_str(); }
  const std::string& GetTopicName() const { return m_topicName; }
  NT_Topic GetTopicHandle() const {
    return m_kind == TopicKind::POSE ? m_poseTopic.GetHandle()
                                     : m_poseArrayTopic.GetHandle();
  }
  NT_Listener GetListener() const { return m_listener; }
  void SetListener(NT_Listener listener) { m_listener = listener; }

  void NTUpdate(const wpi::nt::Value& value);

  void Update() override {}
  bool Exists() override {
    return m_kind == TopicKind::POSE ? m_poseTopic.Exists()
                                     : m_poseArrayTopic.Exists();
  }
  bool IsReadOnly() override {
    return m_kind == TopicKind::POSE ? IsReadOnly(m_poseTopic)
                                     : IsReadOnly(m_poseArrayTopic);
  }

  std::span<const wpi::math::Pose2d> GetPoses() override { return m_poses; }
  void SetPoses(std::span<const wpi::math::Pose2d> poses) override;
  void SetPose(size_t i, wpi::math::Pose2d pose) override;
  void SetPosition(size_t i, wpi::math::Translation2d pos) override;
  void SetRotation(size_t i, wpi::math::Rotation2d rot) override;

 private:
  bool IsReadOnly(const wpi::nt::Topic& topic) const {
    if (!topic.Exists()) {
      return !m_allowLocalPublish;
    }
    return !IsTunableTopicMutable(topic);
  }

  void UpdateNT();

  std::string m_name;
  std::string m_topicName;
  TopicKind m_kind;
  wpi::nt::StructTopic<wpi::math::Pose2d> m_poseTopic;
  wpi::nt::StructPublisher<wpi::math::Pose2d> m_posePub;
  wpi::nt::StructArrayTopic<wpi::math::Pose2d> m_poseArrayTopic;
  wpi::nt::StructArrayPublisher<wpi::math::Pose2d> m_poseArrayPub;
  NT_Listener m_listener = 0;
  bool m_allowLocalPublish = false;

  std::vector<wpi::math::Pose2d> m_poses;
};

void NTField2DModel::ObjectModel::NTUpdate(const wpi::nt::Value& value) {
  if (!value.IsRaw()) {
    return;
  }

  auto raw = value.GetRaw();
  constexpr size_t POSE_SIZE = wpi::util::GetStructSize<wpi::math::Pose2d>();
  if (m_kind == TopicKind::POSE) {
    if (raw.size() != POSE_SIZE) {
      return;
    }
    m_poses.resize(1);
    m_poses[0] = wpi::util::UnpackStruct<wpi::math::Pose2d>(raw);
    return;
  }

  if ((raw.size() % POSE_SIZE) != 0) {
    return;
  }

  m_poses.resize(raw.size() / POSE_SIZE);
  for (size_t i = 0; i < m_poses.size(); ++i) {
    m_poses[i] = wpi::util::UnpackStruct<wpi::math::Pose2d>(
        raw.subspan(i * POSE_SIZE, POSE_SIZE));
  }
}

void NTField2DModel::ObjectModel::UpdateNT() {
  if (IsReadOnly()) {
    return;
  }
  if (m_kind == TopicKind::POSE) {
    if (m_poses.empty()) {
      return;
    }
    if (!m_posePub) {
      m_posePub = m_poseTopic.PublishEx(GetMutableProperties());
    }
    m_posePub.Set(m_poses[0]);
    return;
  }

  if (!m_poseArrayPub) {
    m_poseArrayPub = m_poseArrayTopic.PublishEx(GetMutableProperties());
  }
  m_poseArrayPub.Set(m_poses);
}

void NTField2DModel::ObjectModel::SetPoses(
    std::span<const wpi::math::Pose2d> poses) {
  if (IsReadOnly()) {
    return;
  }
  if (m_kind == TopicKind::POSE) {
    if (poses.empty()) {
      return;
    }
    m_poses.assign(poses.begin(), poses.begin() + 1);
  } else {
    m_poses.assign(poses.begin(), poses.end());
  }
  UpdateNT();
}

void NTField2DModel::ObjectModel::SetPose(size_t i, wpi::math::Pose2d pose) {
  if (IsReadOnly()) {
    return;
  }
  if (i < m_poses.size()) {
    m_poses[i] = pose;
    UpdateNT();
  }
}

void NTField2DModel::ObjectModel::SetPosition(size_t i,
                                              wpi::math::Translation2d pos) {
  if (IsReadOnly()) {
    return;
  }
  if (i < m_poses.size()) {
    m_poses[i] = wpi::math::Pose2d{pos, m_poses[i].Rotation()};
    UpdateNT();
  }
}

void NTField2DModel::ObjectModel::SetRotation(size_t i,
                                              wpi::math::Rotation2d rot) {
  if (IsReadOnly()) {
    return;
  }
  if (i < m_poses.size()) {
    m_poses[i] = wpi::math::Pose2d{m_poses[i].Translation(), rot};
    UpdateNT();
  }
}

NTField2DModel::NTField2DModel(std::string_view path)
    : NTField2DModel{wpi::nt::NetworkTableInstance::GetDefault(), path} {}

NTField2DModel::NTField2DModel(wpi::nt::NetworkTableInstance inst,
                               std::string_view path)
    : m_path{std::format("{}/", path)},
      m_inst{inst},
      m_tableSub{inst, {{m_path}}, {.periodic = 0.05, .sendAll = true}},
      m_typeTopic{inst.GetTopic(std::format("{}/.type", path))},
      m_poller{inst} {
  m_poller.AddListener(m_tableSub, wpi::nt::EventFlags::TOPIC |
                                       wpi::nt::EventFlags::VALUE_ALL |
                                       wpi::nt::EventFlags::IMMEDIATE);
}

NTField2DModel::~NTField2DModel() = default;

void NTField2DModel::Update() {
  for (auto&& event : m_poller.ReadQueue()) {
    if (auto info = event.GetTopicInfo()) {
      // handle publish/unpublish
      auto name = wpi::util::remove_prefix(info->name, m_path).value_or("");
      if (name.empty() || name[0] == '.') {
        continue;
      }
      auto [it, match] = Find(info->name);
      if (event.flags & wpi::nt::EventFlags::UNPUBLISH) {
        if (match) {
          m_objects.erase(it);
        }
        continue;
      } else if (event.flags & wpi::nt::EventFlags::PUBLISH) {
        auto kind = GetTopicKind(*info);
        if (!kind) {
          if (match) {
            m_objects.erase(it);
          }
          continue;
        }
        if (!match) {
          it = m_objects.emplace(it, std::make_unique<ObjectModel>(
                                         name, info->name, m_inst, *kind));
        }
      } else if (!match) {
        continue;
      }
    } else if (auto valueData = event.GetValueEventData()) {
      // update values
      auto it =
          std::find_if(m_objects.begin(), m_objects.end(), [&](const auto& e) {
            return e->GetTopicHandle() == valueData->topic;
          });
      if (it != m_objects.end()) {
        (*it)->NTUpdate(valueData->value);
        continue;
      }
    }
  }
}

bool NTField2DModel::Exists() {
  return m_typeTopic.Exists();
}

bool NTField2DModel::IsReadOnly() {
  return !IsTunableTopicMutable(m_typeTopic);
}

FieldObjectModel* NTField2DModel::AddFieldObject(std::string_view name) {
  auto fullName = std::format("{}{}", m_path, name);
  auto [it, match] = Find(fullName);
  if (!match) {
    it = m_objects.emplace(
        it, std::make_unique<ObjectModel>(
                name, fullName, m_inst, TopicKind::POSE_ARRAY, !IsReadOnly()));
  }
  return it->get();
}

FieldObjectModel* NTField2DModel::AddFieldObject(std::string_view name,
                                                 std::string_view source,
                                                 std::string_view type) {
  auto kind = GetTopicKind(type);
  if (!kind) {
    return nullptr;
  }

  auto [it, match] = Find(source);
  if (!match) {
    it = m_objects.emplace(
        it, std::make_unique<ObjectModel>(name, source, m_inst, *kind));
    (*it)->SetListener(m_poller.AddListener(
        m_inst.GetTopic(source), wpi::nt::EventFlags::TOPIC |
                                     wpi::nt::EventFlags::VALUE_ALL |
                                     wpi::nt::EventFlags::IMMEDIATE));
  }
  return it->get();
}

void NTField2DModel::RemoveFieldObject(std::string_view name) {
  auto [it, match] = Find(std::format("{}{}", m_path, name));
  if (match && IsReadOnly()) {
    return;
  }
  if (!match) {
    it = std::find_if(m_objects.begin(), m_objects.end(),
                      [&](const auto& obj) { return obj->GetName() == name; });
    match = it != m_objects.end();
  }
  if (match) {
    if ((*it)->GetListener() != 0) {
      m_poller.RemoveListener((*it)->GetListener());
    }
    m_objects.erase(it);
  }
}

void NTField2DModel::ForEachFieldObject(
    wpi::util::function_ref<void(FieldObjectModel& model,
                                 std::string_view name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      func(*obj, obj->GetName());
    }
  }
}

std::pair<NTField2DModel::Objects::iterator, bool> NTField2DModel::Find(
    std::string_view fullName) {
  auto it = std::lower_bound(m_objects.begin(), m_objects.end(), fullName,
                             [](const auto& e, std::string_view name) {
                               return e->GetTopicName() < name;
                             });
  return {it, it != m_objects.end() && (*it)->GetTopicName() == fullName};
}
