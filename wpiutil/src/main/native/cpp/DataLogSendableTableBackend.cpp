// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/DataLogSendableTableBackend.h"

#include <variant>

#include "wpi/json.h"
#include "wpi/sendable2/SendableWrapper.h"
#include "wpi/SmallString.h"

using namespace wpi::log;

struct DataLogSendableTableBackend::EntryData
    : public std::enable_shared_from_this<EntryData> {
  EntryData(DataLog& log, std::string_view path);

  template <typename LogEntryType>
  LogEntryType* Init() {
    std::scoped_lock lock{m_mutex};
    if (m_entry.index() == 0) {
      m_entry = LogEntryType{m_log, m_path, m_propertiesStr};
    }
    return std::get_if<LogEntryType>(&m_entry);
  }

  template <typename LogEntryType, typename T>
  void Set(T value) {
    if (auto entry = std::get_if<LogEntryType>(&m_entry)) {
      if (m_appendAll) {
        entry.Append(value);
      } else {
        entry.Update(value);
      }
    }
  }

  template <typename LogEntryType, typename T>
  void InitAndSet(T value) {
    if (Init<LogEntryType>()) {
      Set<LogEntryType>(value);
    }
  }

  template <typename LogEntryType, typename T>
  void InitAndPublish(std::function<T()> supplier) {
    if (Init<LogEntryType>()) {
      SetPolledUpdate([supplier = std::move(supplier)](auto& entry) {
        entry.template Set<LogEntryType>(supplier());
      });
    }
  }

  template <typename LogEntryType, typename T>
  std::function<void(T)> AddPublisher() {
    auto entry = Init<LogEntryType>();
    if (!entry) {
      return [](auto) {};
    }
    return [weak = weak_from_this()](auto value) {
      if (auto self = weak.lock()) {
        self->Set<LogEntryType>(value);
      }
    };
  }

  void SetPolledUpdate(std::function<void(EntryData&)> function) {
    std::scoped_lock lock{m_mutex};
    m_polledUpdate = std::move(function);
  }

  std::variant<std::monostate, RawLogEntry, BooleanLogEntry, IntegerLogEntry,
               FloatLogEntry, DoubleLogEntry, StringLogEntry,
               BooleanArrayLogEntry, IntegerArrayLogEntry, FloatArrayLogEntry,
               DoubleArrayLogEntry, StringArrayLogEntry>
      m_entry;
  DataLog& m_log;
  wpi::mutex m_mutex;
  std::string m_path;
  std::string m_typeString;
  wpi::json m_properties;
  std::string m_propertiesStr;
  std::function<void(EntryData&)> m_polledUpdate;
  bool m_appendAll{false};
};

DataLogSendableTableBackend::~DataLogSendableTableBackend() = default;

void DataLogSendableTableBackend::SetBoolean(std::string_view name,
                                             bool value) {
  GetOrNew(name).InitAndSet<BooleanLogEntry>(value);
}

void DataLogSendableTableBackend::SetInteger(std::string_view name,
                                             int64_t value) {
  GetOrNew(name).InitAndSet<IntegerLogEntry>(value);
}

void DataLogSendableTableBackend::SetFloat(std::string_view name, float value) {
  GetOrNew(name).InitAndSet<FloatLogEntry>(value);
}

void DataLogSendableTableBackend::SetDouble(std::string_view name,
                                            double value) {
  GetOrNew(name).InitAndSet<DoubleLogEntry>(value);
}

void DataLogSendableTableBackend::SetString(std::string_view name,
                                            std::string_view value) {
  GetOrNew(name).InitAndSet<StringLogEntry>(value);
}

void DataLogSendableTableBackend::SetRaw(std::string_view name,
                                         std::string_view typeString,
                                         std::span<const uint8_t> value);

void DataLogSendableTableBackend::PublishBoolean(
    std::string_view name, std::function<bool()> supplier) {
  GetOrNew(name).InitAndPublish<BooleanLogEntry>(std::move(supplier));
}

void DataLogSendableTableBackend::PublishInteger(
    std::string_view name, std::function<int64_t()> supplier) {
  GetOrNew(name).InitAndPublish<IntegerLogEntry>(std::move(supplier));
}

void DataLogSendableTableBackend::PublishFloat(
    std::string_view name, std::function<float()> supplier) {
  GetOrNew(name).InitAndPublish<FloatLogEntry>(std::move(supplier));
}

void DataLogSendableTableBackend::PublishDouble(
    std::string_view name, std::function<double()> supplier) {
  GetOrNew(name).InitAndPublish<DoubleLogEntry>(std::move(supplier));
}

void DataLogSendableTableBackend::PublishString(
    std::string_view name, std::function<std::string()> supplier) {
  GetOrNew(name).InitAndPublish<StringLogEntry>(std::move(supplier));
}

void DataLogSendableTableBackend::PublishRaw(
    std::string_view name, std::string_view typeString,
    std::function<std::vector<uint8_t>()> supplier);

void DataLogSendableTableBackend::PublishRawSmall(
    std::string_view name, std::string_view typeString,
    std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
        supplier);

[[nodiscard]]
std::function<void(bool)> DataLogSendableTableBackend::AddBooleanPublisher(
    std::string_view name) {
  return GetOrNew(name).AddPublisher<BooleanLogEntry, bool>();
}

[[nodiscard]]
std::function<void(int64_t)> DataLogSendableTableBackend::AddIntegerPublisher(
    std::string_view name) {
  return GetOrNew(name).AddPublisher<IntegerLogEntry, int64_t>();
}

