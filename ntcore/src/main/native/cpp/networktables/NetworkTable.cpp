// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/NetworkTable.h"

#include <algorithm>

#include <fmt/core.h>
#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>

#include "networktables/NetworkTableInstance.h"
#include "ntcore.h"

using namespace nt;

std::string_view NetworkTable::BasenameKey(std::string_view key) {
  size_t slash = key.rfind(PATH_SEPARATOR_CHAR);
  if (slash == std::string_view::npos) {
    return key;
  }
  return key.substr(slash + 1);
}

std::string NetworkTable::NormalizeKey(std::string_view key,
                                       bool withLeadingSlash) {
  wpi::SmallString<128> buf;
  return std::string{NormalizeKey(key, buf, withLeadingSlash)};
}

std::string_view NetworkTable::NormalizeKey(std::string_view key,
                                            wpi::SmallVectorImpl<char>& buf,
                                            bool withLeadingSlash) {
  buf.clear();
  if (withLeadingSlash) {
    buf.push_back(PATH_SEPARATOR_CHAR);
  }
  // for each path element, add it with a slash following
  wpi::SmallVector<std::string_view, 16> parts;
  wpi::split(key, parts, PATH_SEPARATOR_CHAR, -1, false);
  for (auto i = parts.begin(); i != parts.end(); ++i) {
    buf.append(i->begin(), i->end());
    buf.push_back(PATH_SEPARATOR_CHAR);
  }
  // remove trailing slash if the input key didn't have one
  if (!key.empty() && key.back() != PATH_SEPARATOR_CHAR) {
    buf.pop_back();
  }
  return {buf.data(), buf.size()};
}

std::vector<std::string> NetworkTable::GetHierarchy(std::string_view key) {
  std::vector<std::string> hierarchy;
  hierarchy.emplace_back(1, PATH_SEPARATOR_CHAR);
  // for each path element, add it to the end of what we built previously
  wpi::SmallString<128> path;
  wpi::SmallVector<std::string_view, 16> parts;
  wpi::split(key, parts, PATH_SEPARATOR_CHAR, -1, false);
  if (!parts.empty()) {
    for (auto i = parts.begin(); i != parts.end(); ++i) {
      path += PATH_SEPARATOR_CHAR;
      path += *i;
      hierarchy.emplace_back(path.str());
    }
    // handle trailing slash
    if (key.back() == PATH_SEPARATOR_CHAR) {
      path += PATH_SEPARATOR_CHAR;
      hierarchy.emplace_back(path.str());
    }
  }
  return hierarchy;
}

NetworkTable::NetworkTable(NT_Inst inst, std::string_view path,
                           const private_init&)
    : m_inst(inst), m_path(path) {}

NetworkTable::~NetworkTable() {
  for (auto i : m_listeners) {
    RemoveEntryListener(i);
  }
}

NetworkTableInstance NetworkTable::GetInstance() const {
  return NetworkTableInstance{m_inst};
}

NetworkTableEntry NetworkTable::GetEntry(std::string_view key) const {
  std::scoped_lock lock(m_mutex);
  NT_Entry& entry = m_entries[key];
  if (entry == 0) {
    fmt::memory_buffer buf;
    fmt::format_to(buf, "{}/{}", m_path, key);
    entry = nt::GetEntry(m_inst, {buf.data(), buf.size()});
  }
  return NetworkTableEntry{entry};
}

NT_EntryListener NetworkTable::AddEntryListener(TableEntryListener listener,
                                                unsigned int flags) const {
  size_t prefix_len = m_path.size() + 1;
  return nt::AddEntryListener(
      m_inst, fmt::format("{}/", m_path),
      [=](const EntryNotification& event) {
        auto relative_key = std::string_view{event.name}.substr(prefix_len);
        if (relative_key.find(PATH_SEPARATOR_CHAR) != std::string_view::npos) {
          return;
        }
        listener(const_cast<NetworkTable*>(this), relative_key,
                 NetworkTableEntry{event.entry}, event.value, event.flags);
      },
      flags);
}

NT_EntryListener NetworkTable::AddEntryListener(std::string_view key,
                                                TableEntryListener listener,
                                                unsigned int flags) const {
  size_t prefix_len = m_path.size() + 1;
  auto entry = GetEntry(key);
  return nt::AddEntryListener(
      entry.GetHandle(),
      [=](const EntryNotification& event) {
        listener(const_cast<NetworkTable*>(this),
                 std::string_view{event.name}.substr(prefix_len), entry,
                 event.value, event.flags);
      },
      flags);
}

