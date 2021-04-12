// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/networktables/NTMechanism2D.h"

#include <algorithm>
#include <vector>

#include <imgui.h>
#include <ntcore_cpp.h>

#include "glass/other/Mechanism2D.h"

using namespace glass;

// Convert "#RRGGBB" hex color to ImU32 color
static void ConvertColor(wpi::StringRef in, ImU32* out) {
  if (in.size() != 7 || in[0] != '#') {
    return;
  }
  ImU32 val = 0;
  if (in.drop_front().getAsInteger(16, val)) {
    return;
  }
  *out = IM_COL32((val >> 16) & 0xff, (val >> 8) & 0xff, val & 0xff, 255);
}

namespace {

class NTMechanismObjectModel;

class NTMechanismGroupImpl final {
 public:
  NTMechanismGroupImpl(NT_Inst inst, const wpi::Twine& path,
                       wpi::StringRef name)
      : m_inst{inst}, m_path{path.str()}, m_name{name} {}

  const char* GetName() const { return m_name.c_str(); }
  void ForEachObject(wpi::function_ref<void(MechanismObjectModel& model)> func);
  void NTUpdate(const nt::EntryNotification& event, wpi::StringRef name);

 protected:
  NT_Inst m_inst;
  std::string m_path;
  std::string m_name;
  std::vector<std::unique_ptr<NTMechanismObjectModel>> m_objects;
};

class NTMechanismObjectModel final : public MechanismObjectModel {
 public:
  NTMechanismObjectModel(NT_Inst inst, const wpi::Twine& path,
                         wpi::StringRef name)
      : m_group{inst, path, name},
        m_type{nt::GetEntry(inst, path + "/.type")},
        m_color{nt::GetEntry(inst, path + "/color")},
        m_weight{nt::GetEntry(inst, path + "/weight")},
        m_angle{nt::GetEntry(inst, path + "/angle")},
        m_length{nt::GetEntry(inst, path + "/length")} {}

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

  bool NTUpdate(const nt::EntryNotification& event, wpi::StringRef childName);

 private:
  NTMechanismGroupImpl m_group;

  NT_Entry m_type;
  NT_Entry m_color;
  NT_Entry m_weight;
  NT_Entry m_angle;
  NT_Entry m_length;

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

void NTMechanismGroupImpl::NTUpdate(const nt::EntryNotification& event,
                                    wpi::StringRef name) {
  if (name.empty()) {
    return;
  }
  wpi::StringRef childName;
  std::tie(name, childName) = name.split('/');
  if (childName.empty()) {
    return;
  }

  auto it = std::lower_bound(
      m_objects.begin(), m_objects.end(), name,
      [](const auto& e, wpi::StringRef name) { return e->GetName() < name; });
  bool match = it != m_objects.end() && (*it)->GetName() == name;

  if (event.flags & NT_NOTIFY_NEW) {
    if (!match) {
      it = m_objects.emplace(it, std::make_unique<NTMechanismObjectModel>(
                                     m_inst, m_path + "/" + name, name));
      match = true;
    }
  }
  if (match) {
    if ((*it)->NTUpdate(event, childName)) {
      m_objects.erase(it);
    }
  }
}

bool NTMechanismObjectModel::NTUpdate(const nt::EntryNotification& event,
                                      wpi::StringRef childName) {
  if (event.entry == m_type) {
    if ((event.flags & NT_NOTIFY_DELETE) != 0) {
      return true;
    }
    if (event.value && event.value->IsString()) {
      m_typeValue = event.value->GetString();
    }
  } else if (event.entry == m_color) {
    if (event.value && event.value->IsString()) {
      ConvertColor(event.value->GetString(), &m_colorValue);
    }
  } else if (event.entry == m_weight) {
    if (event.value && event.value->IsDouble()) {
      m_weightValue = event.value->GetDouble();
    }
  } else if (event.entry == m_angle) {
    if (event.value && event.value->IsDouble()) {
      m_angleValue = units::degree_t{event.value->GetDouble()};
    }
  } else if (event.entry == m_length) {
    if (event.value && event.value->IsDouble()) {
      m_lengthValue = units::meter_t{event.value->GetDouble()};
    }
  } else {
    m_group.NTUpdate(event, childName);
  }
  return false;
}

class NTMechanism2DModel::RootModel final : public MechanismRootModel {
 public:
  RootModel(NT_Inst inst, const wpi::Twine& path, wpi::StringRef name)
      : m_group{inst, path, name},
        m_x{nt::GetEntry(inst, path + "/x")},
        m_y{nt::GetEntry(inst, path + "/y")} {}

