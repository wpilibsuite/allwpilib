// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <memory>
#include <string_view>

#include <wpi/DenseMap.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/Synchronization.h>
#include <wpi/json.h>

#include "HandleMap.h"
#include "local/LocalDataLogger.h"
#include "local/LocalEntry.h"
#include "local/LocalListener.h"
#include "local/LocalMultiSubscriber.h"
#include "local/LocalPublisher.h"
#include "local/LocalSubscriber.h"
#include "local/LocalTopic.h"
#include "ntcore_c.h"
#include "ntcore_cpp.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt {
class IListenerStorage;
}  // namespace nt

namespace nt::net {
class ClientMessageHandler;
}  // namespace nt::net

namespace nt::local {

// inner struct to protect against accidentally deadlocking on the mutex
class StorageImpl {
 public:
  StorageImpl(int inst, IListenerStorage& listenerStorage, wpi::Logger& logger);

  wpi::Logger& GetLogger() { return m_logger; }

  //
  // Network interface functions
  //

  void NetworkAnnounce(LocalTopic* topic, std::string_view typeStr,
                       const wpi::json& properties, std::optional<int> pubuid);
  void RemoveNetworkPublisher(LocalTopic* topic);
  void NetworkPropertiesUpdate(LocalTopic* topic, const wpi::json& update,
                               bool ack);

  void ServerSetValue(LocalTopic* topic, const Value& value) {
    if (SetValue(topic, value, NT_EVENT_VALUE_REMOTE, false, nullptr)) {
      if (topic->IsCached()) {
        topic->lastValueNetwork = value;
        topic->lastValueFromNetwork = true;
      }
    }
  }

  void StartNetwork(net::ClientMessageHandler* network);
  void ClearNetwork();

  //
  // Topic functions
  //

  LocalTopic* GetOrCreateTopic(std::string_view name);

  template <std::invocable<const LocalTopic&> F>
  void ForEachTopic(std::string_view prefix, unsigned int types,
                    F&& func) const {
    for (auto&& topic : m_topics) {
      if (!topic->Exists()) {
        continue;
      }
      if (!wpi::starts_with(topic->name, prefix)) {
        continue;
      }
      if (types != 0 && (types & topic->type) == 0) {
        continue;
      }
      func(*topic);
    }
  }

  template <std::invocable<const LocalTopic&> F>
  void ForEachTopic(std::string_view prefix,
                    std::span<const std::string_view> types, F&& func) const {
    for (auto&& topic : m_topics) {
      if (!topic->Exists()) {
        continue;
      }
      if (!wpi::starts_with(topic->name, prefix)) {
        continue;
      }
      if (!types.empty()) {
        bool match = false;
        for (auto&& type : types) {
          if (topic->typeStr == type) {
            match = true;
            break;
          }
        }
        if (!match) {
          continue;
        }
      }
      func(*topic);
    }
  }

  //
  // Topic property functions
  //

  void SetFlags(LocalTopic* topic, unsigned int flags);
  void SetPersistent(LocalTopic* topic, bool value);
  void SetRetained(LocalTopic* topic, bool value);
  void SetCached(LocalTopic* topic, bool value);

  void SetProperty(LocalTopic* topic, std::string_view name,
                   const wpi::json& value);
  bool SetProperties(LocalTopic* topic, const wpi::json& update,
                     bool sendNetwork);

  void DeleteProperty(LocalTopic* topic, std::string_view name);

  //
  // Value functions
  //

  bool SetEntryValue(NT_Handle pubentryHandle, const Value& value);
  bool SetDefaultEntryValue(NT_Handle pubsubentryHandle, const Value& value);

  Value* GetSubEntryValue(NT_Handle subentryHandle) {
    if (auto subscriber = GetSubEntry(subentryHandle)) {
      return &subscriber->topic->lastValue;
    } else {
      return nullptr;
    }
  }

  //
  // Publish/Subscribe/Entry functions
  //

  LocalSubscriber* Subscribe(LocalTopic* topic, NT_Type type,
                             std::string_view typeString,
                             const PubSubOptions& options);

  LocalPublisher* Publish(LocalTopic* topic, NT_Type type,
                          std::string_view typeStr, const wpi::json& properties,
                          const PubSubOptions& options);

  LocalEntry* GetEntry(LocalTopic* topicHandle, NT_Type type,
                       std::string_view typeStr, const PubSubOptions& options);
  LocalEntry* GetEntry(std::string_view name);

  void RemoveSubEntry(NT_Handle subentryHandle);

  void Unpublish(NT_Handle pubentryHandle);

  //
  // Multi-subscriber functions
  //

  LocalMultiSubscriber* AddMultiSubscriber(
      std::span<const std::string_view> prefixes, const PubSubOptions& options);

  std::unique_ptr<LocalMultiSubscriber> RemoveMultiSubscriber(
      NT_MultiSubscriber subHandle);

  //
  // Lookup functions
  //

