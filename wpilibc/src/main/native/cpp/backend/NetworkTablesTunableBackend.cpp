// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTunableBackend.hpp"

#include <stdint.h>

#include <algorithm>
#include <exception>
#include <format>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/detail/PathUtil.hpp"
#include "wpi/tunables/detail/TunableBase.hpp"
#include "wpi/tunables/detail/TunableDetail.hpp"
#include "wpi/tunables/detail/TunableMember.hpp"
#include "wpi/tunables/detail/TunableTypeValue.hpp"
#include "wpi/util/json.hpp"

using namespace wpi;
using namespace wpi::backend;
using namespace wpi::tunables;

namespace {

std::string_view GetTypeString(std::string_view defaultType,
                               const TunableConfig* config) {
  if (config && config->typeString) {
    return *config->typeString;
  }
  return defaultType;
}

wpi::util::json GetProperties(const TunableConfig* config) {
  if (!config) {
    wpi::util::json properties = wpi::util::json::object();
    properties["mutable"] = true;
    return properties;
  }

  wpi::util::json properties = config->properties;
  if (config->robust) {
    properties["robust"] = true;
  }
  properties["mutable"] = config->isMutable;
  return properties;
}

bool IsAlwaysGet(const TunableConfig* config) {
  return config && config->polling == TunableConfig::Polling::ALWAYS_GET;
}

void NotifyOnTune(uint32_t uid, TunableRegistry::TunableInfo& info) {
  TunableRegistry::ResetChangedAfterUpdate(uid);
  if (auto config = info.config) {
    if (config->onTune) {
      TunableRegistry::RunAfterUpdate([uid] {
        auto info = TunableRegistry::GetTunable(uid);
        if (auto config = info.config) {
          if (auto&& onTune = config->onTune) {
            onTune(*info.tunable, config->parent);
          }
        }
      });
    }
  }
}

void NotifyRemoteSet(TunableRegistry::TunableInfo& info) {
  if (auto config = info.config) {
    if (config->onRemoteSet) {
      config->onRemoteSet(*info.tunable, config->parent);
    }
  }
}

bool ShouldUpdateNetwork(const TunableRegistry::TunableInfo& info) {
  return (info.config &&
          info.config->polling == TunableConfig::Polling::ALWAYS_GET) ||
         info.IsChanged();
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
  enum class UpdateKind { DIRTY, ALWAYS_GET, COMPLEX };

  virtual ~Entry() = default;
  virtual uint32_t GetUid() const = 0;
  virtual void UpdateNetwork(bool force) = 0;
  virtual void RemoveListener() {}
  virtual ValueEntry* AsValueEntry() { return nullptr; }
  void SetInsertEpoch(uint64_t epoch) { m_insertEpoch = epoch; }
  uint64_t GetInsertEpoch() const { return m_insertEpoch; }
  void SetUpdateKind(UpdateKind kind) { m_updateKind = kind; }
  UpdateKind GetUpdateKind() const { return m_updateKind; }
  void SetDirtyQueued(bool queued) { m_dirtyQueued = queued; }
  bool IsDirtyQueued() const { return m_dirtyQueued; }
  void MarkRemoved() { m_removed = true; }
  bool IsRemoved() const { return m_removed; }

 private:
  uint64_t m_insertEpoch = 0;
  UpdateKind m_updateKind = UpdateKind::DIRTY;
  bool m_dirtyQueued = false;
  bool m_removed = false;
};

class NetworkTablesTunableBackend::ValueEntry : public Entry {
 public:
  ValueEntry(NetworkTablesTunableBackend* backend, std::string_view path,
             uint32_t uid, const TunableConfig* config,
             std::string_view typeString)
      : m_backend{backend},
        m_path{path},
        m_uid{uid},
        m_applyInitialValue{config && config->robust && config->isMutable} {
    typeString = GetTypeString(typeString, config);
    if (config && config->robust) {
      m_publisher = backend->m_inst.GetTopic(std::format("{}/value", path))
                        .GenericPublishEx(typeString, GetProperties(config));
      wpi::nt::PubSubOptions subscriberOptions;
      subscriberOptions.excludePublisher = m_publisher.GetHandle();
      m_subscriber = backend->m_inst.GetTopic(std::format("{}/tune", path))
                         .GenericSubscribe(typeString, subscriberOptions);
    } else {
      auto topic = backend->m_inst.GetTopic(path);
      m_publisher = topic.GenericPublishEx(typeString, GetProperties(config));
      wpi::nt::PubSubOptions subscriberOptions;
      subscriberOptions.excludePublisher = m_publisher.GetHandle();
      m_subscriber = topic.GenericSubscribe(typeString, subscriberOptions);
    }
    if (!config || config->isMutable) {
      m_listener = backend->m_poller.AddListener(
          m_subscriber, wpi::nt::EventFlags::VALUE_ALL);
    }
  }

