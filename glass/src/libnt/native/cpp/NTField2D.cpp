/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "glass/networktables/NTField2D.h"

#include <algorithm>

#include <ntcore_cpp.h>
#include <wpi/SmallVector.h>

#include "glass/DataSource.h"

using namespace glass;

class NTField2DModel::GroupModel : public FieldObjectGroupModel {
 public:
  GroupModel(wpi::StringRef name, NT_Entry entry)
      : m_name{name}, m_entry{entry} {}

  wpi::StringRef GetName() const { return m_name; }
  NT_Entry GetEntry() const { return m_entry; }

  void NTUpdate(const nt::Value& value);

  void Update() override {
    if (auto value = nt::GetEntryValue(m_entry)) NTUpdate(*value);
  }
  bool Exists() override { return nt::GetEntryType(m_entry) != NT_UNASSIGNED; }
  bool IsReadOnly() override { return false; }

  void ForEachFieldObject(
      wpi::function_ref<void(FieldObjectModel& model)> func) override;

 private:
  std::string m_name;
  NT_Entry m_entry;

  // keep count of objects rather than resizing vector, as there is a fair
  // amount of overhead associated with the latter (DataSource record keeping)
  size_t m_count = 0;
  class ObjectModel;
  std::vector<std::unique_ptr<ObjectModel>> m_objects;
};

class NTField2DModel::GroupModel::ObjectModel : public FieldObjectModel {
 public:
  ObjectModel(wpi::StringRef name, NT_Entry entry, int index)
      : m_entry{entry},
        m_index{index},
        m_x{name + "[" + wpi::Twine{index} + "]/x"},
        m_y{name + "[" + wpi::Twine{index} + "]/y"},
        m_rot{name + "[" + wpi::Twine{index} + "]/rot"} {}

  void SetExists(bool exists) { m_exists = exists; }

  void Update() override {}
  bool Exists() override { return m_exists; }
  bool IsReadOnly() override { return false; }

  DataSource* GetXData() override { return &m_x; }
  DataSource* GetYData() override { return &m_y; }
  DataSource* GetRotationData() override { return &m_rot; }

  void SetPose(double x, double y, double rot) override;
  void SetPosition(double x, double y) override;
  void SetRotation(double rot) override;

 private:
  void SetPoseImpl(double x, double y, double rot, bool setX, bool setY,
                   bool setRot);

  NT_Entry m_entry;
  int m_index;
  bool m_exists = true;

 public:
  DataSource m_x;
  DataSource m_y;
  DataSource m_rot;
};

void NTField2DModel::GroupModel::NTUpdate(const nt::Value& value) {
  if (!value.IsDoubleArray()) {
    m_count = 0;
    return;
  }

  auto arr = value.GetDoubleArray();
  // must be triples
  if ((arr.size() % 3) != 0) {
    m_count = 0;
    return;
  }

  m_count = arr.size() / 3;
  if (m_count > m_objects.size()) {
    m_objects.reserve(m_count);
    for (size_t i = m_objects.size(); i < m_count; ++i)
      m_objects.emplace_back(std::make_unique<ObjectModel>(m_name, m_entry, i));
  }
  if (m_count < m_objects.size()) {
    for (size_t i = m_count; i < m_objects.size(); ++i)
      m_objects[i]->SetExists(false);
  }

  for (size_t i = 0; i < m_count; ++i) {
    auto& obj = m_objects[i];
    obj->SetExists(true);
    obj->m_x.SetValue(arr[i * 3], value.last_change());
    obj->m_y.SetValue(arr[i * 3 + 1], value.last_change());
    obj->m_rot.SetValue(arr[i * 3 + 2], value.last_change());
  }
}

void NTField2DModel::GroupModel::ForEachFieldObject(
    wpi::function_ref<void(FieldObjectModel& model)> func) {
  for (size_t i = 0; i < m_count; ++i) {
    func(*m_objects[i]);
  }
}

void NTField2DModel::GroupModel::ObjectModel::SetPose(double x, double y,
                                                      double rot) {
  SetPoseImpl(x, y, rot, true, true, true);
}

void NTField2DModel::GroupModel::ObjectModel::SetPosition(double x, double y) {
  SetPoseImpl(x, y, 0, true, true, false);
}

void NTField2DModel::GroupModel::ObjectModel::SetRotation(double rot) {
  SetPoseImpl(0, 0, rot, false, false, true);
}

void NTField2DModel::GroupModel::ObjectModel::SetPoseImpl(double x, double y,
                                                          double rot, bool setX,
                                                          bool setY,
                                                          bool setRot) {
  // get from NT, validate type and size
  auto value = nt::GetEntryValue(m_entry);
  if (!value || !value->IsDoubleArray()) return;
  auto origArr = value->GetDoubleArray();
  if (origArr.size() < static_cast<size_t>((m_index + 1) * 3)) return;

  // copy existing array
  wpi::SmallVector<double, 8> arr;
  arr.reserve(origArr.size());
  for (auto&& elem : origArr) arr.emplace_back(elem);

  // update value
  if (setX) arr[m_index * 3 + 0] = x;
  if (setY) arr[m_index * 3 + 1] = y;
  if (setRot) arr[m_index * 3 + 2] = rot;

  // set back to NT
  nt::SetEntryValue(m_entry, nt::Value::MakeDoubleArray(arr));
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

NTField2DModel::~NTField2DModel() {}

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
          m_groups.begin(), m_groups.end(),
          [&](const auto& e) { return e->GetEntry() == event.entry; });
      if (it != m_groups.end()) {
        (*it)->NTUpdate(*event.value);
        continue;
      }
    }

    // handle create/delete
    if (wpi::StringRef{event.name}.startswith(m_path)) {
      auto name = wpi::StringRef{event.name}.drop_front(m_path.size());
      if (name.empty() || name[0] == '.') continue;
      auto it = std::lower_bound(m_groups.begin(), m_groups.end(), name,
                                 [](const auto& e, wpi::StringRef name) {
                                   return e->GetName() < name;
                                 });
      bool match = (it != m_groups.end() && (*it)->GetName() == name);
      if (event.flags & NT_NOTIFY_DELETE) {
        if (match) m_groups.erase(it);
        continue;
      } else if (event.flags & NT_NOTIFY_NEW) {
        if (!match)
          it = m_groups.emplace(
              it, std::make_unique<GroupModel>(event.name, event.entry));
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

bool NTField2DModel::IsReadOnly() { return false; }

void NTField2DModel::ForEachFieldObjectGroup(
    wpi::function_ref<void(FieldObjectGroupModel& model, wpi::StringRef name)>
        func) {
  for (auto&& group : m_groups) {
    if (group->Exists())
      func(*group, wpi::StringRef{group->GetName()}.drop_front(m_path.size()));
  }
}
