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
    if (property.second.update) {
      property.second.update(property.second.entry, time);
    }
  }
  for (auto& updateTable : m_updateTables) {
    updateTable();
  }
}

void SendableBuilderImpl::StartListeners() {
  for (auto& property : m_properties) {
    property.second.StartListener();
  }
  if (m_controllableEntry) {
    m_controllableEntry.SetBoolean(true);
  }
}

void SendableBuilderImpl::StopListeners() {
  for (auto& property : m_properties) {
    property.second.StopListener();
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

wpi::SendableBuilder& SendableBuilderImpl::SetSmartDashboardType(
    std::string_view type) {
  m_table->GetEntry(".type").SetString(type);
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::SetActuator(bool value) {
  m_table->GetEntry(".actuator").SetBoolean(value);
  m_actuator = value;
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::SetSafeState(
    std::function<void()> func) {
  m_safeState = func;
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::SetUpdateTable(
    std::function<void()> func) {
  m_updateTables.emplace_back(std::move(func));
  return *this;
}

nt::NetworkTableEntry SendableBuilderImpl::GetEntry(std::string_view key) {
  return m_table->GetEntry(key);
}

wpi::SendableBuilder& SendableBuilderImpl::AddBooleanProperty(
    std::string_view key, std::function<bool()> getter,
    std::function<void(bool)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeBoolean(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddDoubleProperty(
    std::string_view key, std::function<double()> getter,
    std::function<void(double)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeDouble(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddStringProperty(
    std::string_view key, std::function<std::string()> getter,
    std::function<void(std::string_view)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeString(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddBooleanArrayProperty(
    std::string_view key, std::function<std::vector<int>()> getter,
    std::function<void(wpi::span<const int>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeBooleanArray(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddDoubleArrayProperty(
    std::string_view key, std::function<std::vector<double>()> getter,
    std::function<void(wpi::span<const double>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeDoubleArray(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddStringArrayProperty(
    std::string_view key, std::function<std::vector<std::string>()> getter,
    std::function<void(wpi::span<const std::string>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeStringArray(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddRawProperty(
    std::string_view key, std::function<std::string()> getter,
    std::function<void(std::string_view)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(nt::Value::MakeRaw(getter(), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddValueProperty(
    std::string_view key, std::function<std::shared_ptr<nt::Value>()> getter,
    std::function<void(std::shared_ptr<nt::Value>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      entry.SetValue(getter());
    };
  }
  if (setter) {
    property.createListener =
        [=](nt::NetworkTableEntry entry) -> NT_EntryListener {
      return entry.AddListener(
          [=](const nt::EntryNotification& event) {
            SmartDashboard::PostListenerTask([=] { setter(event.value); });
          },
          NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_UPDATE);
    };
  }
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddSmallStringProperty(
    std::string_view key,
    std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(std::string_view)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      wpi::SmallString<128> buf;
      entry.SetValue(nt::Value::MakeString(getter(buf), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddSmallBooleanArrayProperty(
    std::string_view key,
    std::function<wpi::span<const int>(wpi::SmallVectorImpl<int>& buf)> getter,
    std::function<void(wpi::span<const int>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      wpi::SmallVector<int, 16> buf;
      entry.SetValue(nt::Value::MakeBooleanArray(getter(buf), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddSmallDoubleArrayProperty(
    std::string_view key,
    std::function<wpi::span<const double>(wpi::SmallVectorImpl<double>& buf)>
        getter,
    std::function<void(wpi::span<const double>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      wpi::SmallVector<double, 16> buf;
      entry.SetValue(nt::Value::MakeDoubleArray(getter(buf), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddSmallStringArrayProperty(
    std::string_view key,
    std::function<
        wpi::span<const std::string>(wpi::SmallVectorImpl<std::string>& buf)>
        getter,
    std::function<void(wpi::span<const std::string>)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      wpi::SmallVector<std::string, 16> buf;
      entry.SetValue(nt::Value::MakeStringArray(getter(buf), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}

wpi::SendableBuilder& SendableBuilderImpl::AddSmallRawProperty(
    std::string_view key,
    std::function<std::string_view(wpi::SmallVectorImpl<char>& buf)> getter,
    std::function<void(std::string_view)> setter) {
  Property property{*m_table, key};
  if (getter) {
    property.update = [=](nt::NetworkTableEntry entry, uint64_t time) {
      wpi::SmallVector<char, 128> buf;
      entry.SetValue(nt::Value::MakeRaw(getter(buf), time));
    };
  }
  if (setter) {
    property.createListener =
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
  m_properties.insert_or_assign(key, std::move(property));
  return *this;
}