  ~ValueEntry() override {
    if (m_listener != 0) {
      m_backend->m_poller.RemoveListener(m_listener);
    }
  }

  uint32_t GetUid() const override { return m_uid; }

  void RemoveListener() override {
    if (m_listener != 0) {
      m_backend->m_poller.RemoveListener(m_listener);
      m_listener = 0;
    }
  }

  ValueEntry* AsValueEntry() override { return this; }

  NT_Handle GetSubscriberHandle() const { return m_subscriber.GetHandle(); }

  bool UpdateTunable(const wpi::nt::Value& value) {
    auto info = TunableRegistry::GetTunable(m_uid);
    if (!info || !TypeMatches(value, info.type)) {
      return false;
    }
    if (!DoUpdateTunable(info, value)) {
      return false;
    }
    NotifyRemoteSet(info);
    m_forceUpdate = true;
    return true;
  }

  bool UpdateInitialTunable() {
    if (!m_applyInitialValue) {
      return false;
    }
    return UpdateTunable(m_subscriber.Get());
  }

 protected:
  bool ShouldPublishNetworkValue(const TunableRegistry::TunableInfo& info,
                                 bool force, bool& forcePublish) {
    forcePublish = force || m_forceUpdate;
    return forcePublish || ShouldUpdateNetwork(info);
  }

  void FinishNetworkUpdate() { m_forceUpdate = false; }

  virtual bool DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                               const wpi::nt::Value& value) = 0;

  void ReportWarning(std::string_view msg) {
    TunableRegistry::ReportWarning(
        std::format("NetworkTables tunable '{}': {}", m_path, msg));
  }

  void ReportWarning(std::string_view msg, const std::exception& e) {
    ReportWarning(std::format("{}: {}", msg, e.what()));
  }

  wpi::nt::GenericPublisher m_publisher;

 private:
  NetworkTablesTunableBackend* m_backend;
  std::string m_path;
  uint32_t m_uid;
  wpi::nt::GenericSubscriber m_subscriber;
  NT_Listener m_listener = 0;
  bool m_applyInitialValue;
  bool m_forceUpdate = false;
};

template <typename T>
class ValueTunableEntry final : public NetworkTablesTunableBackend::ValueEntry {
 public:
  using NetworkTablesTunableBackend::ValueEntry::ValueEntry;

  void UpdateNetwork(bool force) override {
    if (auto info = TunableRegistry::GetTunable(GetUid())) {
      bool forcePublish = false;
      if (!ShouldPublishNetworkValue(info, force, forcePublish)) {
        return;
      }
      auto value = GetValue<T>(info);
      if (forcePublish || !m_lastValue || value != *m_lastValue) {
        SetNetworkValue(value);
        m_lastValue = std::move(value);
      }
      TunableRegistry::ResetChangedAfterUpdate(GetUid());
      FinishNetworkUpdate();
    }
  }

