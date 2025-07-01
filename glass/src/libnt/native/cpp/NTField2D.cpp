// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/networktables/NTField2D.hpp"

#include <algorithm>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "wpi/nt/MultiSubscriber.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/struct/Struct.hpp"

using namespace wpi::glass;

namespace {
constexpr std::string_view POSE2D_ARRAY_TYPE = "struct:Pose2d[]";

bool IsPose2dArrayTopic(const wpi::nt::TopicInfo& info) {
  return info.type == NT_RAW && info.type_str == POSE2D_ARRAY_TYPE;
}
}  // namespace

class NTField2DModel::ObjectModel : public FieldObjectModel {
 public:
  ObjectModel(std::string_view name,
              wpi::nt::StructArrayTopic<wpi::math::Pose2d> topic)
      : m_name{name}, m_topic{topic} {}

  const char* GetName() const override { return m_name.c_str(); }
  wpi::nt::StructArrayTopic<wpi::math::Pose2d> GetTopic() const {
    return m_topic;
  }

  void NTUpdate(const wpi::nt::Value& value);

  void Update() override {}
  bool Exists() override { return m_topic.Exists(); }
  bool IsReadOnly() override { return false; }

  std::span<const wpi::math::Pose2d> GetPoses() override { return m_poses; }
  void SetPoses(std::span<const wpi::math::Pose2d> poses) override;
  void SetPose(size_t i, wpi::math::Pose2d pose) override;
  void SetPosition(size_t i, wpi::math::Translation2d pos) override;
  void SetRotation(size_t i, wpi::math::Rotation2d rot) override;

 private:
  void UpdateNT();

  std::string m_name;
  wpi::nt::StructArrayTopic<wpi::math::Pose2d> m_topic;
  wpi::nt::StructArrayPublisher<wpi::math::Pose2d> m_pub;

  std::vector<wpi::math::Pose2d> m_poses;
};

void NTField2DModel::ObjectModel::NTUpdate(const wpi::nt::Value& value) {
  if (!value.IsRaw()) {
    return;
  }

  auto raw = value.GetRaw();
  constexpr size_t kPoseSize = wpi::util::GetStructSize<wpi::math::Pose2d>();
  if ((raw.size() % kPoseSize) != 0) {
    return;
  }

  m_poses.resize(raw.size() / kPoseSize);
  for (size_t i = 0; i < m_poses.size(); ++i) {
    m_poses[i] = wpi::util::UnpackStruct<wpi::math::Pose2d>(
        raw.subspan(i * kPoseSize, kPoseSize));
  }
}

void NTField2DModel::ObjectModel::UpdateNT() {
  if (!m_pub) {
    m_pub = m_topic.Publish();
  }
  m_pub.Set(m_poses);
}

void NTField2DModel::ObjectModel::SetPoses(
    std::span<const wpi::math::Pose2d> poses) {
  m_poses.assign(poses.begin(), poses.end());
  UpdateNT();
}

void NTField2DModel::ObjectModel::SetPose(size_t i, wpi::math::Pose2d pose) {
  if (i < m_poses.size()) {
    m_poses[i] = pose;
    UpdateNT();
  }
}

void NTField2DModel::ObjectModel::SetPosition(size_t i,
                                              wpi::math::Translation2d pos) {
  if (i < m_poses.size()) {
    m_poses[i] = wpi::math::Pose2d{pos, m_poses[i].Rotation()};
    UpdateNT();
  }
}

void NTField2DModel::ObjectModel::SetRotation(size_t i,
                                              wpi::math::Rotation2d rot) {
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
      m_nameTopic{inst.GetTopic(std::format("{}/.name", path))},
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
        if (!IsPose2dArrayTopic(*info)) {
          if (match) {
            m_objects.erase(it);
          }
          continue;
        }
        if (!match) {
          it = m_objects.emplace(
              it, std::make_unique<ObjectModel>(
                      info->name, wpi::nt::StructArrayTopic<wpi::math::Pose2d>{
                                      info->topic}));
        }
      } else if (!match) {
        continue;
      }
    } else if (auto valueData = event.GetValueEventData()) {
      // update values
      // .name
      if (valueData->topic == m_nameTopic.GetHandle()) {
        if (valueData->value && valueData->value.IsString()) {
          m_nameValue = valueData->value.GetString();
        }
        continue;
      }

      auto it =
          std::find_if(m_objects.begin(), m_objects.end(), [&](const auto& e) {
            return e->GetTopic().GetHandle() == valueData->topic;
          });
      if (it != m_objects.end()) {
        (*it)->NTUpdate(valueData->value);
        continue;
      }
    }
  }
}

bool NTField2DModel::Exists() {
  return m_nameTopic.Exists();
}

bool NTField2DModel::IsReadOnly() {
  return false;
}

FieldObjectModel* NTField2DModel::AddFieldObject(std::string_view name) {
  auto fullName = std::format("{}{}", m_path, name);
  auto [it, match] = Find(fullName);
  if (!match) {
    it = m_objects.emplace(
        it,
        std::make_unique<ObjectModel>(
            fullName, m_inst.GetStructArrayTopic<wpi::math::Pose2d>(fullName)));
  }
  return it->get();
}

void NTField2DModel::RemoveFieldObject(std::string_view name) {
  auto [it, match] = Find(std::format("{}{}", m_path, name));
  if (match) {
    m_objects.erase(it);
  }
}

void NTField2DModel::ForEachFieldObject(
    wpi::util::function_ref<void(FieldObjectModel& model,
                                 std::string_view name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      if (auto name = wpi::util::remove_prefix(obj->GetName(), m_path)) {
        func(*obj, *name);
      }
    }
  }
}

std::pair<NTField2DModel::Objects::iterator, bool> NTField2DModel::Find(
    std::string_view fullName) {
  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), fullName,
      [](const auto& e, std::string_view name) { return e->GetName() < name; });
  return {it, it != m_objects.end() && (*it)->GetName() == fullName};
}
