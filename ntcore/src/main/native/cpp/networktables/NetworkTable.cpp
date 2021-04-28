// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/NetworkTable.h"

#include <algorithm>

#include <wpi/SmallString.h>
#include <wpi/StringMap.h>
#include <wpi/raw_ostream.h>

#include "networktables/NetworkTableInstance.h"
#include "ntcore.h"

using namespace nt;

wpi::StringRef NetworkTable::BasenameKey(wpi::StringRef key) {
  size_t slash = key.rfind(PATH_SEPARATOR_CHAR);
  if (slash == wpi::StringRef::npos) {
    return key;
  }
  return key.substr(slash + 1);
}

std::string NetworkTable::NormalizeKey(const wpi::Twine& key,
                                       bool withLeadingSlash) {
  wpi::SmallString<128> buf;
  return NormalizeKey(key, buf, withLeadingSlash);
}

wpi::StringRef NetworkTable::NormalizeKey(const wpi::Twine& key,
                                          wpi::SmallVectorImpl<char>& buf,
                                          bool withLeadingSlash) {
  buf.clear();
  if (withLeadingSlash) {
    buf.push_back(PATH_SEPARATOR_CHAR);
  }
  // for each path element, add it with a slash following
  wpi::SmallString<128> keyBuf;
  wpi::StringRef keyStr = key.toStringRef(keyBuf);
  wpi::SmallVector<wpi::StringRef, 16> parts;
  keyStr.split(parts, PATH_SEPARATOR_CHAR, -1, false);
  for (auto i = parts.begin(); i != parts.end(); ++i) {
    buf.append(i->begin(), i->end());
    buf.push_back(PATH_SEPARATOR_CHAR);
  }
  // remove trailing slash if the input key didn't have one
  if (!keyStr.empty() && keyStr.back() != PATH_SEPARATOR_CHAR) {
    buf.pop_back();
  }
  return wpi::StringRef(buf.data(), buf.size());
}

std::vector<std::string> NetworkTable::GetHierarchy(const wpi::Twine& key) {
  std::vector<std::string> hierarchy;
  hierarchy.emplace_back(1, PATH_SEPARATOR_CHAR);
  // for each path element, add it to the end of what we built previously
  wpi::SmallString<128> keyBuf;
  wpi::StringRef keyStr = key.toStringRef(keyBuf);
  wpi::SmallString<128> path;
  wpi::SmallVector<wpi::StringRef, 16> parts;
  keyStr.split(parts, PATH_SEPARATOR_CHAR, -1, false);
  if (!parts.empty()) {
    for (auto i = parts.begin(); i != parts.end(); ++i) {
      path += PATH_SEPARATOR_CHAR;
      path += *i;
      hierarchy.emplace_back(path.str());
    }
    // handle trailing slash
    if (keyStr.back() == PATH_SEPARATOR_CHAR) {
      path += PATH_SEPARATOR_CHAR;
      hierarchy.emplace_back(path.str());
    }
  }
  return hierarchy;
}

NetworkTable::NetworkTable(NT_Inst inst, const wpi::Twine& path,
                           const private_init&)
    : m_inst(inst), m_path(path.str()) {}

NetworkTable::~NetworkTable() {
  for (auto i : m_listeners) {
    RemoveEntryListener(i);
  }
}

NetworkTableInstance NetworkTable::GetInstance() const {
  return NetworkTableInstance{m_inst};
}

NetworkTableEntry NetworkTable::GetEntry(const wpi::Twine& key) const {
  wpi::SmallString<128> keyBuf;
  wpi::StringRef keyStr = key.toStringRef(keyBuf);
  std::scoped_lock lock(m_mutex);
  NT_Entry& entry = m_entries[keyStr];
  if (entry == 0) {
    entry =
        nt::GetEntry(m_inst, m_path + wpi::Twine(PATH_SEPARATOR_CHAR) + keyStr);
  }
  return NetworkTableEntry{entry};
}

NT_EntryListener NetworkTable::AddEntryListener(TableEntryListener listener,
                                                unsigned int flags) const {
  size_t prefix_len = m_path.size() + 1;
  return nt::AddEntryListener(
      m_inst, m_path + wpi::Twine(PATH_SEPARATOR_CHAR),
      [=](const EntryNotification& event) {
        wpi::StringRef relative_key = event.name.substr(prefix_len);
        if (relative_key.find(PATH_SEPARATOR_CHAR) != wpi::StringRef::npos) {
          return;
        }
        listener(const_cast<NetworkTable*>(this), relative_key,
                 NetworkTableEntry{event.entry}, event.value, event.flags);
      },
      flags);
}

