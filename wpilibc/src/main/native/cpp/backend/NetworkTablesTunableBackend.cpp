// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTunableBackend.hpp"

#include <stdint.h>

#include <algorithm>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/detail/TunableBase.hpp"
#include "wpi/tunable/detail/TunableDetail.hpp"
#include "wpi/tunable/detail/TunableMember.hpp"
#include "wpi/tunable/detail/TunableTypeValue.hpp"
#include "wpi/util/StringExtras.hpp"
#include "wpi/util/json.hpp"

using namespace wpi;
using namespace wpi::backend;

namespace {

std::string_view GetTypeString(std::string_view defaultType,
                               const TunableConfig* config) {
  if (config && config->typeString) {
    return *config->typeString;
  }
  return defaultType;
}

const wpi::util::json& GetProperties(const TunableConfig* config) {
  static const wpi::util::json empty = wpi::util::json::object();
  return config ? config->properties : empty;
}

void NotifyOnTune(TunableRegistry::TunableInfo& info) {
  info.ResetChanged();
  if (auto config = info.config) {
    if (auto&& onTune = config->onTune) {
      onTune(*info.tunable, config->parent);
    }
  }
}

template <typename T>
const T& GetValue(const TunableRegistry::TunableInfo& info) {
  if (info.type == detail::GetTunableTypeValue<T, false>()) {
    return static_cast<detail::TunableValueBase<T>*>(info.tunable)->Get();
  }
  return static_cast<detail::TunableMemberValueBase<T>*>(info.tunable)
      ->Get(info.config->parent);
}

template <typename T>
void SetValue(const TunableRegistry::TunableInfo& info, T value) {
  if (info.type == detail::GetTunableTypeValue<T, false>()) {
    static_cast<detail::TunableValueBase<T>*>(info.tunable)
        ->Set(std::move(value));
  } else {
    static_cast<detail::TunableMemberValueBase<T>*>(info.tunable)
        ->Set(info.config->parent, std::move(value));
  }
}

std::vector<int> ToNtBooleanArray(const std::vector<bool>& values) {
  std::vector<int> result;
  result.reserve(values.size());
  for (bool value : values) {
    result.emplace_back(value ? 1 : 0);
  }
  return result;
}

std::vector<bool> FromNtBooleanArray(std::span<const int> values) {
  std::vector<bool> result;
  result.reserve(values.size());
  for (int value : values) {
    result.emplace_back(value != 0);
  }
  return result;
}

std::vector<int64_t> ToNtIntegerArray(const std::vector<int32_t>& values) {
  return {values.begin(), values.end()};
}

std::vector<int32_t> FromNtIntegerArray(std::span<const int64_t> values) {
  return {values.begin(), values.end()};
}

bool TypeMatches(const wpi::nt::Value& value, detail::TunableTypeValue type) {
  using enum detail::TunableTypeValue;
  switch (type) {
    case BOOLEAN:
    case MEMBER_BOOLEAN:
      return value.IsBoolean();
    case INT32:
    case INT64:
    case MEMBER_INT32:
    case MEMBER_INT64:
      return value.IsInteger();
    case FLOAT:
    case MEMBER_FLOAT:
      return value.IsFloat();
    case DOUBLE:
    case MEMBER_DOUBLE:
      return value.IsDouble();
    case STRING:
    case MEMBER_STRING:
      return value.IsString();
    case RAW:
    case STRUCT:
    case PROTOBUF:
    case MEMBER_RAW:
    case MEMBER_STRUCT:
    case MEMBER_PROTOBUF:
      return value.IsRaw();
    case BOOLEAN_ARRAY:
    case MEMBER_BOOLEAN_ARRAY:
      return value.IsBooleanArray();
    case INT32_ARRAY:
    case INT64_ARRAY:
    case MEMBER_INT32_ARRAY:
    case MEMBER_INT64_ARRAY:
      return value.IsIntegerArray();
    case FLOAT_ARRAY:
    case MEMBER_FLOAT_ARRAY:
      return value.IsFloatArray();
    case DOUBLE_ARRAY:
    case MEMBER_DOUBLE_ARRAY:
      return value.IsDoubleArray();
    case STRING_ARRAY:
    case MEMBER_STRING_ARRAY:
      return value.IsStringArray();
    default:
      return false;
  }
}

}  // namespace

