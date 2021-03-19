// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTField2D.h"

#include <algorithm>
#include <vector>

#include <ntcore_cpp.h>
#include <wpi/Endian.h>
#include <wpi/MathExtras.h>
#include <wpi/SmallVector.h>

using namespace glass;

class NTField2DModel::ObjectModel : public FieldObjectModel {
 public:
  ObjectModel(wpi::StringRef name, NT_Entry entry)
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

  wpi::ArrayRef<frc::Pose2d> GetPoses() override { return m_poses; }
  void SetPoses(wpi::ArrayRef<frc::Pose2d> poses) override;
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
    wpi::StringRef data = value.GetRaw();

    // must be triples of doubles
    auto size = data.size();
    if ((size % (3 * 8)) != 0) {
      return;
    }
    m_poses.resize(size / (3 * 8));
    const char* p = data.begin();
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
      arr.push_back(translation.X().to<double>());
      arr.push_back(translation.Y().to<double>());
      arr.push_back(pose.Rotation().Degrees().to<double>());
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
          p, wpi::DoubleToBits(translation.X().to<double>()));
      p += 8;
      wpi::support::endian::write64be(
          p, wpi::DoubleToBits(translation.Y().to<double>()));
      p += 8;
      wpi::support::endian::write64be(
          p, wpi::DoubleToBits(pose.Rotation().Degrees().to<double>()));
      p += 8;
    }
    nt::SetEntryTypeValue(
        m_entry, nt::Value::MakeRaw(wpi::StringRef{arr.data(), arr.size()}));
  }
}

void NTField2DModel::ObjectModel::SetPoses(wpi::ArrayRef<frc::Pose2d> poses) {
  m_poses = poses;
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

NTField2DModel::NTField2DModel(wpi::StringRef path)
    : NTField2DModel{nt::GetDefaultInstance(), path} {}

NTField2DModel::NTField2DModel(NT_Inst inst, wpi::StringRef path)
    : m_nt{inst},
      m_path{(path + "/").str()},
      m_name{m_nt.GetEntry(path + "/.name")} {
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
    if (wpi::StringRef{event.name}.startswith(m_path)) {
      auto name = wpi::StringRef{event.name}.drop_front(m_path.size());
      if (name.empty() || name[0] == '.') {
        continue;
      }
      auto it = std::lower_bound(m_objects.begin(), m_objects.end(), event.name,
                                 [](const auto& e, wpi::StringRef name) {
                                   return e->GetName() < name;
                                 });
      bool match = (it != m_objects.end() && (*it)->GetName() == event.name);
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

FieldObjectModel* NTField2DModel::AddFieldObject(wpi::StringRef name) {
  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), name,
      [](const auto& e, wpi::StringRef name) { return e->GetName() < name; });
  bool match = (it != m_objects.end() && (*it)->GetName() == name);
  if (!match) {
    it = m_objects.emplace(
        it, std::make_unique<ObjectModel>(name, m_nt.GetEntry(name)));
  }
  return it->get();
}

void NTField2DModel::RemoveFieldObject(wpi::StringRef name) {
  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), name,
      [](const auto& e, wpi::StringRef name) { return e->GetName() < name; });
  bool match = (it != m_objects.end() && (*it)->GetName() == name);
  if (!match) {
    return;
  }
  nt::DeleteEntry((*it)->GetEntry());
  m_objects.erase(it);
}

void NTField2DModel::ForEachFieldObject(
    wpi::function_ref<void(FieldObjectModel& model, wpi::StringRef name)>
        func) {
  for (auto&& obj : m_objects) {
    if (obj->Exists()) {
      func(*obj, wpi::StringRef{obj->GetName()}.drop_front(m_path.size()));
    }
  }
}