void NetworkTable::RemoveEntryListener(NT_EntryListener listener) const {
  nt::RemoveEntryListener(listener);
}

NT_EntryListener NetworkTable::AddSubTableListener(TableListener listener,
                                                   bool localNotify) {
  size_t prefix_len = m_path.size() + 1;

  // The lambda needs to be copyable, but StringMap is not, so use
  // a shared_ptr to it.
  auto notified_tables = std::make_shared<wpi::StringMap<char>>();

  unsigned int flags = NT_NOTIFY_NEW | NT_NOTIFY_IMMEDIATE;
  if (localNotify) {
    flags |= NT_NOTIFY_LOCAL;
  }
  NT_EntryListener id = nt::AddEntryListener(
      m_inst, fmt::format("{}/", m_path),
      [=](const EntryNotification& event) {
        auto relative_key = std::string_view{event.name}.substr(prefix_len);
        auto end_sub_table = relative_key.find(PATH_SEPARATOR_CHAR);
        if (end_sub_table == std::string_view::npos) {
          return;
        }
        auto sub_table_key = relative_key.substr(0, end_sub_table);
        if (notified_tables->find(sub_table_key) == notified_tables->end()) {
          return;
        }
        notified_tables->insert(std::make_pair(sub_table_key, '\0'));
        listener(this, sub_table_key, this->GetSubTable(sub_table_key));
      },
      flags);
  m_listeners.emplace_back(id);
  return id;
}

void NetworkTable::RemoveTableListener(NT_EntryListener listener) {
  nt::RemoveEntryListener(listener);
  auto matches_begin =
      std::remove(m_listeners.begin(), m_listeners.end(), listener);
  m_listeners.erase(matches_begin, m_listeners.end());
}

std::shared_ptr<NetworkTable> NetworkTable::GetSubTable(
    std::string_view key) const {
  return std::make_shared<NetworkTable>(
      m_inst, fmt::format("{}/{}", m_path, key), private_init{});
}

bool NetworkTable::ContainsKey(std::string_view key) const {
  if (key.empty()) {
    return false;
  }
  return GetEntry(key).Exists();
}

bool NetworkTable::ContainsSubTable(std::string_view key) const {
  return !GetEntryInfo(m_inst, fmt::format("{}/{}/", m_path, key), 0).empty();
}

std::vector<std::string> NetworkTable::GetKeys(int types) const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  auto infos = GetEntryInfo(m_inst, fmt::format("{}/", m_path), types);
  std::scoped_lock lock(m_mutex);
  for (auto& info : infos) {
    auto relative_key = std::string_view{info.name}.substr(prefix_len);
    if (relative_key.find(PATH_SEPARATOR_CHAR) != std::string_view::npos) {
      continue;
    }
    keys.emplace_back(relative_key);
    m_entries[relative_key] = info.entry;
  }
  return keys;
}

std::vector<std::string> NetworkTable::GetSubTables() const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  for (auto& entry : GetEntryInfo(m_inst, fmt::format("{}/", m_path), 0)) {
    auto relative_key = std::string_view{entry.name}.substr(prefix_len);
    size_t end_subtable = relative_key.find(PATH_SEPARATOR_CHAR);
    if (end_subtable == std::string_view::npos) {
      continue;
    }
    keys.emplace_back(relative_key.substr(0, end_subtable));
  }
  return keys;
}

void NetworkTable::SetPersistent(std::string_view key) {
  GetEntry(key).SetPersistent();
}

void NetworkTable::ClearPersistent(std::string_view key) {
  GetEntry(key).ClearPersistent();
}

bool NetworkTable::IsPersistent(std::string_view key) const {
  return GetEntry(key).IsPersistent();
}

void NetworkTable::SetFlags(std::string_view key, unsigned int flags) {
  GetEntry(key).SetFlags(flags);
}

void NetworkTable::ClearFlags(std::string_view key, unsigned int flags) {
  GetEntry(key).ClearFlags(flags);
}

unsigned int NetworkTable::GetFlags(std::string_view key) const {
  return GetEntry(key).GetFlags();
}

void NetworkTable::Delete(std::string_view key) {
  GetEntry(key).Delete();
}

bool NetworkTable::PutNumber(std::string_view key, double value) {
  return GetEntry(key).SetDouble(value);
}