class NetworkTablesTunableBackend::Entry {
 public:
  virtual ~Entry() = default;
  virtual uint32_t GetUid() const = 0;
  virtual void UpdateNetwork() = 0;
};

class NetworkTablesTunableBackend::ValueEntry : public Entry {
 public:
  ValueEntry(NetworkTablesTunableBackend* backend, std::string_view path,
             uint32_t uid, const TunableConfig* config,
             std::string_view typeString)
      : m_backend{backend}, m_uid{uid} {
    typeString = GetTypeString(typeString, config);
    if (config && config->robust) {
      m_publisher = backend->m_inst.GetTopic(fmt::format("{}/value", path))
                        .GenericPublishEx(typeString, GetProperties(config));
      m_subscriber = backend->m_inst.GetTopic(fmt::format("{}/tune", path))
                         .GenericSubscribe(typeString);
    } else {
      m_publisher =
          backend->m_inst.GetTopic(path).GenericPublishEx(typeString,
                                                          GetProperties(config));
      m_subscriber =
          backend->m_inst.GetTopic(path).GenericSubscribe(typeString);
    }
    backend->m_subscribers[m_subscriber.GetHandle()] = this;
    if (!config || config->isMutable) {
      m_listener = backend->m_poller.AddListener(
          m_subscriber, wpi::nt::EventFlags::VALUE_ALL);
    }
  }

  ~ValueEntry() override {
    if (m_listener != 0) {
      m_backend->m_poller.RemoveListener(m_listener);
    }
    m_backend->m_subscribers.erase(m_subscriber.GetHandle());
  }

  uint32_t GetUid() const override { return m_uid; }

  void UpdateTunable(const wpi::nt::Value& value) {
    auto info = TunableRegistry::GetTunable(m_uid);
    if (!info || !TypeMatches(value, info.type)) {
      return;
    }
    DoUpdateTunable(info, value);
    NotifyOnTune(info);
  }

 protected:
  virtual void DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                               const wpi::nt::Value& value) = 0;

  wpi::nt::GenericPublisher m_publisher;

 private:
  NetworkTablesTunableBackend* m_backend;
  uint32_t m_uid;
  wpi::nt::GenericSubscriber m_subscriber;
  NT_Listener m_listener = 0;
};

template <typename T>
class ValueTunableEntry final : public NetworkTablesTunableBackend::ValueEntry {
 public:
  using NetworkTablesTunableBackend::ValueEntry::ValueEntry;

  void UpdateNetwork() override {
    if (auto info = TunableRegistry::GetTunable(GetUid())) {
      SetNetworkValue(GetValue<T>(info));
      info.ResetChanged();
    }
  }

 private:
  void DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                       const wpi::nt::Value& value) override {
    SetValue<T>(info, GetNtValue(value));
  }

  void SetNetworkValue(const T& value);
  T GetNtValue(const wpi::nt::Value& value);
};

template <>
void ValueTunableEntry<bool>::SetNetworkValue(const bool& value) {
  m_publisher.SetBoolean(value);
}

template <>
bool ValueTunableEntry<bool>::GetNtValue(const wpi::nt::Value& value) {
  return value.GetBoolean();
}

template <>
void ValueTunableEntry<int32_t>::SetNetworkValue(const int32_t& value) {
  m_publisher.SetInteger(value);
}

template <>
int32_t ValueTunableEntry<int32_t>::GetNtValue(const wpi::nt::Value& value) {
  return value.GetInteger();
}

template <>
void ValueTunableEntry<int64_t>::SetNetworkValue(const int64_t& value) {
  m_publisher.SetInteger(value);
}

template <>
int64_t ValueTunableEntry<int64_t>::GetNtValue(const wpi::nt::Value& value) {
  return value.GetInteger();
}

