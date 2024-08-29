// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/DataLogSendableTableBackend.h"

#include <variant>

#include "wpi/json.h"
#include "wpi/sendable2/SendableTable.h"
#include "wpi/SmallString.h"

using namespace wpi::log;

struct DataLogSendableTableBackend::EntryData
    : public std::enable_shared_from_this<EntryData> {
  EntryData(DataLog& log, std::string_view path) : m_log{log}, m_path{path} {}

  template <typename LogEntryType>
  LogEntryType* Init() {
    std::scoped_lock lock{m_mutex};
    if (m_entry.index() == 0) {
      m_entry = LogEntryType{m_log, m_path, m_propertiesStr};
    }
    return std::get_if<LogEntryType>(&m_entry);
  }

  RawLogEntry* InitRaw(std::string_view typeString) {
    std::scoped_lock lock{m_mutex};
    if (m_entry.index() == 0) {
      m_entry = RawLogEntry{m_log, m_path, m_propertiesStr, typeString};
      m_typeString = typeString;
    } else if (m_typeString != typeString) {
      return nullptr;
    }
    return std::get_if<RawLogEntry>(&m_entry);
  }

  template <typename LogEntryType, typename T>
  void Set(T value) {
    if (auto entry = std::get_if<LogEntryType>(&m_entry)) {
      if (m_appendAll) {
        entry->Append(value);
      } else {
        entry->Update(value);
      }
    }
  }

  template <typename LogEntryType, typename T>
  void InitAndSet(T value) {
    if (Init<LogEntryType>()) {
      Set<LogEntryType>(value);
    }
  }

  void InitAndSetRaw(std::string_view typeString,
                     std::span<const uint8_t> value) {
    if (InitRaw(typeString)) {
      Set<RawLogEntry>(value);
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

  void InitAndPublishRaw(std::string_view typeString,
                         std::function<std::vector<uint8_t>()> supplier) {
    if (InitRaw(typeString)) {
      SetPolledUpdate([supplier = std::move(supplier)](auto& entry) {
        entry.template Set<RawLogEntry>(supplier());
      });
    }
  }

  void InitAndPublishRawSmall(
      std::string_view typeString,
      std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>&)>
          supplier) {
    if (InitRaw(typeString)) {
      SetPolledUpdate([supplier = std::move(supplier)](auto& entry) {
        wpi::SmallVector<uint8_t, 128> buf;
        entry.template Set<RawLogEntry>(supplier(buf));
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

  std::function<void(std::span<const uint8_t>)> AddPublisherRaw(
      std::string_view typeString) {
    auto entry = InitRaw(typeString);
    if (!entry) {
      return [](auto) {};
    }
    return [weak = weak_from_this()](auto value) {
      if (auto self = weak.lock()) {
        self->Set<RawLogEntry>(value);
      }
    };
  }

  template <typename LogEntryType>
  LogEntryType* Get() {
    std::scoped_lock lock{m_mutex};
    if (m_entry.index() == 0) {
      return nullptr;
    }
    return std::get_if<LogEntryType>(&m_entry);
  }

  void SetPolledUpdate(std::function<void(EntryData&)> function) {
    std::scoped_lock lock{m_mutex};
    m_polledUpdate = std::move(function);
  }

  void RefreshProperties();

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

void DataLogSendableTableBackend::EntryData::RefreshProperties() {
  m_propertiesStr.clear();
  wpi::raw_string_ostream os{m_propertiesStr};
  m_properties.dump(os);
  if (auto entry = std::get_if<RawLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<BooleanLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<BooleanLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<IntegerLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<FloatLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<DoubleLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<StringLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<BooleanArrayLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<IntegerArrayLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<FloatArrayLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<DoubleArrayLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  } else if (auto entry = std::get_if<StringArrayLogEntry>(&m_entry)) {
    entry->SetMetadata(m_propertiesStr);
  }
}

DataLogSendableTableBackend::DataLogSendableTableBackend(DataLog& log,
                                                         std::string_view path)
    : m_log{log}, m_path{path} {}

DataLogSendableTableBackend::~DataLogSendableTableBackend() = default;

bool DataLogSendableTableBackend::GetBoolean(std::string_view name,
                                             bool defaultValue) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(name);
  if (it != m_entries.end()) {
    if (auto entry = it->second->Get<BooleanLogEntry>()) {
      if (auto val = entry->GetLastValue()) {
        return *val;
      }
    }
  }
  return defaultValue;
}

int64_t DataLogSendableTableBackend::GetInteger(std::string_view name,
                                                int64_t defaultValue) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(name);
  if (it != m_entries.end()) {
    if (auto entry = it->second->Get<IntegerLogEntry>()) {
      if (auto val = entry->GetLastValue()) {
        return *val;
      }
    }
  }
  return defaultValue;
}

float DataLogSendableTableBackend::GetFloat(std::string_view name,
                                            float defaultValue) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(name);
  if (it != m_entries.end()) {
    if (auto entry = it->second->Get<FloatLogEntry>()) {
      if (auto val = entry->GetLastValue()) {
        return *val;
      }
    }
  }
  return defaultValue;
}

double DataLogSendableTableBackend::GetDouble(std::string_view name,
                                              double defaultValue) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(name);
  if (it != m_entries.end()) {
    if (auto entry = it->second->Get<DoubleLogEntry>()) {
      if (auto val = entry->GetLastValue()) {
        return *val;
      }
    }
  }
  return defaultValue;
}

std::string DataLogSendableTableBackend::GetString(
    std::string_view name, std::string_view defaultValue) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(name);
  if (it != m_entries.end()) {
    if (auto entry = it->second->Get<StringLogEntry>()) {
      if (auto val = entry->GetLastValue()) {
        return *val;
      }
    }
  }
  return {defaultValue.data(), defaultValue.size()};
}

std::vector<uint8_t> DataLogSendableTableBackend::GetRaw(
    std::string_view name, std::string_view typeString,
    std::span<const uint8_t> defaultValue) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(name);
  if (it != m_entries.end()) {
    if (auto entry = it->second->Get<RawLogEntry>()) {
      if (auto val = entry->GetLastValue()) {
        return *val;
      }
    }
  }
  return {defaultValue.begin(), defaultValue.end()};
}

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
                                         std::span<const uint8_t> value) {
  GetOrNew(name).InitAndSetRaw(typeString, value);
}

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
    std::function<std::vector<uint8_t>()> supplier) {
  GetOrNew(name).InitAndPublishRaw(typeString, std::move(supplier));
}