bool NetworkTable::SetDefaultNumber(std::string_view key, double defaultValue) {
  return GetEntry(key).SetDefaultDouble(defaultValue);
}

double NetworkTable::GetNumber(std::string_view key,
                               double defaultValue) const {
  return GetEntry(key).GetDouble(defaultValue);
}

bool NetworkTable::PutString(std::string_view key, std::string_view value) {
  return GetEntry(key).SetString(value);
}

bool NetworkTable::SetDefaultString(std::string_view key,
                                    std::string_view defaultValue) {
  return GetEntry(key).SetDefaultString(defaultValue);
}

std::string NetworkTable::GetString(std::string_view key,
                                    std::string_view defaultValue) const {
  return GetEntry(key).GetString(defaultValue);
}

bool NetworkTable::PutBoolean(std::string_view key, bool value) {
  return GetEntry(key).SetBoolean(value);
}

bool NetworkTable::SetDefaultBoolean(std::string_view key, bool defaultValue) {
  return GetEntry(key).SetDefaultBoolean(defaultValue);
}

bool NetworkTable::GetBoolean(std::string_view key, bool defaultValue) const {
  return GetEntry(key).GetBoolean(defaultValue);
}

bool NetworkTable::PutBooleanArray(std::string_view key,
                                   wpi::ArrayRef<int> value) {
  return GetEntry(key).SetBooleanArray(value);
}

bool NetworkTable::SetDefaultBooleanArray(std::string_view key,
                                          wpi::ArrayRef<int> defaultValue) {
  return GetEntry(key).SetDefaultBooleanArray(defaultValue);
}

std::vector<int> NetworkTable::GetBooleanArray(
    std::string_view key, wpi::ArrayRef<int> defaultValue) const {
  return GetEntry(key).GetBooleanArray(defaultValue);
}

bool NetworkTable::PutNumberArray(std::string_view key,
                                  wpi::ArrayRef<double> value) {
  return GetEntry(key).SetDoubleArray(value);
}

bool NetworkTable::SetDefaultNumberArray(std::string_view key,
                                         wpi::ArrayRef<double> defaultValue) {
  return GetEntry(key).SetDefaultDoubleArray(defaultValue);
}

std::vector<double> NetworkTable::GetNumberArray(
    std::string_view key, wpi::ArrayRef<double> defaultValue) const {
  return GetEntry(key).GetDoubleArray(defaultValue);
}

bool NetworkTable::PutStringArray(std::string_view key,
                                  wpi::ArrayRef<std::string> value) {
  return GetEntry(key).SetStringArray(value);
}

bool NetworkTable::SetDefaultStringArray(
    std::string_view key, wpi::ArrayRef<std::string> defaultValue) {
  return GetEntry(key).SetDefaultStringArray(defaultValue);
}

std::vector<std::string> NetworkTable::GetStringArray(
    std::string_view key, wpi::ArrayRef<std::string> defaultValue) const {
  return GetEntry(key).GetStringArray(defaultValue);
}

bool NetworkTable::PutRaw(std::string_view key, std::string_view value) {
  return GetEntry(key).SetRaw(value);
}

bool NetworkTable::SetDefaultRaw(std::string_view key,
                                 std::string_view defaultValue) {
  return GetEntry(key).SetDefaultRaw(defaultValue);
}

std::string NetworkTable::GetRaw(std::string_view key,
                                 std::string_view defaultValue) const {
  return GetEntry(key).GetRaw(defaultValue);
}

bool NetworkTable::PutValue(std::string_view key,
                            std::shared_ptr<Value> value) {
  return GetEntry(key).SetValue(value);
}

bool NetworkTable::SetDefaultValue(std::string_view key,
                                   std::shared_ptr<Value> defaultValue) {
  return GetEntry(key).SetDefaultValue(defaultValue);
}

std::shared_ptr<Value> NetworkTable::GetValue(std::string_view key) const {
  return GetEntry(key).GetValue();
}

std::string_view NetworkTable::GetPath() const {
  return m_path;
}

const char* NetworkTable::SaveEntries(std::string_view filename) const {
  return nt::SaveEntries(m_inst, filename, fmt::format("{}/", m_path));
}

const char* NetworkTable::LoadEntries(
    std::string_view filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return nt::LoadEntries(m_inst, filename, fmt::format("{}/", m_path), warn);
}