NT_EntryListener NetworkTable::AddEntryListener(const wpi::Twine& key,
                                                TableEntryListener listener,
                                                unsigned int flags) const {
  size_t prefix_len = m_path.size() + 1;
  auto entry = GetEntry(key);
  return nt::AddEntryListener(
      entry.GetHandle(),
      [=](const EntryNotification& event) {
        listener(const_cast<NetworkTable*>(this), event.name.substr(prefix_len),
                 entry, event.value, event.flags);
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
      m_inst, m_path + wpi::Twine(PATH_SEPARATOR_CHAR),
      [=](const EntryNotification& event) {
        wpi::StringRef relative_key = event.name.substr(prefix_len);
        auto end_sub_table = relative_key.find(PATH_SEPARATOR_CHAR);
        if (end_sub_table == wpi::StringRef::npos) {
          return;
        }
        wpi::StringRef sub_table_key = relative_key.substr(0, end_sub_table);
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
    const wpi::Twine& key) const {
  return std::make_shared<NetworkTable>(
      m_inst, m_path + wpi::Twine(PATH_SEPARATOR_CHAR) + key, private_init{});
}

bool NetworkTable::ContainsKey(const wpi::Twine& key) const {
  if (key.isTriviallyEmpty() ||
      (key.isSingleStringRef() && key.getSingleStringRef().empty())) {
    return false;
  }
  return GetEntry(key).Exists();
}

bool NetworkTable::ContainsSubTable(const wpi::Twine& key) const {
  return !GetEntryInfo(m_inst,
                       m_path + wpi::Twine(PATH_SEPARATOR_CHAR) + key +
                           wpi::Twine(PATH_SEPARATOR_CHAR),
                       0)
              .empty();
}

std::vector<std::string> NetworkTable::GetKeys(int types) const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  auto infos =
      GetEntryInfo(m_inst, m_path + wpi::Twine(PATH_SEPARATOR_CHAR), types);
  std::scoped_lock lock(m_mutex);
  for (auto& info : infos) {
    auto relative_key = wpi::StringRef(info.name).substr(prefix_len);
    if (relative_key.find(PATH_SEPARATOR_CHAR) != wpi::StringRef::npos) {
      continue;
    }
    keys.push_back(relative_key);
    m_entries[relative_key] = info.entry;
  }
  return keys;
}

std::vector<std::string> NetworkTable::GetSubTables() const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  for (auto& entry :
       GetEntryInfo(m_inst, m_path + wpi::Twine(PATH_SEPARATOR_CHAR), 0)) {
    auto relative_key = wpi::StringRef(entry.name).substr(prefix_len);
    size_t end_subtable = relative_key.find(PATH_SEPARATOR_CHAR);
    if (end_subtable == wpi::StringRef::npos) {
      continue;
    }
    keys.push_back(relative_key.substr(0, end_subtable));
  }
  return keys;
}

void NetworkTable::SetPersistent(wpi::StringRef key) {
  GetEntry(key).SetPersistent();
}

void NetworkTable::ClearPersistent(wpi::StringRef key) {
  GetEntry(key).ClearPersistent();
}

bool NetworkTable::IsPersistent(wpi::StringRef key) const {
  return GetEntry(key).IsPersistent();
}

void NetworkTable::SetFlags(wpi::StringRef key, unsigned int flags) {
  GetEntry(key).SetFlags(flags);
}

void NetworkTable::ClearFlags(wpi::StringRef key, unsigned int flags) {
  GetEntry(key).ClearFlags(flags);
}

unsigned int NetworkTable::GetFlags(wpi::StringRef key) const {
  return GetEntry(key).GetFlags();
}

void NetworkTable::Delete(const wpi::Twine& key) {
  GetEntry(key).Delete();
}

bool NetworkTable::PutNumber(wpi::StringRef key, double value) {
  return GetEntry(key).SetDouble(value);
}

bool NetworkTable::SetDefaultNumber(wpi::StringRef key, double defaultValue) {
  return GetEntry(key).SetDefaultDouble(defaultValue);
}

double NetworkTable::GetNumber(wpi::StringRef key, double defaultValue) const {
  return GetEntry(key).GetDouble(defaultValue);
}

bool NetworkTable::PutString(wpi::StringRef key, wpi::StringRef value) {
  return GetEntry(key).SetString(value);
}

bool NetworkTable::SetDefaultString(wpi::StringRef key,
                                    wpi::StringRef defaultValue) {
  return GetEntry(key).SetDefaultString(defaultValue);
}

std::string NetworkTable::GetString(wpi::StringRef key,
                                    wpi::StringRef defaultValue) const {
  return GetEntry(key).GetString(defaultValue);
}

bool NetworkTable::PutBoolean(wpi::StringRef key, bool value) {
  return GetEntry(key).SetBoolean(value);
}

bool NetworkTable::SetDefaultBoolean(wpi::StringRef key, bool defaultValue) {
  return GetEntry(key).SetDefaultBoolean(defaultValue);
}

bool NetworkTable::GetBoolean(wpi::StringRef key, bool defaultValue) const {
  return GetEntry(key).GetBoolean(defaultValue);
}

bool NetworkTable::PutBooleanArray(wpi::StringRef key,
                                   wpi::ArrayRef<int> value) {
  return GetEntry(key).SetBooleanArray(value);
}

bool NetworkTable::SetDefaultBooleanArray(wpi::StringRef key,
                                          wpi::ArrayRef<int> defaultValue) {
  return GetEntry(key).SetDefaultBooleanArray(defaultValue);
}

std::vector<int> NetworkTable::GetBooleanArray(
    wpi::StringRef key, wpi::ArrayRef<int> defaultValue) const {
  return GetEntry(key).GetBooleanArray(defaultValue);
}

bool NetworkTable::PutNumberArray(wpi::StringRef key,
                                  wpi::ArrayRef<double> value) {
  return GetEntry(key).SetDoubleArray(value);
}

bool NetworkTable::SetDefaultNumberArray(wpi::StringRef key,
                                         wpi::ArrayRef<double> defaultValue) {
  return GetEntry(key).SetDefaultDoubleArray(defaultValue);
}

std::vector<double> NetworkTable::GetNumberArray(
    wpi::StringRef key, wpi::ArrayRef<double> defaultValue) const {
  return GetEntry(key).GetDoubleArray(defaultValue);
}

bool NetworkTable::PutStringArray(wpi::StringRef key,
                                  wpi::ArrayRef<std::string> value) {
  return GetEntry(key).SetStringArray(value);
}

bool NetworkTable::SetDefaultStringArray(
    wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue) {
  return GetEntry(key).SetDefaultStringArray(defaultValue);
}

std::vector<std::string> NetworkTable::GetStringArray(
    wpi::StringRef key, wpi::ArrayRef<std::string> defaultValue) const {
  return GetEntry(key).GetStringArray(defaultValue);
}

bool NetworkTable::PutRaw(wpi::StringRef key, wpi::StringRef value) {
  return GetEntry(key).SetRaw(value);
}

bool NetworkTable::SetDefaultRaw(wpi::StringRef key,
                                 wpi::StringRef defaultValue) {
  return GetEntry(key).SetDefaultRaw(defaultValue);
}

std::string NetworkTable::GetRaw(wpi::StringRef key,
                                 wpi::StringRef defaultValue) const {
  return GetEntry(key).GetRaw(defaultValue);
}

bool NetworkTable::PutValue(const wpi::Twine& key,
                            std::shared_ptr<Value> value) {
  return GetEntry(key).SetValue(value);
}

bool NetworkTable::SetDefaultValue(const wpi::Twine& key,
                                   std::shared_ptr<Value> defaultValue) {
  return GetEntry(key).SetDefaultValue(defaultValue);
}

std::shared_ptr<Value> NetworkTable::GetValue(const wpi::Twine& key) const {
  return GetEntry(key).GetValue();
}

wpi::StringRef NetworkTable::GetPath() const {
  return m_path;
}

const char* NetworkTable::SaveEntries(const wpi::Twine& filename) const {
  return nt::SaveEntries(m_inst, filename,
                         m_path + wpi::Twine(PATH_SEPARATOR_CHAR));
}

const char* NetworkTable::LoadEntries(
    const wpi::Twine& filename,
    std::function<void(size_t line, const char* msg)> warn) {
  return nt::LoadEntries(m_inst, filename,
                         m_path + wpi::Twine(PATH_SEPARATOR_CHAR), warn);
}