 private:
  bool DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                       const wpi::nt::Value& value) override {
    SetValue<T>(info, GetNtValue(value));
    return true;
  }

  void SetNetworkValue(const T& value);
  T GetNtValue(const wpi::nt::Value& value);
  std::optional<T> m_lastValue;
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
void ValueTunableEntry<std::string>::SetNetworkValue(const std::string& value) {
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
std::vector<std::string>
ValueTunableEntry<std::vector<std::string>>::GetNtValue(
    const wpi::nt::Value& value) {
  auto arr = value.GetStringArray();
  return {arr.begin(), arr.end()};
}

class StructTunableEntry final
    : public NetworkTablesTunableBackend::ValueEntry {
 public:
  using NetworkTablesTunableBackend::ValueEntry::ValueEntry;

  void UpdateNetwork(bool force) override {
    auto info = TunableRegistry::GetTunable(GetUid());
    if (!info) {
      return;
    }
    bool forcePublish = false;
    if (!ShouldPublishNetworkValue(info, force, forcePublish)) {
      return;
    }
    if (info.type == detail::TunableTypeValue::STRUCT) {
      auto tunable = static_cast<detail::TunableStructBase*>(info.tunable);
      PublishSchemas(*tunable);
      m_data.resize(tunable->GetStructSize());
      tunable->PackStruct(m_data);
    } else {
      auto tunable =
          static_cast<detail::TunableMemberStructBase*>(info.tunable);
      PublishSchemas(*tunable);
      m_data.resize(tunable->GetStructSize(info.config->parent));
      tunable->PackStruct(info.config->parent, m_data);
    }
    if (forcePublish || m_data != m_lastData) {
      m_publisher.SetRaw(m_data);
      m_lastData = m_data;
    }
    TunableRegistry::ResetChangedAfterUpdate(GetUid());
    FinishNetworkUpdate();
  }

 private:
  bool DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                       const wpi::nt::Value& value) override {
    try {
      auto data = value.GetRaw();
      if (info.type == detail::TunableTypeValue::STRUCT) {
        return static_cast<detail::TunableStructBase*>(info.tunable)
            ->UnpackStruct(data);
      } else {
        return static_cast<detail::TunableMemberStructBase*>(info.tunable)
            ->UnpackStruct(info.config->parent, data);
      }
    } catch (const std::exception& e) {
      ReportWarning("rejected struct tune payload", e);
      return false;
    } catch (...) {
      ReportWarning("rejected struct tune payload");
      return false;
    }
  }

  void PublishSchemas(detail::TunableStructBase& tunable) {
    if (m_schemaPublished) {
      return;
    }
    tunable.ForEachStructSchema(
        [this](std::string_view typeName, std::string_view schema) {
          m_publisher.GetTopic().GetInstance().AddSchema(
              typeName, "structschema", schema);
        });
    m_schemaPublished = true;
  }

  void PublishSchemas(detail::TunableMemberStructBase& tunable) {
    if (m_schemaPublished) {
      return;
    }
    tunable.ForEachStructSchema(
        [this](std::string_view typeName, std::string_view schema) {
          m_publisher.GetTopic().GetInstance().AddSchema(
              typeName, "structschema", schema);
        });
    m_schemaPublished = true;
  }

  bool m_schemaPublished = false;
  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_lastData;
};