template <>
void ValueTunableEntry<float>::SetNetworkValue(const float& value) {
  m_publisher.SetFloat(value);
}

template <>
float ValueTunableEntry<float>::GetNtValue(const wpi::nt::Value& value) {
  return value.GetFloat();
}

template <>
void ValueTunableEntry<double>::SetNetworkValue(const double& value) {
  m_publisher.SetDouble(value);
}

template <>
double ValueTunableEntry<double>::GetNtValue(const wpi::nt::Value& value) {
  return value.GetDouble();
}

template <>
void ValueTunableEntry<std::string>::SetNetworkValue(
    const std::string& value) {
  m_publisher.SetString(value);
}

template <>
std::string ValueTunableEntry<std::string>::GetNtValue(
    const wpi::nt::Value& value) {
  return std::string{value.GetString()};
}

template <>
void ValueTunableEntry<std::vector<uint8_t>>::SetNetworkValue(
    const std::vector<uint8_t>& value) {
  m_publisher.SetRaw(value);
}

template <>
std::vector<uint8_t> ValueTunableEntry<std::vector<uint8_t>>::GetNtValue(
    const wpi::nt::Value& value) {
  auto raw = value.GetRaw();
  return {raw.begin(), raw.end()};
}

template <>
void ValueTunableEntry<std::vector<bool>>::SetNetworkValue(
    const std::vector<bool>& value) {
  m_publisher.SetBooleanArray(ToNtBooleanArray(value));
}

template <>
std::vector<bool> ValueTunableEntry<std::vector<bool>>::GetNtValue(
    const wpi::nt::Value& value) {
  return FromNtBooleanArray(value.GetBooleanArray());
}

template <>
void ValueTunableEntry<std::vector<int32_t>>::SetNetworkValue(
    const std::vector<int32_t>& value) {
  m_publisher.SetIntegerArray(ToNtIntegerArray(value));
}

template <>
std::vector<int32_t> ValueTunableEntry<std::vector<int32_t>>::GetNtValue(
    const wpi::nt::Value& value) {
  return FromNtIntegerArray(value.GetIntegerArray());
}

template <>
void ValueTunableEntry<std::vector<int64_t>>::SetNetworkValue(
    const std::vector<int64_t>& value) {
  m_publisher.SetIntegerArray(value);
}

template <>
std::vector<int64_t> ValueTunableEntry<std::vector<int64_t>>::GetNtValue(
    const wpi::nt::Value& value) {
  auto arr = value.GetIntegerArray();
  return {arr.begin(), arr.end()};
}

template <>
void ValueTunableEntry<std::vector<float>>::SetNetworkValue(
    const std::vector<float>& value) {
  m_publisher.SetFloatArray(value);
}

template <>
std::vector<float> ValueTunableEntry<std::vector<float>>::GetNtValue(
    const wpi::nt::Value& value) {
  auto arr = value.GetFloatArray();
  return {arr.begin(), arr.end()};
}

template <>
void ValueTunableEntry<std::vector<double>>::SetNetworkValue(
    const std::vector<double>& value) {
  m_publisher.SetDoubleArray(value);
}

template <>
std::vector<double> ValueTunableEntry<std::vector<double>>::GetNtValue(
    const wpi::nt::Value& value) {
  auto arr = value.GetDoubleArray();
  return {arr.begin(), arr.end()};
}

template <>
void ValueTunableEntry<std::vector<std::string>>::SetNetworkValue(
    const std::vector<std::string>& value) {
  m_publisher.SetStringArray(value);
}

template <>
std::vector<std::string> ValueTunableEntry<std::vector<std::string>>::GetNtValue(
    const wpi::nt::Value& value) {
  auto arr = value.GetStringArray();
  return {arr.begin(), arr.end()};
}

class StructTunableEntry final : public NetworkTablesTunableBackend::ValueEntry {
 public:
  using NetworkTablesTunableBackend::ValueEntry::ValueEntry;