  LocalTopic* GetTopic(NT_Handle handle);
  LocalTopic* GetTopicByHandle(NT_Topic topicHandle) {
    return m_topics.Get(topicHandle);
  }
  LocalTopic* GetTopicByName(std::string_view name) {
    auto it = m_nameTopics.find(name);
    if (it == m_nameTopics.end()) {
      return nullptr;
    }
    return it->second;
  }
  LocalTopic* GetTopicById(int topicId) {
    return m_topics.Get(Handle{m_inst, topicId, Handle::kTopic});
  }

  LocalSubscriber* GetSubEntry(NT_Handle subentryHandle);

  LocalEntry* GetEntryByHandle(NT_Entry entryHandle) {
    return m_entries.Get(entryHandle);
  }

  LocalMultiSubscriber* GetMultiSubscriberByHandle(NT_MultiSubscriber handle) {
    return m_multiSubscribers.Get(handle);
  }

  LocalSubscriber* GetSubscriberByHandle(NT_Subscriber handle) {
    return m_subscribers.Get(handle);
  }

  //
  // Listener functions
  //

  void AddListenerImpl(NT_Listener listenerHandle, LocalTopic* topic,
                       unsigned int eventMask);
  void AddListenerImpl(NT_Listener listenerHandle, LocalSubscriber* subscriber,
                       unsigned int eventMask, NT_Handle subentryHandle,
                       bool subscriberOwned);
  void AddListenerImpl(NT_Listener listenerHandle,
                       LocalMultiSubscriber* subscriber, unsigned int eventMask,
                       bool subscriberOwned);
  void RemoveListener(NT_Listener listener, unsigned int mask);

  //
  // Data log functions
  //

  LocalDataLogger* StartDataLog(wpi::log::DataLog& log, std::string_view prefix,
                                std::string_view logPrefix);
  void StopDataLog(NT_DataLogger logger);

  //
  // Schema functions
  //

  bool HasSchema(std::string_view name);
  void AddSchema(std::string_view name, std::string_view type,
                 std::span<const uint8_t> schema);

  void Reset();

 private:
  // topic functions
  void NotifyTopic(LocalTopic* topic, unsigned int eventFlags);

  bool SetValue(LocalTopic* topic, const Value& value, unsigned int eventFlags,
                bool suppressIfDuplicate, const LocalPublisher* publisher);
  void NotifyValue(LocalTopic* topic, const Value& value,
                   unsigned int eventFlags, bool isDuplicate,
                   const LocalPublisher* publisher);

  void PropertiesUpdated(LocalTopic* topic, const wpi::json& update,
                         unsigned int eventFlags, bool sendNetwork,
                         bool updateFlags = true);

  void RefreshPubSubActive(LocalTopic* topic, bool warnOnSubMismatch);

  LocalPublisher* AddLocalPublisher(LocalTopic* topic,
                                    const wpi::json& properties,
                                    const PubSubConfig& options);

  std::unique_ptr<LocalPublisher> RemoveLocalPublisher(NT_Publisher pubHandle);

  LocalSubscriber* AddLocalSubscriber(LocalTopic* topic,
                                      const PubSubConfig& options);

  std::unique_ptr<LocalSubscriber> RemoveLocalSubscriber(
      NT_Subscriber subHandle);

  LocalEntry* AddEntry(LocalSubscriber* subscriber) {
    auto entry = m_entries.Add(m_inst, subscriber);
    subscriber->topic->entries.Add(entry);
    return entry;
  }

  std::unique_ptr<LocalEntry> RemoveEntry(NT_Entry entryHandle) {
    auto entry = m_entries.Remove(entryHandle);
    if (entry) {
      entry->topic->entries.Remove(entry.get());
    }
    return entry;
  }

  LocalPublisher* PublishEntry(LocalEntry* entry, NT_Type type);

  bool PublishLocalValue(LocalPublisher* publisher, const Value& value,
                         bool force = false);

 private:
  int m_inst;
  IListenerStorage& m_listenerStorage;
  wpi::Logger& m_logger;
  net::ClientMessageHandler* m_network{nullptr};

  // handle mappings
  HandleMap<LocalTopic, 16> m_topics;
  HandleMap<LocalPublisher, 16> m_publishers;
  HandleMap<LocalSubscriber, 16> m_subscribers;
  HandleMap<LocalEntry, 16> m_entries;
  HandleMap<LocalMultiSubscriber, 16> m_multiSubscribers;
  HandleMap<LocalDataLogger, 16> m_dataloggers;

  // name mappings
  wpi::StringMap<LocalTopic*> m_nameTopics;

  // listeners
  wpi::DenseMap<NT_Listener, std::unique_ptr<LocalListener>> m_listeners;

  // string-based listeners
  VectorSet<LocalListener*> m_topicPrefixListeners;

  // schema publishers
  wpi::StringMap<NT_Publisher> m_schemas;
};

}  // namespace nt::local
