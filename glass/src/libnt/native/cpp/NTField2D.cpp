// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTField2D.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <networktables/DoubleArrayTopic.h>
#include <networktables/MultiSubscriber.h>
#include <ntcore_cpp.h>
#include <wpi/Endian.h>
#include <wpi/MathExtras.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>

using namespace glass;

class NTField2DModel::ObjectModel : public FieldObjectModel {
 public:
  ObjectModel(std::string_view name, nt::DoubleArrayTopic topic)
      : m_name{name}, m_topic{topic} {}

  const char* GetName() const override { return m_name.c_str(); }
  nt::DoubleArrayTopic GetTopic() const { return m_topic; }

  void NTUpdate(const nt::Value& value);

  void Update() override {}
  bool Exists() override { return m_topic.Exists(); }
  bool IsReadOnly() override { return false; }

  std::span<const frc::Pose2d> GetPoses() override { return m_poses; }
  void SetPoses(std::span<const frc::Pose2d> poses) override;
  void SetPose(size_t i, frc::Pose2d pose) override;
  void SetPosition(size_t i, frc::Translation2d pos) override;
  void SetRotation(size_t i, frc::Rotation2d rot) override;

 private:
  void UpdateNT();

  std::string m_name;
  nt::DoubleArrayTopic m_topic;
  nt::DoubleArrayPublisher m_pub;

  std::vector<frc::Pose2d> m_poses;
};

void NTField2DModel::ObjectModel::NTUpdate(const nt::Value& value) {
  if (value.IsDoubleArray()) {
    auto arr = value.GetDoubleArray();
    auto size = arr.size();
    if ((size % 3) != 0) {
      return;
    }
    m_poses.resize(size / 3);
    for (size_t i = 0; i < size / 3; ++i) {
      m_poses[i] = frc::Pose2d{
          units::meter_t{arr[i * 3 + 0]}, units::meter_t{arr[i * 3 + 1]},
          frc::Rotation2d{units::degree_t{arr[i * 3 + 2]}}};
    }
  }
}

void NTField2DModel::ObjectModel::UpdateNT() {
  wpi::SmallVector<double, 9> arr;
  for (auto&& pose : m_poses) {
    auto& translation = pose.Translation();
    arr.push_back(translation.X().value());
    arr.push_back(translation.Y().value());
    arr.push_back(pose.Rotation().Degrees().value());
  }
  if (!m_pub) {
    m_pub = m_topic.Publish();
  }
  m_pub.Set(arr);
}

void NTField2DModel::ObjectModel::SetPoses(std::span<const frc::Pose2d> poses) {
  m_poses.assign(poses.begin(), poses.end());
  UpdateNT();
}

void NTField2DModel::ObjectModel::SetPose(size_t i, frc::Pose2d pose) {
  if (i < m_poses.size()) {
    m_poses[i] = pose;
    UpdateNT();
  }
}

void NTField2DModel::ObjectModel::SetPosition(size_t i,
                                              frc::Translation2d pos) {
  if (i < m_poses.size()) {
    m_poses[i] = frc::Pose2d{pos, m_poses[i].Rotation()};
    UpdateNT();
  }
}

void NTField2DModel::ObjectModel::SetRotation(size_t i, frc::Rotation2d rot) {
  if (i < m_poses.size()) {
    m_poses[i] = frc::Pose2d{m_poses[i].Translation(), rot};
    UpdateNT();
  }
}

NTField2DModel::NTField2DModel(std::string_view path)
    : NTField2DModel{nt::NetworkTableInstance::GetDefault(), path} {}

NTField2DModel::NTField2DModel(nt::NetworkTableInstance inst,
                               std::string_view path)
    : m_path{fmt::format("{}/", path)},
      m_inst{inst},
      m_tableSub{inst, {{m_path}}, {.periodic = 0.05, .sendAll = true}},
      m_nameTopic{inst.GetTopic(fmt::format("{}/.name", path))},
      m_poller{inst} {
  m_poller.AddListener(m_tableSub, nt::EventFlags::kTopic |
                                       nt::EventFlags::kValueAll |
                                       nt::EventFlags::kImmediate);
}

NTField2DModel::~NTField2DModel() = default;

void NTField2DModel::Update() {
  for (auto&& event : m_poller.ReadQueue()) {
    if (auto info = event.GetTopicInfo()) {
      // handle publish/unpublish
      auto name = wpi::remove_prefix(info->name, m_path).value_or("");
      if (name.empty() || name[0] == '.') {
        continue;
      }
      auto [it, match] = Find(info->name);
      if (event.flags & nt::EventFlags::kUnpublish) {
        if (match) {
          m_objects.erase(it);
        }
        continue;
      } else if (event.flags & nt::EventFlags::kPublish) {
        if (!match) {
          it = m_objects.emplace(
              it, std::make_unique<ObjectModel>(
                      info->name, nt::DoubleArrayTopic{info->topic}));
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
  auto fullName = fmt::format("{}{}", m_path, name);
  auto [it, match] = Find(fullName);
  if (!match) {
    it = m_objects.emplace(it,
                           std::make_unique<ObjectModel>(
                               fullName, m_inst.GetDoubleArrayTopic(fullName)));
  }
  return it->get();
}

void NTField2DModel::RemoveFieldObject(std::string_view name) {
  auto [it, match] = Find(fmt::format("{}{}", m_path, name));
  if (match) {
    m_objects.erase(it);
  }
}

void NTField2DModel::ForEachFieldObject(
    wpi::function_ref<void(FieldObjectModel& model, std::string_view name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      if (auto name = wpi::remove_prefix(obj->GetName(), m_path)) {
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