  void UpdateNetwork() override {
    auto info = TunableRegistry::GetTunable(GetUid());
    if (!info) {
      return;
    }
    if (info.type == detail::TunableTypeValue::STRUCT) {
      auto tunable = static_cast<detail::TunableStructBase*>(info.tunable);
      PublishSchemas(*tunable);
      std::vector<uint8_t> data(tunable->GetStructSize());
      tunable->PackStruct(data);
      m_publisher.SetRaw(data);
    } else {
      auto tunable = static_cast<detail::TunableMemberStructBase*>(
          info.tunable);
      PublishSchemas(*tunable);
      std::vector<uint8_t> data(tunable->GetStructSize(info.config->parent));
      tunable->PackStruct(info.config->parent, data);
      m_publisher.SetRaw(data);
    }
    info.ResetChanged();
  }

 private:
  void DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                       const wpi::nt::Value& value) override {
    if (info.type == detail::TunableTypeValue::STRUCT) {
      static_cast<detail::TunableStructBase*>(info.tunable)
          ->UnpackStruct(value.GetRaw());
    } else {
      static_cast<detail::TunableMemberStructBase*>(info.tunable)
          ->UnpackStruct(info.config->parent, value.GetRaw());
    }
  }

  void PublishSchemas(detail::TunableStructBase& tunable) {
    if (m_schemaPublished) {
      return;
    }
    tunable.ForEachStructSchema([this](std::string_view typeName,
                                       std::string_view schema) {
      m_publisher.GetTopic().GetInstance().AddSchema(typeName, "structschema",
                                                     schema);
    });
    m_schemaPublished = true;
  }

  void PublishSchemas(detail::TunableMemberStructBase& tunable) {
    if (m_schemaPublished) {
      return;
    }
    tunable.ForEachStructSchema([this](std::string_view typeName,
                                       std::string_view schema) {
      m_publisher.GetTopic().GetInstance().AddSchema(typeName, "structschema",
                                                     schema);
    });
    m_schemaPublished = true;
  }

  bool m_schemaPublished = false;
};

class ProtobufTunableEntry final
    : public NetworkTablesTunableBackend::ValueEntry {
 public:
  using NetworkTablesTunableBackend::ValueEntry::ValueEntry;

  void UpdateNetwork() override {
    auto info = TunableRegistry::GetTunable(GetUid());
    if (!info) {
      return;
    }
    std::vector<uint8_t> data;
    if (info.type == detail::TunableTypeValue::PROTOBUF) {
      auto tunable = static_cast<detail::TunableProtobufBase*>(info.tunable);
      PublishSchemas(*tunable);
      if (tunable->PackProtobuf(data)) {
        m_publisher.SetRaw(data);
      }
    } else {
      auto tunable = static_cast<detail::TunableMemberProtobufBase*>(
          info.tunable);
      PublishSchemas(*tunable);
      if (tunable->PackProtobuf(info.config->parent, data)) {
        m_publisher.SetRaw(data);
      }
    }
    info.ResetChanged();
  }

 private:
  void DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                       const wpi::nt::Value& value) override {
    if (info.type == detail::TunableTypeValue::PROTOBUF) {
      static_cast<detail::TunableProtobufBase*>(info.tunable)
          ->UnpackProtobuf(value.GetRaw());
    } else {
      static_cast<detail::TunableMemberProtobufBase*>(info.tunable)
          ->UnpackProtobuf(info.config->parent, value.GetRaw());
    }
  }

  void PublishSchemas(detail::TunableProtobufBase& tunable) {
    if (m_schemaPublished) {
      return;
    }
    auto inst = m_publisher.GetTopic().GetInstance();
    tunable.ForEachProtobufDescriptor(
        [&](std::string_view filename) { return inst.HasSchema(filename); },
        [&](std::string_view filename, std::string_view descriptor) {
          inst.AddSchema(filename, "proto:FileDescriptorProto", descriptor);
        });
    m_schemaPublished = true;
  }

  void PublishSchemas(detail::TunableMemberProtobufBase& tunable) {
    if (m_schemaPublished) {
      return;
    }
    auto inst = m_publisher.GetTopic().GetInstance();
    tunable.ForEachProtobufDescriptor(
        [&](std::string_view filename) { return inst.HasSchema(filename); },
        [&](std::string_view filename, std::string_view descriptor) {
          inst.AddSchema(filename, "proto:FileDescriptorProto", descriptor);
        });
    m_schemaPublished = true;
  }

  bool m_schemaPublished = false;
};

