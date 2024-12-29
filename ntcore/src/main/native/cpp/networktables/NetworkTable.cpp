// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "networktables/NetworkTable.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>

#include "networktables/BooleanArrayTopic.h"
#include "networktables/BooleanTopic.h"
#include "networktables/DoubleArrayTopic.h"
#include "networktables/DoubleTopic.h"
#include "networktables/FloatArrayTopic.h"
#include "networktables/FloatTopic.h"
#include "networktables/IntegerArrayTopic.h"
#include "networktables/IntegerTopic.h"
#include "networktables/NetworkTableInstance.h"
#include "networktables/RawTopic.h"
#include "networktables/StringArrayTopic.h"
#include "networktables/StringTopic.h"
#include "ntcore.h"
#include "ntcore_cpp.h"

using namespace nt;

std::string_view NetworkTable::BasenameKey(std::string_view key) {
  size_t slash = key.rfind(PATH_SEPARATOR_CHAR);
  if (slash == std::string_view::npos) {
    return key;
  }
  return wpi::substr(key, slash + 1);
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

NetworkTable::~NetworkTable() = default;

NetworkTableInstance NetworkTable::GetInstance() const {
  return NetworkTableInstance{m_inst};
}

NetworkTableEntry NetworkTable::GetEntry(std::string_view key) const {
  std::scoped_lock lock(m_mutex);
  NT_Entry& entry = m_entries[key];
  if (entry == 0) {
    fmt::memory_buffer buf;
    fmt::format_to(fmt::appender{buf}, "{}/{}", m_path, key);
    entry = nt::GetEntry(m_inst, {buf.data(), buf.size()});
  }
  return NetworkTableEntry{entry};
}

Topic NetworkTable::GetTopic(std::string_view name) const {
  fmt::memory_buffer buf;
  fmt::format_to(fmt::appender{buf}, "{}/{}", m_path, name);
  return Topic{::nt::GetTopic(m_inst, {buf.data(), buf.size()})};
}

BooleanTopic NetworkTable::GetBooleanTopic(std::string_view name) const {
  return BooleanTopic{GetTopic(name)};
}

IntegerTopic NetworkTable::GetIntegerTopic(std::string_view name) const {
  return IntegerTopic{GetTopic(name)};
}

FloatTopic NetworkTable::GetFloatTopic(std::string_view name) const {
  return FloatTopic{GetTopic(name)};
}

DoubleTopic NetworkTable::GetDoubleTopic(std::string_view name) const {
  return DoubleTopic{GetTopic(name)};
}

StringTopic NetworkTable::GetStringTopic(std::string_view name) const {
  return StringTopic{GetTopic(name)};
}

RawTopic NetworkTable::GetRawTopic(std::string_view name) const {
  return RawTopic{GetTopic(name)};
}

BooleanArrayTopic NetworkTable::GetBooleanArrayTopic(
    std::string_view name) const {
  return BooleanArrayTopic{GetTopic(name)};
}

IntegerArrayTopic NetworkTable::GetIntegerArrayTopic(
    std::string_view name) const {
  return IntegerArrayTopic{GetTopic(name)};
}

FloatArrayTopic NetworkTable::GetFloatArrayTopic(std::string_view name) const {
  return FloatArrayTopic{GetTopic(name)};
}

DoubleArrayTopic NetworkTable::GetDoubleArrayTopic(
    std::string_view name) const {
  return DoubleArrayTopic{GetTopic(name)};
}

StringArrayTopic NetworkTable::GetStringArrayTopic(
    std::string_view name) const {
  return StringArrayTopic{GetTopic(name)};
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
  return GetTopic(key).Exists();
}

bool NetworkTable::ContainsSubTable(std::string_view key) const {
  return !::nt::GetTopics(m_inst, fmt::format("{}/{}/", m_path, key), 0)
              .empty();
}

std::vector<TopicInfo> NetworkTable::GetTopicInfo(int types) const {
  std::vector<TopicInfo> infos;
  size_t prefix_len = m_path.size() + 1;
  for (auto&& info :
       ::nt::GetTopicInfo(m_inst, fmt::format("{}/", m_path), types)) {
    auto relative_key = wpi::substr(info.name, prefix_len);
    if (relative_key.find(PATH_SEPARATOR_CHAR) != std::string_view::npos) {
      continue;
    }
    infos.emplace_back(std::move(info));
  }
  return infos;
}

std::vector<Topic> NetworkTable::GetTopics(int types) const {
  std::vector<Topic> topics;
  size_t prefix_len = m_path.size() + 1;
  for (auto&& info :
       ::nt::GetTopicInfo(m_inst, fmt::format("{}/", m_path), types)) {
    auto relative_key = wpi::substr(info.name, prefix_len);
    if (relative_key.find(PATH_SEPARATOR_CHAR) != std::string_view::npos) {
      continue;
    }
    topics.emplace_back(info.topic);
  }
  return topics;
}

std::vector<std::string> NetworkTable::GetKeys(int types) const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  for (auto&& info :
       ::nt::GetTopicInfo(m_inst, fmt::format("{}/", m_path), types)) {
    auto relative_key = wpi::substr(info.name, prefix_len);
    if (relative_key.find(PATH_SEPARATOR_CHAR) != std::string_view::npos) {
      continue;
    }
    keys.emplace_back(relative_key);
  }
  return keys;
}

