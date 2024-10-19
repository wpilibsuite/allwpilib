// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
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

  //
  // Network interface functions
  //

  void NetworkAnnounce(TopicData* topic, std::string_view typeStr,
                       const wpi::json& properties, std::optional<int> pubuid);
  void RemoveNetworkPublisher(TopicData* topic);
  void NetworkPropertiesUpdate(TopicData* topic, const wpi::json& update,
                               bool ack);

  void ServerSetValue(TopicData* topic, const Value& value) {
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

  TopicData* GetOrCreateTopic(std::string_view name);

  template <std::invocable<const TopicData&> F>
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

  template <std::invocable<const TopicData&> F>
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

  void SetFlags(TopicData* topic, unsigned int flags);
  void SetPersistent(TopicData* topic, bool value);
  void SetRetained(TopicData* topic, bool value);
  void SetCached(TopicData* topic, bool value);

  void SetProperty(TopicData* topic, std::string_view name,
                   const wpi::json& value);
  bool SetProperties(TopicData* topic, const wpi::json& update,
                     bool sendNetwork);

  void DeleteProperty(TopicData* topic, std::string_view name);

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

  SubscriberData* Subscribe(TopicData* topic, NT_Type type,
                            std::string_view typeString,
                            const PubSubOptions& options);

  PublisherData* Publish(TopicData* topic, NT_Type type,
                         std::string_view typeStr, const wpi::json& properties,
                         const PubSubOptions& options);

  EntryData* GetEntry(TopicData* topicHandle, NT_Type type,
                      std::string_view typeStr, const PubSubOptions& options);
  TopicData* GetEntry(std::string_view name);

  void RemoveSubEntry(NT_Handle subentryHandle);

  std::unique_ptr<PublisherData> RemoveLocalPublisher(NT_Publisher pubHandle);

  //
  // Multi-subscriber functions
  //

  MultiSubscriberData* AddMultiSubscriber(
      std::span<const std::string_view> prefixes, const PubSubOptions& options);

  std::unique_ptr<MultiSubscriberData> RemoveMultiSubscriber(
      NT_MultiSubscriber subHandle);

  //
  // Lookup functions
  //

  TopicData* GetTopic(NT_Handle handle);
  TopicData* GetTopicByHandle(NT_Topic topicHandle) {
    return m_topics.Get(topicHandle);
  }
  TopicData* GetTopicByName(std::string_view name) {
    return m_nameTopics.lookup(name);
  }
  TopicData* GetTopicById(int topicId) {
    return m_topics.Get(Handle{m_inst, topicId, Handle::kTopic});
  }

  SubscriberData* GetSubEntry(NT_Handle subentryHandle);

  EntryData* GetEntryByHandle(NT_Entry entryHandle) {
    return m_entries.Get(entryHandle);
  }

  MultiSubscriberData* GetMultiSubscriberByHandle(NT_MultiSubscriber handle) {
    return m_multiSubscribers.Get(handle);
  }

  SubscriberData* GetSubscriberByHandle(NT_Subscriber handle) {
    return m_subscribers.Get(handle);
  }

  //
  // Listener functions
  //

  void AddListenerImpl(NT_Listener listenerHandle, TopicData* topic,
                       unsigned int eventMask);
  void AddListenerImpl(NT_Listener listenerHandle, SubscriberData* subscriber,
                       unsigned int eventMask, NT_Handle subentryHandle,
                       bool subscriberOwned);
  void AddListenerImpl(NT_Listener listenerHandle,
                       MultiSubscriberData* subscriber, unsigned int eventMask,
                       bool subscriberOwned);
  void RemoveListener(NT_Listener listener, unsigned int mask);

  //
  // Data log functions
  //

  DataLoggerData* StartDataLog(wpi::log::DataLog& log, std::string_view prefix,
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
  void NotifyTopic(TopicData* topic, unsigned int eventFlags);

  void CheckReset(TopicData* topic);

  bool SetValue(TopicData* topic, const Value& value, unsigned int eventFlags,
                bool suppressIfDuplicate, const PublisherData* publisher);
  void NotifyValue(TopicData* topic, const Value& value,
                   unsigned int eventFlags, bool isDuplicate,
                   const PublisherData* publisher);

  void PropertiesUpdated(TopicData* topic, const wpi::json& update,
                         unsigned int eventFlags, bool sendNetwork,
                         bool updateFlags = true);

  void RefreshPubSubActive(TopicData* topic, bool warnOnSubMismatch);

  PublisherData* AddLocalPublisher(TopicData* topic,
                                   const wpi::json& properties,
                                   const PubSubConfig& options);

  SubscriberData* AddLocalSubscriber(TopicData* topic,
                                     const PubSubConfig& options);

  std::unique_ptr<SubscriberData> RemoveLocalSubscriber(
      NT_Subscriber subHandle);

  EntryData* AddEntry(SubscriberData* subscriber) {
    auto entry = m_entries.Add(m_inst, subscriber);
    subscriber->topic->entries.Add(entry);
    return entry;
  }

  std::unique_ptr<EntryData> RemoveEntry(NT_Entry entryHandle) {
    auto entry = m_entries.Remove(entryHandle);
    if (entry) {
      entry->topic->entries.Remove(entry.get());
    }
    return entry;
  }

  PublisherData* PublishEntry(EntryData* entry, NT_Type type);

  bool PublishLocalValue(PublisherData* publisher, const Value& value,
                         bool force = false);

 private:
  int m_inst;
  IListenerStorage& m_listenerStorage;
  wpi::Logger& m_logger;
  net::ClientMessageHandler* m_network{nullptr};

  // handle mappings
  HandleMap<TopicData, 16> m_topics;
  HandleMap<PublisherData, 16> m_publishers;
  HandleMap<SubscriberData, 16> m_subscribers;
  HandleMap<EntryData, 16> m_entries;
  HandleMap<MultiSubscriberData, 16> m_multiSubscribers;
  HandleMap<DataLoggerData, 16> m_dataloggers;

  // name mappings
  wpi::StringMap<TopicData*> m_nameTopics;

  // listeners
  wpi::DenseMap<NT_Listener, std::unique_ptr<ListenerData>> m_listeners;

  // string-based listeners
  VectorSet<ListenerData*> m_topicPrefixListeners;

  // schema publishers
  wpi::StringMap<NT_Publisher> m_schemas;
};

}  // namespace nt::local