void DataLogSendableTableBackend::PublishRawSmall(
    std::string_view name, std::string_view typeString,
    std::function<std::span<uint8_t>(wpi::SmallVectorImpl<uint8_t>& buf)>
        supplier) {
  GetOrNew(name).InitAndPublishRawSmall(typeString, std::move(supplier));
}

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
                                             std::string_view typeString) {
  return GetOrNew(name).AddPublisherRaw(typeString);
}

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
    std::string_view name, const wpi2::SendableOptions& options) {
  auto& entry = GetOrNew(name);
  if (options.keepDuplicates) {
    entry.m_appendAll = true;
  }
}

void DataLogSendableTableBackend::SetSubscribeOptions(
    std::string_view name, const wpi2::SendableOptions& options) {}

wpi::json DataLogSendableTableBackend::GetProperty(
    std::string_view name, std::string_view propName) const {
  std::scoped_lock lock{m_mutex};
  auto entryIt = m_entries.find(name);
  if (entryIt == m_entries.end()) {
    return {};
  }
  std::scoped_lock lock2{entryIt->second->m_mutex};
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
  std::scoped_lock lock{m_mutex};
  auto entryIt = m_entries.find(name);
  if (entryIt == m_entries.end()) {
    return wpi::json::object();
  }
  std::scoped_lock lock2{entryIt->second->m_mutex};
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
  return true;
}

void DataLogSendableTableBackend::Remove(std::string_view name) {
  std::scoped_lock lock{m_mutex};
  m_entries.erase(name);
  m_tables.erase(name);
}

bool DataLogSendableTableBackend::IsPublished() const {
  return true;
}

void DataLogSendableTableBackend::Update() {
  std::scoped_lock lock{m_mutex};
  for (auto&& data : m_entries) {
    std::scoped_lock lock{data.second->m_mutex};
    auto& consumer = data.second->m_polledUpdate;
    if (consumer &&
        !std::holds_alternative<std::monostate>(data.second->m_entry)) {
      consumer(*data.second);
    }
  }
  for (auto&& table : m_tables) {
    table.second->Update();
  }
}

void DataLogSendableTableBackend::Clear() {
  std::scoped_lock lock{m_mutex};
  m_entries.clear();
  m_tables.clear();
}

bool DataLogSendableTableBackend::HasSchema(std::string_view name) const {
  return m_log.HasSchema(name);
}

void DataLogSendableTableBackend::AddSchema(std::string_view name,
                                            std::string_view type,
                                            std::span<const uint8_t> schema) {
  m_log.AddSchema(name, type, schema);
}

void DataLogSendableTableBackend::AddSchema(std::string_view name,
                                            std::string_view type,
                                            std::string_view schema) {
  m_log.AddSchema(name, type, schema);
}

DataLogSendableTableBackend::EntryData& DataLogSendableTableBackend::GetOrNew(
    std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto& entry = m_entries[name];
  if (!entry) {
    wpi::SmallString<128> path{m_path};
    path += '/';
    path += name;
    entry = std::make_shared<EntryData>(m_log, path);
  }
  return *entry;
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
