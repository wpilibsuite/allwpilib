// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/Logger.h>
#include <wpi/SmallVector.h>
#include <wpi/json.h>
#include <wpi/mutex.h>

#include "local/LocalStorageImpl.h"
#include "net/MessageHandler.h"
#include "net/NetworkInterface.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt {

class IListenerStorage;

class LocalStorage final : public net::ILocalStorage {
 public:
  LocalStorage(int inst, IListenerStorage& listenerStorage, wpi::Logger& logger)
      : m_impl{inst, listenerStorage, logger} {}
  LocalStorage(const LocalStorage&) = delete;
  LocalStorage& operator=(const LocalStorage&) = delete;

  // network interface functions
  int ServerAnnounce(std::string_view name, int id, std::string_view typeStr,
                     const wpi::json& properties,
                     std::optional<int> pubuid) final {
    std::scoped_lock lock{m_mutex};
    auto topic = m_impl.GetOrCreateTopic(name);
    m_impl.NetworkAnnounce(topic, typeStr, properties, pubuid);
    return Handle{topic->handle}.GetIndex();
  }

  void ServerUnannounce(std::string_view name, int id) final {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveNetworkPublisher(m_impl.GetOrCreateTopic(name));
  }

  void ServerPropertiesUpdate(std::string_view name, const wpi::json& update,
                              bool ack) final {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByName(name)) {
      m_impl.NetworkPropertiesUpdate(topic, update, ack);
    }
  }

  void ServerSetValue(int topicId, const Value& value) final {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicById(topicId)) {
      m_impl.ServerSetValue(topic, value);
    }
  }

  void StartNetwork(net::ClientMessageHandler* network) final {
    std::scoped_lock lock{m_mutex};
    m_impl.StartNetwork(network);
  }

  void ClearNetwork() final {
    std::scoped_lock lock{m_mutex};
    m_impl.ClearNetwork();
  }

  // User functions.  These are the actual implementations of the corresponding
  // user API functions in ntcore_cpp.

  std::vector<NT_Topic> GetTopics(std::string_view prefix, unsigned int types);
  std::vector<NT_Topic> GetTopics(std::string_view prefix,
                                  std::span<const std::string_view> types);

  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix,
                                      unsigned int types);
  std::vector<TopicInfo> GetTopicInfo(std::string_view prefix,
                                      std::span<const std::string_view> types);

  NT_Topic GetTopic(std::string_view name) {
    if (name.empty()) {
      return {};
    }
    std::scoped_lock lock{m_mutex};
    return m_impl.GetOrCreateTopic(name)->handle;
  }

  std::string GetTopicName(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return topic->name;
    } else {
      return {};
    }
  }

  NT_Type GetTopicType(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return topic->type;
    } else {
      return {};
    }
  }

  std::string GetTopicTypeString(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return topic->typeStr;
    } else {
      return {};
    }
  }

  void SetTopicPersistent(NT_Topic topicHandle, bool value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      m_impl.SetPersistent(topic, value);
    }
  }

  bool GetTopicPersistent(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return (topic->GetFlags() & NT_PERSISTENT) != 0;
    } else {
      return false;
    }
  }

  void SetTopicRetained(NT_Topic topicHandle, bool value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      m_impl.SetRetained(topic, value);
    }
  }

  bool GetTopicRetained(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return (topic->GetFlags() & NT_RETAINED) != 0;
    } else {
      return false;
    }
  }

  void SetTopicCached(NT_Topic topicHandle, bool value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      m_impl.SetCached(topic, value);
    }
  }

  bool GetTopicCached(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return (topic->GetFlags() & NT_UNCACHED) == 0;
    } else {
      return false;
    }
  }

  bool GetTopicExists(NT_Handle handle) {
    std::scoped_lock lock{m_mutex};
    local::LocalTopic* topic = m_impl.GetTopic(handle);
    return topic && topic->Exists();
  }

  wpi::json GetTopicProperty(NT_Topic topicHandle, std::string_view name) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return topic->properties.value(name, wpi::json{});
    } else {
      return {};
    }
  }

  void SetTopicProperty(NT_Topic topicHandle, std::string_view name,
                        const wpi::json& value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      m_impl.SetProperty(topic, name, value);
    }
  }

  void DeleteTopicProperty(NT_Topic topicHandle, std::string_view name) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      m_impl.DeleteProperty(topic, name);
    }
  }

  wpi::json GetTopicProperties(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return topic->properties;
    } else {
      return wpi::json::object();
    }
  }

  bool SetTopicProperties(NT_Topic topicHandle, const wpi::json& update) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return m_impl.SetProperties(topic, update, true);
    } else {
      return {};
    }
  }

  TopicInfo GetTopicInfo(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      return topic->GetTopicInfo();
    } else {
      return {};
    }
  }

  NT_Subscriber Subscribe(NT_Topic topicHandle, NT_Type type,
                          std::string_view typeStr,
                          const PubSubOptions& options) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      if (auto subscriber = m_impl.Subscribe(topic, type, typeStr, options)) {
        return subscriber->handle;
      }
    }
    return {};
  }

  void Unsubscribe(NT_Subscriber subHandle) {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveSubEntry(subHandle);
  }

  NT_MultiSubscriber SubscribeMultiple(
      std::span<const std::string_view> prefixes,
      const PubSubOptions& options) {
    std::scoped_lock lock{m_mutex};
    if (auto sub = m_impl.AddMultiSubscriber(prefixes, options)) {
      return sub->handle;
    } else {
      return {};
    }
  }

  void UnsubscribeMultiple(NT_MultiSubscriber subHandle) {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveMultiSubscriber(subHandle);
  }

  NT_Publisher Publish(NT_Topic topicHandle, NT_Type type,
                       std::string_view typeStr, const wpi::json& properties,
                       const PubSubOptions& options) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      if (auto publisher =
              m_impl.Publish(topic, type, typeStr, properties, options)) {
        return publisher->handle;
      }
    } else {
      WPI_ERROR(m_impl.GetLogger(),
                "trying to publish invalid topic handle ({})", topicHandle);
    }
    return {};
  }

  void Unpublish(NT_Handle pubentryHandle) {
    std::scoped_lock lock{m_mutex};
    m_impl.Unpublish(pubentryHandle);
  }

  NT_Entry GetEntry(NT_Topic topicHandle, NT_Type type,
                    std::string_view typeStr, const PubSubOptions& options) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopicByHandle(topicHandle)) {
      if (auto entry = m_impl.GetEntry(topic, type, typeStr, options)) {
        return entry->handle;
      }
    }
    return {};
  }

  void ReleaseEntry(NT_Entry entryHandle) {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveSubEntry(entryHandle);
  }

  void Release(NT_Handle pubsubentry);

  NT_Topic GetTopicFromHandle(NT_Handle pubsubentryHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.GetTopic(pubsubentryHandle)) {
      return topic->handle;
    } else {
      return {};
    }
  }

  bool SetEntryValue(NT_Handle pubentryHandle, const Value& value) {
    std::scoped_lock lock{m_mutex};
    return m_impl.SetEntryValue(pubentryHandle, value);
  }

  bool SetDefaultEntryValue(NT_Handle pubsubentryHandle, const Value& value) {
    std::scoped_lock lock{m_mutex};
    return m_impl.SetDefaultEntryValue(pubsubentryHandle, value);
  }

  template <ValidType T>
  Timestamped<typename TypeInfo<T>::Value> GetAtomic(
      NT_Handle subentry, typename TypeInfo<T>::View defaultValue) {
    std::scoped_lock lock{m_mutex};
    Value* value = m_impl.GetSubEntryValue(subentry);
    if (value && (IsNumericConvertibleTo<T>(*value) || IsType<T>(*value))) {
      return GetTimestamped<T, true>(*value);
    } else {
      return {0, 0, CopyValue<T>(defaultValue)};
    }
  }

  template <SmallArrayType T>
  Timestamped<typename TypeInfo<T>::SmallRet> GetAtomic(
      NT_Handle subentry,
      wpi::SmallVectorImpl<typename TypeInfo<T>::SmallElem>& buf,
      typename TypeInfo<T>::View defaultValue) {
    std::scoped_lock lock{m_mutex};
    Value* value = m_impl.GetSubEntryValue(subentry);
    if (value && (IsNumericConvertibleTo<T>(*value) || IsType<T>(*value))) {
      return GetTimestamped<T, true>(*value, buf);
    } else {
      return {0, 0, CopyValue<T>(defaultValue, buf)};
    }
  }

  std::vector<Value> ReadQueueValue(NT_Handle subentry, unsigned int types) {
    std::scoped_lock lock{m_mutex};
    auto subscriber = m_impl.GetSubEntry(subentry);
    if (!subscriber) {
      return {};
    }
    return subscriber->pollStorage.ReadValue(types);
  }

  template <ValidType T>
  std::vector<Timestamped<typename TypeInfo<T>::Value>> ReadQueue(
      NT_Handle subentry) {
    std::scoped_lock lock{m_mutex};
    auto subscriber = m_impl.GetSubEntry(subentry);
    if (!subscriber) {
      return {};
    }
    return subscriber->pollStorage.Read<T>();
  }

  //
  // Backwards compatible user functions
  //

  Value GetEntryValue(NT_Handle subentry);

  void SetEntryFlags(NT_Entry entryHandle, unsigned int flags) {
    std::scoped_lock lock{m_mutex};
    if (auto entry = m_impl.GetEntryByHandle(entryHandle)) {
      m_impl.SetFlags(entry->subscriber->topic, flags);
    }
  }

  unsigned int GetEntryFlags(NT_Entry entryHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto entry = m_impl.GetEntryByHandle(entryHandle)) {
      return entry->subscriber->topic->GetFlags();
    } else {
      return 0;
    }
  }

  // Index-only
  NT_Entry GetEntry(std::string_view name) {
    std::scoped_lock lock{m_mutex};
    if (auto entry = m_impl.GetEntry(name)) {
      return entry->handle;
    } else {
      return {};
    }
  }

  std::string GetEntryName(NT_Entry subentryHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto subscriber = m_impl.GetSubEntry(subentryHandle)) {
      return subscriber->topic->name;
    } else {
      return {};
    }
  }

  NT_Type GetEntryType(NT_Entry subentryHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto subscriber = m_impl.GetSubEntry(subentryHandle)) {
      return subscriber->topic->type;
    } else {
      return {};
    }
  }

  int64_t GetEntryLastChange(NT_Entry subentryHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto subscriber = m_impl.GetSubEntry(subentryHandle)) {
      return subscriber->topic->lastValue.time();
    } else {
      return 0;
    }
  }

  //
  // Listener functions
  //

  void AddListener(NT_Listener listenerHandle,
                   std::span<const std::string_view> prefixes,
                   unsigned int mask) {
    mask &= (NT_EVENT_TOPIC | NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE);
    std::scoped_lock lock{m_mutex};
    // subscribe to make sure topic updates are received
    if (auto sub = m_impl.AddMultiSubscriber(
            prefixes, {.topicsOnly = (mask & NT_EVENT_VALUE_ALL) == 0})) {
      m_impl.AddListenerImpl(listenerHandle, sub, mask, true);
    }
  }

  void AddListener(NT_Listener listener, NT_Handle handle, unsigned int mask);

  void RemoveListener(NT_Listener listener, unsigned int mask) {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveListener(listener, mask);
  }

  //
  // Data log functions
  //
  NT_DataLogger StartDataLog(wpi::log::DataLog& log, std::string_view prefix,
                             std::string_view logPrefix) {
    std::scoped_lock lock{m_mutex};
    if (auto dl = m_impl.StartDataLog(log, prefix, logPrefix)) {
      return dl->handle;
    } else {
      return {};
    }
  }

  void StopDataLog(NT_DataLogger logger) {
    std::scoped_lock lock{m_mutex};
    m_impl.StopDataLog(logger);
  }

  //
  // Schema functions
  //
  bool HasSchema(std::string_view name) {
    std::scoped_lock lock{m_mutex};
    return m_impl.HasSchema(name);
  }

  void AddSchema(std::string_view name, std::string_view type,
                 std::span<const uint8_t> schema) {
    std::scoped_lock lock{m_mutex};
    m_impl.AddSchema(name, type, schema);
  }

  void Reset() {
    std::scoped_lock lock{m_mutex};
    m_impl.Reset();
  }

 private:
  wpi::mutex m_mutex;
  local::StorageImpl m_impl;
};

}  // namespace nt