class ProtobufTunableEntry final
    : public NetworkTablesTunableBackend::ValueEntry {
 public:
  using NetworkTablesTunableBackend::ValueEntry::ValueEntry;

  void UpdateNetwork(bool force) override {
    auto info = TunableRegistry::GetTunable(GetUid());
    if (!info) {
      return;
    }
    bool forcePublish = false;
    if (!ShouldPublishNetworkValue(info, force, forcePublish)) {
      return;
    }
    m_data.clear();
    bool packed = false;
    if (info.type == detail::TunableTypeValue::PROTOBUF) {
      auto tunable = static_cast<detail::TunableProtobufBase*>(info.tunable);
      PublishSchemas(*tunable);
      packed = tunable->PackProtobuf(m_data);
    } else {
      auto tunable =
          static_cast<detail::TunableMemberProtobufBase*>(info.tunable);
      PublishSchemas(*tunable);
      packed = tunable->PackProtobuf(info.config->parent, m_data);
    }
    if (packed && (forcePublish || m_data != m_lastData)) {
      m_publisher.SetRaw(m_data);
      m_lastData = m_data;
    }
    TunableRegistry::ResetChangedAfterUpdate(GetUid());
    FinishNetworkUpdate();
  }

 private:
  bool DoUpdateTunable(const TunableRegistry::TunableInfo& info,
                       const wpi::nt::Value& value) override {
    bool updated = false;
    try {
      if (info.type == detail::TunableTypeValue::PROTOBUF) {
        updated = static_cast<detail::TunableProtobufBase*>(info.tunable)
                      ->UnpackProtobuf(value.GetRaw());
      } else {
        updated = static_cast<detail::TunableMemberProtobufBase*>(info.tunable)
                      ->UnpackProtobuf(info.config->parent, value.GetRaw());
      }
    } catch (const std::exception& e) {
      ReportWarning("rejected protobuf tune payload", e);
      return false;
    } catch (...) {
      ReportWarning("rejected protobuf tune payload");
      return false;
    }
    if (!updated) {
      ReportWarning("rejected protobuf tune payload");
    }
    return updated;
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
  std::vector<uint8_t> m_data;
  std::vector<uint8_t> m_lastData;
};

class ComplexTunableEntry final : public NetworkTablesTunableBackend::Entry {
 public:
  ComplexTunableEntry(wpi::nt::NetworkTableInstance inst, std::string_view path,
                      uint32_t uid, detail::TunableBase& tunable)
      : m_uid{uid},
        m_typePublisher{inst.GetStringTopic(std::format("{}/.type", path))
                            .PublishEx("string", GetProperties(nullptr))} {
    auto& complex = static_cast<ComplexTunable&>(tunable);
    m_typePublisher.Set(complex.GetTunableType());
  }

  uint32_t GetUid() const override { return m_uid; }

  void UpdateNetwork(bool) override {
    TunableRegistry::ResetChangedAfterUpdate(m_uid);
  }

 private:
  uint32_t m_uid;
  wpi::nt::StringPublisher m_typePublisher;
};

NetworkTablesTunableBackend::NetworkTablesTunableBackend(
    wpi::nt::NetworkTableInstance inst, std::string_view prefix)
    : m_inst{inst}, m_prefix{prefix}, m_poller{inst} {}

void NetworkTablesTunableBackend::TrackEntry(
    const std::shared_ptr<Entry>& entry, const TunableConfig* config,
    detail::TunableTypeValue type) {
  using enum detail::TunableTypeValue;
  if (type == COMPLEX || type == MEMBER_COMPLEX) {
    entry->SetUpdateKind(Entry::UpdateKind::COMPLEX);
    m_complexEntries.emplace_back(entry);
  } else if (IsAlwaysGet(config)) {
    entry->SetUpdateKind(Entry::UpdateKind::ALWAYS_GET);
    m_alwaysGetEntries.emplace_back(entry);
  } else {
    entry->SetUpdateKind(Entry::UpdateKind::DIRTY);
  }
}

void NetworkTablesTunableBackend::UntrackEntry(
    const std::shared_ptr<Entry>& entry) {
  if (m_updateDepth > 0) {
    m_deferredUntracks.emplace_back(entry);
    return;
  }
  UntrackEntryNow(entry);
}

void NetworkTablesTunableBackend::UntrackEntryNow(
    const std::shared_ptr<Entry>& entry) {
  entry->SetDirtyQueued(false);
  std::erase(m_dirtyEntries, entry);
  std::erase(m_alwaysGetEntries, entry);
  std::erase(m_complexEntries, entry);
}

void NetworkTablesTunableBackend::EnqueueDirtyEntry(
    const std::shared_ptr<Entry>& entry) {
  if (entry->IsRemoved() ||
      entry->GetUpdateKind() != Entry::UpdateKind::DIRTY ||
      entry->IsDirtyQueued()) {
    return;
  }
  entry->SetDirtyQueued(true);
  m_dirtyEntries.emplace_back(entry);
}

void NetworkTablesTunableBackend::RetireEntry(
    const std::shared_ptr<Entry>& entry) {
  if (entry->IsRemoved()) {
    return;
  }
  UntrackEntry(entry);
  entry->MarkRemoved();
  entry->RemoveListener();
  if (auto valueEntry = entry->AsValueEntry()) {
    m_subscribers.erase(valueEntry->GetSubscriberHandle());
  }
}

void NetworkTablesTunableBackend::RemovePublishedEntry(
    std::string path, const std::shared_ptr<Entry>& entry) {
  auto uid = entry->GetUid();
  RetireEntry(entry);
  if (auto uidIt = m_uids.find(uid); uidIt != m_uids.end()) {
    std::erase(uidIt->second, path);
    if (uidIt->second.empty()) {
      m_uids.erase(uidIt);
    }
  }
  EraseEntry(std::move(path), entry);
}

void NetworkTablesTunableBackend::EraseEntry(
    std::string path, const std::shared_ptr<Entry>& entry) {
  if (m_updateDepth > 0) {
    m_deferredErases.emplace_back(std::move(path), entry);
    return;
  }
  auto it = m_entries.find(path);
  if (it != m_entries.end() && it->second == entry) {
    m_entries.erase(it);
  }
}

void NetworkTablesTunableBackend::ApplyDeferredErases() {
  if (m_clearTrackedEntries) {
    ClearTrackedEntries();
    m_deferredErases.clear();
    m_deferredUntracks.clear();
    m_clearTrackedEntries = false;
    return;
  }

  for (auto&& pending : m_deferredErases) {
    auto it = m_entries.find(pending.path);
    if (it != m_entries.end() && it->second == pending.entry) {
      m_entries.erase(it);
    }
  }
  m_deferredErases.clear();

  for (auto&& entry : m_deferredUntracks) {
    UntrackEntryNow(entry);
  }
  m_deferredUntracks.clear();
}

void NetworkTablesTunableBackend::ClearTrackedEntries() {
  for (auto&& entry : m_dirtyEntries) {
    entry->SetDirtyQueued(false);
  }
  m_dirtyEntries.clear();
  m_alwaysGetEntries.clear();
  m_complexEntries.clear();
}

NetworkTablesTunableBackend::~NetworkTablesTunableBackend() {
  Retire();
}

void NetworkTablesTunableBackend::Retire() {
  // Entries use backend-owned listener state, so retire them before releasing
  // the poller.
  std::scoped_lock lock{m_mutex};
  if (m_retired) {
    return;
  }
  m_retired = true;
  for (auto&& [path, entry] : m_entries) {
    RetireEntry(entry);
  }
  m_entries.clear();
  m_uids.clear();
  m_subscribers.clear();
  m_deferredErases.clear();
  if (m_updateDepth > 0) {
    m_deferredUntracks.clear();
    m_clearTrackedEntries = true;
  } else {
    ClearTrackedEntries();
  }
  m_poller = {};
}

bool NetworkTablesTunableBackend::Publish(std::string_view path, uint32_t uid,
                                          detail::TunableBase& tunable,
                                          const TunableConfig* config,
                                          detail::TunableTypeValue type) {
  std::shared_ptr<Entry> entry;
  {
    std::scoped_lock lock{m_mutex};
    if (m_retired) {
      return false;
    }
    if (auto it = m_entries.find(path);
        it != m_entries.end() && !it->second->IsRemoved()) {
      TunableRegistry::ReportWarning(std::format(
          "NetworkTables tunable '{}{}' already exists", m_prefix, path));
      return false;
    }

    std::string ntPath = std::format("{}{}", m_prefix, path);
    using enum detail::TunableTypeValue;
    switch (type) {
      case BOOLEAN:
      case MEMBER_BOOLEAN:
        entry = std::make_shared<ValueTunableEntry<bool>>(this, ntPath, uid,
                                                          config, "boolean");
        break;
      case INT32:
      case MEMBER_INT32:
        entry = std::make_shared<ValueTunableEntry<int32_t>>(this, ntPath, uid,
                                                             config, "int");
        break;
      case INT64:
      case MEMBER_INT64:
        entry = std::make_shared<ValueTunableEntry<int64_t>>(this, ntPath, uid,
                                                             config, "int");
        break;
      case FLOAT:
      case MEMBER_FLOAT:
        entry = std::make_shared<ValueTunableEntry<float>>(this, ntPath, uid,
                                                           config, "float");
        break;
      case DOUBLE:
      case MEMBER_DOUBLE:
        entry = std::make_shared<ValueTunableEntry<double>>(this, ntPath, uid,
                                                            config, "double");
        break;
      case STRING:
      case MEMBER_STRING:
        entry = std::make_shared<ValueTunableEntry<std::string>>(
            this, ntPath, uid, config, "string");
        break;
      case RAW:
      case MEMBER_RAW:
        entry = std::make_shared<ValueTunableEntry<std::vector<uint8_t>>>(
            this, ntPath, uid, config, "raw");
        break;
      case BOOLEAN_ARRAY:
      case MEMBER_BOOLEAN_ARRAY:
        entry = std::make_shared<ValueTunableEntry<std::vector<bool>>>(
            this, ntPath, uid, config, "boolean[]");
        break;
      case INT32_ARRAY:
      case MEMBER_INT32_ARRAY:
        entry = std::make_shared<ValueTunableEntry<std::vector<int32_t>>>(
            this, ntPath, uid, config, "int[]");
        break;
      case INT64_ARRAY:
      case MEMBER_INT64_ARRAY:
        entry = std::make_shared<ValueTunableEntry<std::vector<int64_t>>>(
            this, ntPath, uid, config, "int[]");
        break;
      case FLOAT_ARRAY:
      case MEMBER_FLOAT_ARRAY:
        entry = std::make_shared<ValueTunableEntry<std::vector<float>>>(
            this, ntPath, uid, config, "float[]");
        break;
      case DOUBLE_ARRAY:
      case MEMBER_DOUBLE_ARRAY:
        entry = std::make_shared<ValueTunableEntry<std::vector<double>>>(
            this, ntPath, uid, config, "double[]");
        break;
      case STRING_ARRAY:
      case MEMBER_STRING_ARRAY:
        entry = std::make_shared<ValueTunableEntry<std::vector<std::string>>>(
            this, ntPath, uid, config, "string[]");
        break;
      case STRUCT:
        entry = std::make_shared<StructTunableEntry>(
            this, ntPath, uid, config,
            std::format("struct:{}",
                        static_cast<detail::TunableStructBase&>(tunable)
                            .GetStructTypeName()));
        break;
      case MEMBER_STRUCT:
        entry = std::make_shared<StructTunableEntry>(
            this, ntPath, uid, config,
            std::format("struct:{}",
                        static_cast<detail::TunableMemberStructBase&>(tunable)
                            .GetStructTypeName()));
        break;
      case PROTOBUF:
        entry = std::make_shared<ProtobufTunableEntry>(
            this, ntPath, uid, config,
            static_cast<detail::TunableProtobufBase&>(tunable)
                .GetProtobufTypeString());
        break;
      case MEMBER_PROTOBUF:
        entry = std::make_shared<ProtobufTunableEntry>(
            this, ntPath, uid, config,
            static_cast<detail::TunableMemberProtobufBase&>(tunable)
                .GetProtobufTypeString());
        break;
      case COMPLEX:
      case MEMBER_COMPLEX:
        entry =
            std::make_shared<ComplexTunableEntry>(m_inst, ntPath, uid, tunable);
        break;
      default:
        TunableRegistry::ReportWarning(
            std::format("Unsupported tunable type for path {}", path));
        return false;
    }

    entry->SetInsertEpoch(++m_nextInsertEpoch);
    m_entries[path] = entry;
    if (auto valueEntry = entry->AsValueEntry()) {
      m_subscribers[valueEntry->GetSubscriberHandle()] =
          std::static_pointer_cast<ValueEntry>(entry);
    }
    m_uids[uid].emplace_back(path);
    TrackEntry(entry, config, type);
  }

  bool tunedInitialValue = false;
  try {
    if (auto valueEntry = entry->AsValueEntry()) {
      tunedInitialValue = valueEntry->UpdateInitialTunable();
    }
    entry->UpdateNetwork(true);
  } catch (...) {
    {
      std::scoped_lock lock{m_mutex};
      if (!m_retired) {
        RemovePublishedEntry(std::string{path}, entry);
      }
    }
    try {
      throw;
    } catch (const std::exception& e) {
      TunableRegistry::ReportWarning(std::format(
          "NetworkTables tunable '{}{}' failed during initial publish: {}",
          m_prefix, path, e.what()));
    } catch (...) {
      TunableRegistry::ReportWarning(std::format(
          "NetworkTables tunable '{}{}' failed during initial publish",
          m_prefix, path));
    }
    return false;
  }
  if (tunedInitialValue) {
    auto info = TunableRegistry::GetTunable(entry->GetUid());
    if (info) {
      NotifyOnTune(entry->GetUid(), info);
    }
  }
  return true;
}

void NetworkTablesTunableBackend::MarkDirty(uint32_t uid) {
  std::scoped_lock lock{m_mutex};
  if (m_retired) {
    return;
  }
  auto uidIt = m_uids.find(uid);
  if (uidIt == m_uids.end()) {
    return;
  }
  for (auto&& path : uidIt->second) {
    if (auto entryIt = m_entries.find(path);
        entryIt != m_entries.end() && !entryIt->second->IsRemoved()) {
      EnqueueDirtyEntry(entryIt->second);
    }
  }
}

void NetworkTablesTunableBackend::Remove(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  if (m_retired) {
    return;
  }
  auto it = m_entries.find(path);
  if (it != m_entries.end() && !it->second->IsRemoved()) {
    RemovePublishedEntry(std::string{it->first}, it->second);
  }
}

std::vector<TunableBackend::PublishedTunable>
NetworkTablesTunableBackend::RemovePrefix(std::string_view prefix) {
  std::string prefixBuf;
  prefix = wpi::tunables::detail::NormalizePrefix(prefix, prefixBuf);
  std::scoped_lock lock{m_mutex};
  std::vector<PublishedTunable> removed;
  if (m_retired) {
    return removed;
  }
  for (auto it = m_entries.begin(); it != m_entries.end();) {
    if (it->second->IsRemoved() ||
        !wpi::tunables::detail::IsPathOrDescendant(it->first, prefix)) {
      ++it;
      continue;
    }
    std::string path{it->first};
    auto entry = it->second;
    ++it;
    removed.push_back({path, entry->GetUid()});
    RemovePublishedEntry(std::move(path), entry);
  }
  return removed;
}

void NetworkTablesTunableBackend::UnregisterTunable(uint32_t uid) {
  std::scoped_lock lock{m_mutex};
  if (m_retired) {
    return;
  }
  auto it = m_uids.find(uid);
  if (it != m_uids.end()) {
    for (auto&& path : it->second) {
      if (auto entryIt = m_entries.find(path);
          entryIt != m_entries.end() && !entryIt->second->IsRemoved()) {
        auto entry = entryIt->second;
        RetireEntry(entry);
        EraseEntry(path, entry);
      }
    }
    m_uids.erase(it);
  }
}

void NetworkTablesTunableBackend::Update() {
  std::vector<uint32_t> onTuneUids;
  auto processEvents = [&](std::vector<wpi::nt::Event>& tuneEvents) {
    for (auto&& tuneEvent : tuneEvents) {
      auto valueData = tuneEvent.GetValueEventData();
      if (!valueData || !valueData->value) {
        continue;
      }
      std::shared_ptr<ValueEntry> entry;
      {
        std::scoped_lock lock{m_mutex};
        auto it = m_subscribers.find(valueData->subentry);
        if (it != m_subscribers.end()) {
          entry = it->second;
        }
      }
      if (entry && entry->UpdateTunable(valueData->value)) {
        onTuneUids.emplace_back(entry->GetUid());
      }
    }
  };

  std::vector<wpi::nt::Event> events;
  {
    std::scoped_lock lock{m_mutex};
    if (m_retired) {
      return;
    }
    events = m_poller.ReadQueue();
  }
  processEvents(events);

  std::unique_lock lock{m_mutex};
  ++m_updateDepth;
  auto finishUpdate = [&] {
    if (!lock.owns_lock()) {
      lock.lock();
    }
    --m_updateDepth;
    if (m_updateDepth == 0) {
      ApplyDeferredErases();
    }
  };
  try {
    uint64_t updateEpoch = m_nextInsertEpoch;
    auto updateEntry = [&](const std::shared_ptr<Entry>& entry,
                           bool requeueIfInsertedDuringUpdate) {
      if (entry->IsRemoved() || entry->GetInsertEpoch() > updateEpoch) {
        if (!entry->IsRemoved() && requeueIfInsertedDuringUpdate) {
          EnqueueDirtyEntry(entry);
        }
        return;
      }

      lock.unlock();
      entry->UpdateNetwork(false);
      lock.lock();
    };

    auto updateEntries = [&](auto& entries,
                             bool requeueIfInsertedDuringUpdate) {
      size_t size = entries.size();
      for (size_t i = 0; i < size; ++i) {
        auto entry = entries[i];
        updateEntry(entry, requeueIfInsertedDuringUpdate);
      }
    };

    updateEntries(m_complexEntries, false);
    updateEntries(m_alwaysGetEntries, false);

    std::vector<std::shared_ptr<Entry>> dirtyEntries;
    dirtyEntries.swap(m_dirtyEntries);
    for (auto&& entry : dirtyEntries) {
      entry->SetDirtyQueued(false);
    }
    for (auto&& entry : dirtyEntries) {
      updateEntry(entry, true);
    }
  } catch (...) {
    if (!lock.owns_lock()) {
      lock.lock();
    }
    finishUpdate();
    lock.unlock();
    try {
      throw;
    } catch (const std::exception& e) {
      TunableRegistry::ReportWarning(std::format(
          "NetworkTables tunable backend update failed: {}", e.what()));
    } catch (...) {
      TunableRegistry::ReportWarning(
          "NetworkTables tunable backend update failed");
    }
    return;
  }
  finishUpdate();
  lock.unlock();

  // onTune callbacks can publish or remove tunables, which re-enters this
  // backend. Run them after releasing m_mutex so those mutations are safe.
  for (auto uid : onTuneUids) {
    auto info = TunableRegistry::GetTunable(uid);
    if (info) {
      NotifyOnTune(uid, info);
    }
  }
}