  const char* GetName() const final { return m_group.GetName(); }
  void ForEachObject(
      wpi::function_ref<void(MechanismObjectModel& model)> func) final {
    m_group.ForEachObject(func);
  }

  bool NTUpdate(const nt::EntryNotification& event, wpi::StringRef childName);

  frc::Translation2d GetPosition() const override { return m_pos; };

 private:
  NTMechanismGroupImpl m_group;
  NT_Entry m_x;
  NT_Entry m_y;
  frc::Translation2d m_pos;
};

bool NTMechanism2DModel::RootModel::NTUpdate(const nt::EntryNotification& event,
                                             wpi::StringRef childName) {
  if ((event.flags & NT_NOTIFY_DELETE) != 0 &&
      (event.entry == m_x || event.entry == m_y)) {
    return true;
  } else if (event.entry == m_x) {
    if (event.value && event.value->IsDouble()) {
      m_pos = frc::Translation2d{units::meter_t{event.value->GetDouble()},
                                 m_pos.Y()};
    }
  } else if (event.entry == m_y) {
    if (event.value && event.value->IsDouble()) {
      m_pos = frc::Translation2d{m_pos.X(),
                                 units::meter_t{event.value->GetDouble()}};
    }
  } else {
    m_group.NTUpdate(event, childName);
  }
  return false;
}

NTMechanism2DModel::NTMechanism2DModel(wpi::StringRef path)
    : NTMechanism2DModel{nt::GetDefaultInstance(), path} {}

NTMechanism2DModel::NTMechanism2DModel(NT_Inst inst, wpi::StringRef path)
    : m_nt{inst},
      m_path{(path + "/").str()},
      m_name{m_nt.GetEntry(path + "/.name")},
      m_dimensions{m_nt.GetEntry(path + "/dims")},
      m_bgColor{m_nt.GetEntry(path + "/backgroundColor")},
      m_dimensionsValue{1_m, 1_m} {
  m_nt.AddListener(m_path, NT_NOTIFY_LOCAL | NT_NOTIFY_NEW | NT_NOTIFY_DELETE |
                               NT_NOTIFY_UPDATE | NT_NOTIFY_IMMEDIATE);
}

NTMechanism2DModel::~NTMechanism2DModel() = default;

void NTMechanism2DModel::Update() {
  for (auto&& event : m_nt.PollListener()) {
    // .name
    if (event.entry == m_name) {
      if (event.value && event.value->IsString()) {
        m_nameValue = event.value->GetString();
      }
      continue;
    }

    // dims
    if (event.entry == m_dimensions) {
      if (event.value && event.value->IsDoubleArray()) {
        auto arr = event.value->GetDoubleArray();
        if (arr.size() == 2) {
          m_dimensionsValue = frc::Translation2d{units::meter_t{arr[0]},
                                                 units::meter_t{arr[1]}};
        }
      }
    }

    // backgroundColor
    if (event.entry == m_bgColor) {
      if (event.value && event.value->IsString()) {
        ConvertColor(event.value->GetString(), &m_bgColorValue);
      }
    }

    if (wpi::StringRef{event.name}.startswith(m_path)) {
      auto name = wpi::StringRef{event.name}.drop_front(m_path.size());
      if (name.empty() || name[0] == '.') {
        continue;
      }
      wpi::StringRef childName;
      std::tie(name, childName) = name.split('/');
      if (childName.empty()) {
        continue;
      }

      auto it = std::lower_bound(m_roots.begin(), m_roots.end(), name,
                                 [](const auto& e, wpi::StringRef name) {
                                   return e->GetName() < name;
                                 });
      bool match = it != m_roots.end() && (*it)->GetName() == name;

      if (event.flags & NT_NOTIFY_NEW) {
        if (!match) {
          it =
              m_roots.emplace(it, std::make_unique<RootModel>(
                                      m_nt.GetInstance(), m_path + name, name));
          match = true;
        }
      }
      if (match) {
        if ((*it)->NTUpdate(event, childName)) {
          m_roots.erase(it);
        }
      }
    }
  }
}

bool NTMechanism2DModel::Exists() {
  return m_nt.IsConnected() && nt::GetEntryType(m_name) != NT_UNASSIGNED;
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
