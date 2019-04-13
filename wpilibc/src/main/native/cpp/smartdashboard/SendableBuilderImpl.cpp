/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/smartdashboard/SendableBuilderImpl.h"

#include <ntcore_cpp.h>
#include <wpi/SmallString.h>

using namespace frc;

void SendableBuilderImpl::SetTable(std::shared_ptr<nt::NetworkTable> table) {
  m_table = table;
  m_controllableEntry = table->GetEntry(".controllable");
}

std::shared_ptr<nt::NetworkTable> SendableBuilderImpl::GetTable() {
  return m_table;
}

bool SendableBuilderImpl::IsActuator() const { return m_actuator; }

void SendableBuilderImpl::UpdateTable() {
  uint64_t time = nt::Now();
  for (auto& property : m_properties) {
    if (property.update) property.update(property.entry, time);
  }
  if (m_updateTable) m_updateTable();
}

void SendableBuilderImpl::StartListeners() {
  for (auto& property : m_properties) property.StartListener();
  if (m_controllableEntry) m_controllableEntry.SetBoolean(true);
}

void SendableBuilderImpl::StopListeners() {
  for (auto& property : m_properties) property.StopListener();
  if (m_controllableEntry) m_controllableEntry.SetBoolean(false);
}

void SendableBuilderImpl::StartLiveWindowMode() {
  if (m_safeState) m_safeState();
  StartListeners();
}

void SendableBuilderImpl::StopLiveWindowMode() {
  StopListeners();
  if (m_safeState) m_safeState();
}

void SendableBuilderImpl::SetSmartDashboardType(const wpi::Twine& type) {
  m_table->GetEntry(".type").SetString(type);
}

void SendableBuilderImpl::SetActuator(bool value) {
  m_table->GetEntry(".actuator").SetBoolean(value);
  m_actuator = value;
}

void SendableBuilderImpl::SetSafeState(std::function<void()> func) {
  m_safeState = func;
}

void SendableBuilderImpl::SetUpdateTable(std::function<void()> func) {
  m_updateTable = func;
}

nt::NetworkTableEntry SendableBuilderImpl::GetEntry(const wpi::Twine& key) {
  return m_table->GetEntry(key);
}

void SendableBuilderImpl::AddBooleanProperty(const wpi::Twine& key,
                                             std::function<bool()> getter,
                                             std::function<void(bool)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeBoolean(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsBoolean()) return;
            setter(event.value->GetBoolean());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddDoubleProperty(
    const wpi::Twine& key, std::function<double()> getter,
    std::function<void(double)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeDouble(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsDouble()) return;
            setter(event.value->GetDouble());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddStringProperty(
    const wpi::Twine& key, std::function<std::string()> getter,
    std::function<void(wpi::StringRef)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeString(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsString()) return;
            setter(event.value->GetString());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddBooleanArrayProperty(
    const wpi::Twine& key, std::function<std::vector<int>()> getter,
    std::function<void(wpi::ArrayRef<int>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeBooleanArray(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsBooleanArray()) return;
            setter(event.value->GetBooleanArray());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddDoubleArrayProperty(
    const wpi::Twine& key, std::function<std::vector<double>()> getter,
    std::function<void(wpi::ArrayRef<double>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeDoubleArray(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsDoubleArray()) return;
            setter(event.value->GetDoubleArray());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddStringArrayProperty(
    const wpi::Twine& key, std::function<std::vector<std::string>()> getter,
    std::function<void(wpi::ArrayRef<std::string>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeStringArray(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsStringArray()) return;
            setter(event.value->GetStringArray());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddRawProperty(
    const wpi::Twine& key, std::function<std::string()> getter,
    std::function<void(wpi::StringRef)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(nt::Value::MakeRaw(getter(), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsRaw()) return;
            setter(event.value->GetRaw());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddValueProperty(
    const wpi::Twine& key, std::function<std::shared_ptr<nt::Value>()> getter,
    std::function<void(std::shared_ptr<nt::Value>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      entry.SetValue(getter());
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) { setter(event.value); },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallStringProperty(
    const wpi::Twine& key,
    std::function<wpi::StringRef(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(wpi::StringRef)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      wpi::SmallString<128> buf;
      entry.SetValue(nt::Value::MakeString(getter(buf), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsString()) return;
            setter(event.value->GetString());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallBooleanArrayProperty(
    const wpi::Twine& key,
    std::function<wpi::ArrayRef<int>(wpi::SmallVectorImpl<int>& buf)> getter,
    std::function<void(wpi::ArrayRef<int>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      wpi::SmallVector<int, 16> buf;
      entry.SetValue(nt::Value::MakeBooleanArray(getter(buf), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsBooleanArray()) return;
            setter(event.value->GetBooleanArray());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallDoubleArrayProperty(
    const wpi::Twine& key,
    std::function<wpi::ArrayRef<double>(wpi::SmallVectorImpl<double>& buf)>
        getter,
    std::function<void(wpi::ArrayRef<double>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      wpi::SmallVector<double, 16> buf;
      entry.SetValue(nt::Value::MakeDoubleArray(getter(buf), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsDoubleArray()) return;
            setter(event.value->GetDoubleArray());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallStringArrayProperty(
    const wpi::Twine& key,
    std::function<
        wpi::ArrayRef<std::string>(wpi::SmallVectorImpl<std::string>& buf)>
        getter,
    std::function<void(wpi::ArrayRef<std::string>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      wpi::SmallVector<std::string, 16> buf;
      entry.SetValue(nt::Value::MakeStringArray(getter(buf), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsStringArray()) return;
            setter(event.value->GetStringArray());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallRawProperty(
    const wpi::Twine& key,
    std::function<wpi::StringRef(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(wpi::StringRef)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      wpi::SmallVector<char, 128> buf;
      entry.SetValue(nt::Value::MakeRaw(getter(buf), time));
    };
  }
  if (setter) {
    m_properties.back().createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            if (!event.value->IsRaw()) return;
            setter(event.value->GetRaw());
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}