[[nodiscard]]
std::function<void(float)> DataLogSendableTableBackend::AddFloatPublisher(
    std::string_view name) {
  return GetOrNew(name).AddPublisher<FloatLogEntry, float>();
}

[[nodiscard]]
std::function<void(double)> DataLogSendableTableBackend::AddDoublePublisher(
    std::string_view name) {
  return GetOrNew(name).AddPublisher<DoubleLogEntry, double>();
}

[[nodiscard]]
std::function<void(std::string_view)>
DataLogSendableTableBackend::AddStringPublisher(std::string_view name) {
  return GetOrNew(name).AddPublisher<StringLogEntry, std::string_view>();
}

[[nodiscard]]
std::function<void(std::span<const uint8_t>)>
DataLogSendableTableBackend::AddRawPublisher(std::string_view name,
                                             std::string_view typeString);

void DataLogSendableTableBackend::SubscribeBoolean(
    std::string_view name, std::function<void(bool)> consumer) {}

void DataLogSendableTableBackend::SubscribeInteger(
    std::string_view name, std::function<void(int64_t)> consumer) {}

void DataLogSendableTableBackend::SubscribeFloat(
    std::string_view name, std::function<void(float)> consumer) {}

void DataLogSendableTableBackend::SubscribeDouble(
    std::string_view name, std::function<void(double)> consumer) {}

void DataLogSendableTableBackend::SubscribeString(
    std::string_view name, std::function<void(std::string_view)> consumer) {}

void DataLogSendableTableBackend::SubscribeRaw(
    std::string_view name, std::string_view typeString,
    std::function<void(std::span<const uint8_t>)> consumer) {}

std::shared_ptr<wpi2::SendableTableBackend>
DataLogSendableTableBackend::CreateSendable(
    std::string_view name, std::unique_ptr<wpi2::SendableWrapper> sendable) {
  auto child = GetChildInternal(name);
  std::scoped_lock lock{child->m_mutex};
  if (!child->m_sendable) {
    wpi2::SendableTable table{child};
    sendable->Init(table);
    child->m_sendable = std::move(sendable);
  }
  return child;
}

std::shared_ptr<wpi2::SendableTableBackend>
DataLogSendableTableBackend::GetChild(std::string_view name) {
  return GetChildInternal(name);
}

void DataLogSendableTableBackend::SetPublishOptions(
    const wpi2::SendableOptions& options) {
  // TODO
}

void DataLogSendableTableBackend::SetSubscribeOptions(
    const wpi2::SendableOptions& options) {}

wpi::json DataLogSendableTableBackend::GetProperty(
    std::string_view name, std::string_view propName) const {
  auto entryIt = m_entries.find(name);
  if (entryIt == m_entries.end()) {
    return {};
  }
  std::scoped_lock lock{entryIt->second->m_mutex};
  auto valueIt = entryIt->second->m_properties.find(propName);
  if (valueIt == entryIt->second->m_properties.end()) {
    return {};
  }
  return valueIt.value();
}

void DataLogSendableTableBackend::SetProperty(std::string_view name,
                                              std::string_view propName,
                                              const wpi::json& value) {
  auto& data = GetOrNew(name);
  std::scoped_lock lock{data.m_mutex};
  data.m_properties[propName] = value;
  data.RefreshProperties();
}

void DataLogSendableTableBackend::DeleteProperty(std::string_view name,
                                                 std::string_view propName) {
  auto& data = GetOrNew(name);
  std::scoped_lock lock{data.m_mutex};
  data.m_properties.erase(propName);
  data.RefreshProperties();
}

wpi::json DataLogSendableTableBackend::GetProperties(
    std::string_view name) const {
  auto entryIt = m_entries.find(name);
  if (entryIt == m_entries.end()) {
    return wpi::json::object();
  }
  std::scoped_lock lock{entryIt->second->m_mutex};
  return entryIt->second->m_properties;
}

bool DataLogSendableTableBackend::SetProperties(std::string_view name,
                                                const wpi::json& properties) {
  if (!properties.is_object()) {
    return false;
  }
  if (properties.empty()) {
    return true;
  }
  auto& data = GetOrNew(name);
  std::scoped_lock lock{data.m_mutex};
  for (auto&& [key, value] : properties.items()) {
    if (value.is_null()) {
      data.m_properties.erase(key);
    } else {
      data.m_properties[key] = value;
    }
  }
  data.RefreshProperties();
}

void DataLogSendableTableBackend::Remove(std::string_view name);

bool DataLogSendableTableBackend::IsPublished() const;

void DataLogSendableTableBackend::Update();

void DataLogSendableTableBackend::Clear();

bool DataLogSendableTableBackend::HasSchema(std::string_view name) const;

void DataLogSendableTableBackend::AddSchema(std::string_view name,
                                            std::string_view type,
                                            std::span<const uint8_t> schema);

void DataLogSendableTableBackend::AddSchema(std::string_view name,
                                            std::string_view type,
                                            std::string_view schema);

DataLogSendableTableBackend::EntryData& DataLogSendableTableBackend::GetOrNew(
    std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto& entry = m_entries[name];
  if (!entry) {
    wpi::SmallString<128> path{m_path};
    path += '/';
    path += name;
    entry = std::make_unique<EntryData>(m_log, path);
  }
  return entry;
}

std::shared_ptr<DataLogSendableTableBackend>
DataLogSendableTableBackend::GetChildInternal(std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto& child = m_tables[name];
  if (!child) {
    wpi::SmallString<128> path{m_path};
    path += '/';
    path += name;
    child = std::make_shared<DataLogSendableTableBackend>(m_log, path);
  }
  return child;
}