std::vector<std::string> NetworkTable::GetSubTables() const {
  std::vector<std::string> keys;
  size_t prefix_len = m_path.size() + 1;
  for (auto&& topic :
       ::nt::GetTopicInfo(m_inst, fmt::format("{}/", m_path), 0)) {
    auto relative_key = wpi::substr(topic.name, prefix_len);
    size_t end_subtable = relative_key.find(PATH_SEPARATOR_CHAR);
    if (end_subtable == std::string_view::npos) {
      continue;
    }
    auto subTable = wpi::substr(relative_key, 0, end_subtable);
    if (keys.empty() || keys.back() != subTable) {
      keys.emplace_back(subTable);
    }
  }
  // remove duplicates
  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
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
                                   std::span<const int> value) {
  return GetEntry(key).SetBooleanArray(value);
}

bool NetworkTable::SetDefaultBooleanArray(std::string_view key,
                                          std::span<const int> defaultValue) {
  return GetEntry(key).SetDefaultBooleanArray(defaultValue);
}

std::vector<int> NetworkTable::GetBooleanArray(
    std::string_view key, std::span<const int> defaultValue) const {
  return GetEntry(key).GetBooleanArray(defaultValue);
}

bool NetworkTable::PutNumberArray(std::string_view key,
                                  std::span<const double> value) {
  return GetEntry(key).SetDoubleArray(value);
}

bool NetworkTable::SetDefaultNumberArray(std::string_view key,
                                         std::span<const double> defaultValue) {
  return GetEntry(key).SetDefaultDoubleArray(defaultValue);
}

std::vector<double> NetworkTable::GetNumberArray(
    std::string_view key, std::span<const double> defaultValue) const {
  return GetEntry(key).GetDoubleArray(defaultValue);
}

bool NetworkTable::PutStringArray(std::string_view key,
                                  std::span<const std::string> value) {
  return GetEntry(key).SetStringArray(value);
}

bool NetworkTable::SetDefaultStringArray(
    std::string_view key, std::span<const std::string> defaultValue) {
  return GetEntry(key).SetDefaultStringArray(defaultValue);
}

std::vector<std::string> NetworkTable::GetStringArray(
    std::string_view key, std::span<const std::string> defaultValue) const {
  return GetEntry(key).GetStringArray(defaultValue);
}

bool NetworkTable::PutRaw(std::string_view key,
                          std::span<const uint8_t> value) {
  return GetEntry(key).SetRaw(value);
}

bool NetworkTable::SetDefaultRaw(std::string_view key,
                                 std::span<const uint8_t> defaultValue) {
  return GetEntry(key).SetDefaultRaw(defaultValue);
}

std::vector<uint8_t> NetworkTable::GetRaw(
    std::string_view key, std::span<const uint8_t> defaultValue) const {
  return GetEntry(key).GetRaw(defaultValue);
}

bool NetworkTable::PutValue(std::string_view key, const Value& value) {
  return GetEntry(key).SetValue(value);
}

bool NetworkTable::SetDefaultValue(std::string_view key,
                                   const Value& defaultValue) {
  return GetEntry(key).SetDefaultValue(defaultValue);
}

Value NetworkTable::GetValue(std::string_view key) const {
  return GetEntry(key).GetValue();
}

std::string_view NetworkTable::GetPath() const {
  return m_path;
}

NT_Listener NetworkTable::AddListener(int eventMask,
                                      TableEventListener listener) {
  return NetworkTableInstance{m_inst}.AddListener(
      {{fmt::format("{}/", m_path)}}, eventMask,
      [this, cb = std::move(listener)](const Event& event) {
        std::string topicNameStr;
        std::string_view topicName;
        if (auto topicInfo = event.GetTopicInfo()) {
          topicName = topicInfo->name;
        } else if (auto valueData = event.GetValueEventData()) {
          topicNameStr = Topic{valueData->topic}.GetName();
          topicName = topicNameStr;
        } else {
          return;
        }
        auto relative_key = wpi::substr(topicName, m_path.size() + 1);
        if (relative_key.find(PATH_SEPARATOR_CHAR) != std::string_view::npos) {
          return;
        }
        cb(this, relative_key, event);
      });
}

NT_Listener NetworkTable::AddListener(std::string_view key, int eventMask,
                                      TableEventListener listener) {
  return NetworkTableInstance{m_inst}.AddListener(
      GetEntry(key), eventMask,
      [this, cb = std::move(listener),
       key = std::string{key}](const Event& event) { cb(this, key, event); });
}

NT_Listener NetworkTable::AddSubTableListener(SubTableListener listener) {
  // The lambda needs to be copyable, but StringMap is not, so use
  // a shared_ptr to it.
  auto notified_tables = std::make_shared<wpi::StringMap<char>>();

  return NetworkTableInstance{m_inst}.AddListener(
      {{fmt::format("{}/", m_path)}}, NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE,
      [this, cb = std::move(listener), notified_tables](const Event& event) {
        auto topicInfo = event.GetTopicInfo();
        if (!topicInfo) {
          return;
        }
        auto relative_key = wpi::substr(topicInfo->name, m_path.size() + 1);
        auto end_sub_table = relative_key.find(PATH_SEPARATOR_CHAR);
        if (end_sub_table == std::string_view::npos) {
          return;
        }
        auto sub_table_key = relative_key.substr(0, end_sub_table);
        if (notified_tables->find(sub_table_key) != notified_tables->end()) {
          return;
        }
        notified_tables->insert(std::pair{sub_table_key, '\0'});
        cb(this, sub_table_key, this->GetSubTable(sub_table_key));
      });
}

void NetworkTable::RemoveListener(NT_Listener listener) {
  NetworkTableInstance{m_inst}.RemoveListener(listener);
}
