// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/smartdashboard/SendableBuilderImpl.h"

#include <ntcore_cpp.h>
#include <wpi/SmallString.h>

#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

void SendableBuilderImpl::SetTable(std::shared_ptr<nt::NetworkTable> table) {
  m_table = table;
  m_controllableEntry = table->GetEntry(".controllable");
}

std::shared_ptr<nt::NetworkTable> SendableBuilderImpl::GetTable() {
  return m_table;
}

bool SendableBuilderImpl::IsPublished() const {
  return m_table != nullptr;
}

bool SendableBuilderImpl::IsActuator() const {
  return m_actuator;
}

void SendableBuilderImpl::Update() {
  uint64_t time = nt::Now();
  for (auto& property : m_properties) {
    if (property.update) {
      property.update(property.entry, time);
    }
  }
  for (auto& updateTable : m_updateTables) {
    updateTable();
  }
}

void SendableBuilderImpl::StartListeners() {
  for (auto& property : m_properties) {
    property.StartListener();
  }
  if (m_controllableEntry) {
    m_controllableEntry.SetBoolean(true);
  }
}

void SendableBuilderImpl::StopListeners() {
  for (auto& property : m_properties) {
    property.StopListener();
  }
  if (m_controllableEntry) {
    m_controllableEntry.SetBoolean(false);
  }
}

void SendableBuilderImpl::StartLiveWindowMode() {
  if (m_safeState) {
    m_safeState();
  }
  StartListeners();
}

void SendableBuilderImpl::StopLiveWindowMode() {
  StopListeners();
  if (m_safeState) {
    m_safeState();
  }
}

void SendableBuilderImpl::ClearProperties() {
  m_properties.clear();
}

void SendableBuilderImpl::SetSmartDashboardType(std::string_view type) {
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
  m_updateTables.emplace_back(std::move(func));
}

nt::NetworkTableEntry SendableBuilderImpl::GetEntry(std::string_view key) {
  return m_table->GetEntry(key);
}

void SendableBuilderImpl::AddBooleanProperty(std::string_view key,
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
            if (!event.value->IsBoolean()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetBoolean()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddDoubleProperty(
    std::string_view key, std::function<double()> getter,
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
            if (!event.value->IsDouble()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetDouble()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddStringProperty(
    std::string_view key, std::function<std::string()> getter,
    std::function<void(std::string_view)> setter) {
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
            if (!event.value->IsString()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetString()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddBooleanArrayProperty(
    std::string_view key, std::function<std::vector<int>()> getter,
    std::function<void(wpi::span<const int>)> setter) {
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
            if (!event.value->IsBooleanArray()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetBooleanArray()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddDoubleArrayProperty(
    std::string_view key, std::function<std::vector<double>()> getter,
    std::function<void(wpi::span<const double>)> setter) {
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
            if (!event.value->IsDoubleArray()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetDoubleArray()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddStringArrayProperty(
    std::string_view key, std::function<std::vector<std::string>()> getter,
    std::function<void(wpi::span<const std::string>)> setter) {
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
            if (!event.value->IsStringArray()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetStringArray()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddRawProperty(
    std::string_view key, std::function<std::string()> getter,
    std::function<void(std::string_view)> setter) {
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
            if (!event.value->IsRaw()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetRaw()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddValueProperty(
    std::string_view key, std::function<std::shared_ptr<nt::Value>()> getter,
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
          [=](const nt::EntryNotification& event) {
            SmartDashboard::PostListenerTask([=] { setter(event.value); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallStringProperty(
    std::string_view key,
    std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(std::string_view)> setter) {
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
            if (!event.value->IsString()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetString()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallBooleanArrayProperty(
    std::string_view key,
    std::function<wpi::span<const int>(wpi::SmallVectorImpl<int>& buf)> getter,
    std::function<void(wpi::span<const int>)> setter) {
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
            if (!event.value->IsBooleanArray()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetBooleanArray()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallDoubleArrayProperty(
    std::string_view key,
    std::function<wpi::span<const double>(wpi::SmallVectorImpl<double>& buf)>
        getter,
    std::function<void(wpi::span<const double>)> setter) {
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
            if (!event.value->IsDoubleArray()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetDoubleArray()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallStringArrayProperty(
    std::string_view key,
    std::function<
        wpi::span<const std::string>(wpi::SmallVectorImpl<std::string>& buf)>
        getter,
    std::function<void(wpi::span<const std::string>)> setter) {
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
            if (!event.value->IsStringArray()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetStringArray()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}

void SendableBuilderImpl::AddSmallRawProperty(
    std::string_view key,
    std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(std::string_view)> setter) {
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
            if (!event.value->IsRaw()) {
              return;
            }
            SmartDashboard::PostListenerTask(
                [=] { setter(event.value->GetRaw()); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
}