class ComplexTunableEntry final : public NetworkTablesTunableBackend::Entry {
 public:
  ComplexTunableEntry(wpi::nt::NetworkTableInstance inst,
                      std::string_view path, uint32_t uid,
                      detail::TunableBase& tunable)
      : m_uid{uid},
        m_typePublisher{inst.GetStringTopic(fmt::format("{}/.type", path))
                            .Publish()} {
    auto& complex = static_cast<ComplexTunable&>(tunable);
    m_typePublisher.Set(complex.GetTunableType());
  }

  uint32_t GetUid() const override { return m_uid; }

  void UpdateNetwork() override {
    if (auto info = TunableRegistry::GetTunable(m_uid)) {
      static_cast<ComplexTunable*>(info.tunable)->UpdateTunable();
      info.ResetChanged();
    }
  }

 private:
  uint32_t m_uid;
  wpi::nt::StringPublisher m_typePublisher;
};

NetworkTablesTunableBackend::NetworkTablesTunableBackend(
    wpi::nt::NetworkTableInstance inst, std::string_view prefix)
    : m_inst{inst}, m_prefix{prefix}, m_poller{inst} {}

NetworkTablesTunableBackend::~NetworkTablesTunableBackend() = default;

void NetworkTablesTunableBackend::Publish(std::string_view path, uint32_t uid,
                                          detail::TunableBase& tunable,
                                          const TunableConfig* config,
                                          detail::TunableTypeValue type) {
  std::scoped_lock lock{m_mutex};
  if (m_entries.contains(path)) {
    throw std::runtime_error(fmt::format("Tunable already exists: {}", path));
  }

  std::string ntPath = fmt::format("{}{}", m_prefix, path);
  std::unique_ptr<Entry> entry;
  using enum detail::TunableTypeValue;
  switch (type) {
    case BOOLEAN:
    case MEMBER_BOOLEAN:
      entry = std::make_unique<ValueTunableEntry<bool>>(
          this, ntPath, uid, config, "boolean");
      break;
    case INT32:
    case MEMBER_INT32:
      entry = std::make_unique<ValueTunableEntry<int32_t>>(
          this, ntPath, uid, config, "int");
      break;
    case INT64:
    case MEMBER_INT64:
      entry = std::make_unique<ValueTunableEntry<int64_t>>(
          this, ntPath, uid, config, "int");
      break;
    case FLOAT:
    case MEMBER_FLOAT:
      entry = std::make_unique<ValueTunableEntry<float>>(
          this, ntPath, uid, config, "float");
      break;
    case DOUBLE:
    case MEMBER_DOUBLE:
      entry = std::make_unique<ValueTunableEntry<double>>(
          this, ntPath, uid, config, "double");
      break;
    case STRING:
    case MEMBER_STRING:
      entry = std::make_unique<ValueTunableEntry<std::string>>(
          this, ntPath, uid, config, "string");
      break;
    case RAW:
    case MEMBER_RAW:
      entry = std::make_unique<ValueTunableEntry<std::vector<uint8_t>>>(
          this, ntPath, uid, config, "raw");
      break;
    case BOOLEAN_ARRAY:
    case MEMBER_BOOLEAN_ARRAY:
      entry = std::make_unique<ValueTunableEntry<std::vector<bool>>>(
          this, ntPath, uid, config, "boolean[]");
      break;
    case INT32_ARRAY:
    case MEMBER_INT32_ARRAY:
      entry = std::make_unique<ValueTunableEntry<std::vector<int32_t>>>(
          this, ntPath, uid, config, "int[]");
      break;
    case INT64_ARRAY:
    case MEMBER_INT64_ARRAY:
      entry = std::make_unique<ValueTunableEntry<std::vector<int64_t>>>(
          this, ntPath, uid, config, "int[]");
      break;
    case FLOAT_ARRAY:
    case MEMBER_FLOAT_ARRAY:
      entry = std::make_unique<ValueTunableEntry<std::vector<float>>>(
          this, ntPath, uid, config, "float[]");
      break;
    case DOUBLE_ARRAY:
    case MEMBER_DOUBLE_ARRAY:
      entry = std::make_unique<ValueTunableEntry<std::vector<double>>>(
          this, ntPath, uid, config, "double[]");
      break;
    case STRING_ARRAY:
    case MEMBER_STRING_ARRAY:
      entry = std::make_unique<ValueTunableEntry<std::vector<std::string>>>(
          this, ntPath, uid, config, "string[]");
      break;
    case STRUCT:
      entry = std::make_unique<StructTunableEntry>(
          this, ntPath, uid, config,
          static_cast<detail::TunableStructBase&>(tunable).GetStructTypeName());
      break;
    case MEMBER_STRUCT:
      entry = std::make_unique<StructTunableEntry>(
          this, ntPath, uid, config,
          static_cast<detail::TunableMemberStructBase&>(tunable)
              .GetStructTypeName());
      break;
    case PROTOBUF:
      entry = std::make_unique<ProtobufTunableEntry>(
          this, ntPath, uid, config,
          static_cast<detail::TunableProtobufBase&>(tunable)
              .GetProtobufTypeString());
      break;
    case MEMBER_PROTOBUF:
      entry = std::make_unique<ProtobufTunableEntry>(
          this, ntPath, uid, config,
          static_cast<detail::TunableMemberProtobufBase&>(tunable)
              .GetProtobufTypeString());
      break;
    case COMPLEX:
    case MEMBER_COMPLEX:
      entry =
          std::make_unique<ComplexTunableEntry>(m_inst, ntPath, uid, tunable);
      break;
    default:
      TunableRegistry::ReportWarning(
          fmt::format("Unsupported tunable type for path {}", path));
      return;
  }

  entry->UpdateNetwork();
  m_entries[path] = std::move(entry);
  m_uids[uid].emplace_back(path);
}

