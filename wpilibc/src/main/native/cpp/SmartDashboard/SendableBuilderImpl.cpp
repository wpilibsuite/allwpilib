/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableBuilderImpl.h"

#include <llvm/SmallString.h>

#include "ntcore_cpp.h"

using namespace frc;

void SendableBuilderImpl::SetTable(std::shared_ptr<nt::NetworkTable> table) {
  m_table = table;
}

std::shared_ptr<nt::NetworkTable> SendableBuilderImpl::GetTable() {
  return m_table;
}

void SendableBuilderImpl::UpdateTable() {
  uint64_t time = nt::Now();
  for (auto& property : m_properties) {
    if (property.update) property.update(property.entry, time);
  }
  if (m_updateTable) m_updateTable();
}

void SendableBuilderImpl::StartListeners() {
  for (auto& property : m_properties) property.StartListener();
}

void SendableBuilderImpl::StopListeners() {
  for (auto& property : m_properties) property.StopListener();
}

void SendableBuilderImpl::StartLiveWindowMode() {
  if (m_safeState) m_safeState();
  StartListeners();
}

void SendableBuilderImpl::StopLiveWindowMode() {
  StopListeners();
  if (m_safeState) m_safeState();
}

void SendableBuilderImpl::SetSmartDashboardType(const llvm::Twine& type) {
  m_table->GetEntry(".type").SetString(type);
}

void SendableBuilderImpl::SetSafeState(std::function<void()> func) {
  m_safeState = func;
}

void SendableBuilderImpl::SetUpdateTable(std::function<void()> func) {
  m_updateTable = func;
}

nt::NetworkTableEntry SendableBuilderImpl::GetEntry(const llvm::Twine& key) {
  return m_table->GetEntry(key);
}

void SendableBuilderImpl::AddBooleanProperty(const llvm::Twine& key,
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
    const llvm::Twine& key, std::function<double()> getter,
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
    const llvm::Twine& key, std::function<std::string()> getter,
    std::function<void(llvm::StringRef)> setter) {
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
    const llvm::Twine& key, std::function<std::vector<int>()> getter,
    std::function<void(llvm::ArrayRef<int>)> setter) {
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
    const llvm::Twine& key, std::function<std::vector<double>()> getter,
    std::function<void(llvm::ArrayRef<double>)> setter) {
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
    const llvm::Twine& key, std::function<std::vector<std::string>()> getter,
    std::function<void(llvm::ArrayRef<std::string>)> setter) {
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
    const llvm::Twine& key, std::function<std::string()> getter,
    std::function<void(llvm::StringRef)> setter) {
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
    const llvm::Twine& key, std::function<std::shared_ptr<nt::Value>()> getter,
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
    const llvm::Twine& key,
    std::function<llvm::StringRef(llvm::SmallVectorImpl<char>& buf)> getter,
    std::function<void(llvm::StringRef)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      llvm::SmallString<128> buf;
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
    const llvm::Twine& key,
    std::function<llvm::ArrayRef<int>(llvm::SmallVectorImpl<int>& buf)> getter,
    std::function<void(llvm::ArrayRef<int>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      llvm::SmallVector<int, 16> buf;
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
    const llvm::Twine& key,
    std::function<llvm::ArrayRef<double>(llvm::SmallVectorImpl<double>& buf)>
        getter,
    std::function<void(llvm::ArrayRef<double>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      llvm::SmallVector<double, 16> buf;
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
    const llvm::Twine& key,
    std::function<
        llvm::ArrayRef<std::string>(llvm::SmallVectorImpl<std::string>& buf)>
        getter,
    std::function<void(llvm::ArrayRef<std::string>)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      llvm::SmallVector<std::string, 16> buf;
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
    const llvm::Twine& key,
    std::function<llvm::StringRef(llvm::SmallVectorImpl<char>& buf)> getter,
    std::function<void(llvm::StringRef)> setter) {
  m_properties.emplace_back(*m_table, key);
  if (getter) {
    m_properties.back().update = [=](nt::NetworkTableEntry entry,
                                     uint64_t time) {
      llvm::SmallVector<char, 128> buf;
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
