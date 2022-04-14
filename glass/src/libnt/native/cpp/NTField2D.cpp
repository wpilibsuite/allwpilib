// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTField2D.h"

#include <algorithm>
#include <vector>

#include <fmt/format.h>
#include <ntcore_cpp.h>
#include <wpi/Endian.h>
#include <wpi/MathExtras.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>

using namespace glass;

class NTField2DModel::ObjectModel : public FieldObjectModel {
 public:
  ObjectModel(std::string_view name, NT_Entry entry)
      : m_name{name}, m_entry{entry} {}

  const char* GetName() const override { return m_name.c_str(); }
  NT_Entry GetEntry() const { return m_entry; }

  void NTUpdate(const nt::Value& value);

  void Update() override {
    if (auto value = nt::GetEntryValue(m_entry)) {
      NTUpdate(*value);
    }
  }
  bool Exists() override { return nt::GetEntryType(m_entry) != NT_UNASSIGNED; }
  bool IsReadOnly() override { return false; }

  wpi::span<const frc::Pose2d> GetPoses() override { return m_poses; }
  void SetPoses(wpi::span<const frc::Pose2d> poses) override;
  void SetPose(size_t i, frc::Pose2d pose) override;
  void SetPosition(size_t i, frc::Translation2d pos) override;
  void SetRotation(size_t i, frc::Rotation2d rot) override;

 private:
  void UpdateNT();

  std::string m_name;
  NT_Entry m_entry;

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
  } else if (value.IsRaw()) {
    // treat it simply as an array of doubles
    std::string_view data = value.GetRaw();

    // must be triples of doubles
    auto size = data.size();
    if ((size % (3 * 8)) != 0) {
      return;
    }
    m_poses.resize(size / (3 * 8));
    const char* p = data.data();
    for (size_t i = 0; i < size / (3 * 8); ++i) {
      double x = wpi::BitsToDouble(
          wpi::support::endian::readNext<uint64_t, wpi::support::big,
                                         wpi::support::unaligned>(p));
      double y = wpi::BitsToDouble(
          wpi::support::endian::readNext<uint64_t, wpi::support::big,
                                         wpi::support::unaligned>(p));
      double rot = wpi::BitsToDouble(
          wpi::support::endian::readNext<uint64_t, wpi::support::big,
                                         wpi::support::unaligned>(p));
      m_poses[i] = frc::Pose2d{units::meter_t{x}, units::meter_t{y},
                               frc::Rotation2d{units::degree_t{rot}}};
    }
  }
}

void NTField2DModel::ObjectModel::UpdateNT() {
  if (m_poses.size() < (255 / 3)) {
    wpi::SmallVector<double, 9> arr;
    for (auto&& pose : m_poses) {
      auto& translation = pose.Translation();
      arr.push_back(translation.X().value());
      arr.push_back(translation.Y().value());
      arr.push_back(pose.Rotation().Degrees().value());
    }
    nt::SetEntryTypeValue(m_entry, nt::Value::MakeDoubleArray(arr));
  } else {
    // send as raw array of doubles if too big for NT array
    std::vector<char> arr;
    arr.resize(m_poses.size() * 3 * 8);
    char* p = arr.data();
    for (auto&& pose : m_poses) {
      auto& translation = pose.Translation();
      wpi::support::endian::write64be(
          p, wpi::DoubleToBits(translation.X().value()));
      p += 8;
      wpi::support::endian::write64be(
          p, wpi::DoubleToBits(translation.Y().value()));
      p += 8;
      wpi::support::endian::write64be(
          p, wpi::DoubleToBits(pose.Rotation().Degrees().value()));
      p += 8;
    }
    nt::SetEntryTypeValue(m_entry,
                          nt::Value::MakeRaw({arr.data(), arr.size()}));
  }
}

void NTField2DModel::ObjectModel::SetPoses(wpi::span<const frc::Pose2d> poses) {
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
    : NTField2DModel{nt::GetDefaultInstance(), path} {}

NTField2DModel::NTField2DModel(NT_Inst inst, std::string_view path)
    : m_nt{inst},
      m_path{fmt::format("{}/", path)},
      m_name{m_nt.GetEntry(fmt::format("{}/.name", path))} {
  m_nt.AddListener(m_path, NT_NOTIFY_LOCAL | NT_NOTIFY_NEW | NT_NOTIFY_DELETE |
                               NT_NOTIFY_UPDATE | NT_NOTIFY_IMMEDIATE);
}

NTField2DModel::~NTField2DModel() = default;

void NTField2DModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    // .name
    if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
      continue;
    }

    // common case: update of existing entry; search by entry
    if (event.flags & NT_NOTIFY_UPDATE) {
      auto it = std::find_if(
          m_objects.begin(), m_objects.end(),
          [&](const auto& e) { return e->GetEntry() == event.entry; });
      if (it != m_objects.end()) {
        (*it)->NTUpdate(*event.value);
        continue;
      }
    }

    // handle create/delete
    std::string_view name = event.name;
    if (wpi::starts_with(name, m_path)) {
      name.remove_prefix(m_path.size());
      if (name.empty() || name[0] == '.') {
        continue;
      }
      auto [it, match] = Find(event.name);
      if (event.flags & NT_NOTIFY_DELETE) {
        if (match) {
          m_objects.erase(it);
        }
        continue;
      } else if (event.flags & NT_NOTIFY_NEW) {
        if (!match) {
          it = m_objects.emplace(
              it, std::make_unique<ObjectModel>(event.name, event.entry));
        }
      } else if (!match) {
        continue;
      }
      if (event.flags & (NT_NOTIFY_NEW | NT_NOTIFY_UPDATE)) {
        (*it)->NTUpdate(*event.value);
      }
    }
  }
}

bool NTField2DModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_name) != NT_UNASSIGNED;
}

bool NTField2DModel::IsReadOnly() {
  return false;
}

FieldObjectModel* NTField2DModel::AddFieldObject(std::string_view name) {
  auto fullName = fmt::format("{}{}", m_path, name);
  auto [it, match] = Find(fullName);
  if (!match) {
    it = m_objects.emplace(
        it, std::make_unique<ObjectModel>(fullName, m_nt.GetEntry(fullName)));
  }
  return it->get();
}

void NTField2DModel::RemoveFieldObject(std::string_view name) {
  auto [it, match] = Find(fmt::format("{}{}", m_path, name));
  if (match) {
    nt::DeleteEntry((*it)->GetEntry());
    m_objects.erase(it);
  }
}

void NTField2DModel::ForEachFieldObject(
    wpi::function_ref<void(FieldObjectModel& model, std::string_view name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      func(*obj, wpi::drop_front(obj->GetName(), m_path.size()));
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