void NetworkTablesTunableBackend::Remove(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    auto uid = it->second->GetUid();
    if (auto uidIt = m_uids.find(uid); uidIt != m_uids.end()) {
      std::erase(uidIt->second, path);
      if (uidIt->second.empty()) {
        m_uids.erase(uidIt);
      }
    }
    m_entries.erase(it);
  }
}

std::vector<TunableBackend::PublishedTunable>
NetworkTablesTunableBackend::RemovePrefix(std::string_view prefix) {
  std::scoped_lock lock{m_mutex};
  std::vector<PublishedTunable> removed;
  for (auto it = m_entries.begin(); it != m_entries.end();) {
    if (!wpi::util::starts_with(it->first, prefix)) {
      ++it;
      continue;
    }
    std::string path{it->first};
    auto uid = it->second->GetUid();
    removed.push_back({path, uid});
    if (auto uidIt = m_uids.find(uid); uidIt != m_uids.end()) {
      std::erase(uidIt->second, path);
      if (uidIt->second.empty()) {
        m_uids.erase(uidIt);
      }
    }
    it = m_entries.erase(it);
  }
  return removed;
}

void NetworkTablesTunableBackend::UnregisterTunable(uint32_t uid) {
  std::scoped_lock lock{m_mutex};
  auto it = m_uids.find(uid);
  if (it != m_uids.end()) {
    for (auto&& path : it->second) {
      m_entries.erase(path);
    }
    m_uids.erase(it);
  }
}

void NetworkTablesTunableBackend::Update() {
  std::scoped_lock lock{m_mutex};
  for (auto&& event : m_poller.ReadQueue()) {
    auto valueData = event.GetValueEventData();
    if (!valueData || !valueData->value) {
      continue;
    }
    auto it = m_subscribers.find(valueData->subentry);
    if (it != m_subscribers.end()) {
      it->second->UpdateTunable(valueData->value);
    }
  }

  for (auto&& [path, entry] : m_entries) {
    entry->UpdateNetwork();
  }
}
