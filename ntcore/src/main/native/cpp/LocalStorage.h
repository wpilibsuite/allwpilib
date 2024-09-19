// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/DenseMap.h>
#include <wpi/StringMap.h>
#include <wpi/Synchronization.h>
#include <wpi/json.h>
#include <wpi/mutex.h>

#include "Handle.h"
#include "HandleMap.h"
#include "PubSubOptions.h"
#include "Types_internal.h"
#include "ValueCircularBuffer.h"
#include "VectorSet.h"
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
  ~LocalStorage() final;

  // network interface functions
  int ServerAnnounce(std::string_view name, int id, std::string_view typeStr,
                     const wpi::json& properties,
                     std::optional<int> pubuid) final;
  void ServerUnannounce(std::string_view name, int id) final;
  void ServerPropertiesUpdate(std::string_view name, const wpi::json& update,
                              bool ack) final;
  void ServerSetValue(int topicId, const Value& value) final;

  void StartNetwork(net::ClientMessageHandler* network) final;
  void ClearNetwork() final;

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
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return topic->name;
    } else {
      return {};
    }
  }

  NT_Type GetTopicType(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return topic->type;
    } else {
      return {};
    }
  }

  std::string GetTopicTypeString(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return topic->typeStr;
    } else {
      return {};
    }
  }

  void SetTopicPersistent(NT_Topic topicHandle, bool value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      m_impl.SetPersistent(topic, value);
    }
  }

  bool GetTopicPersistent(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return (topic->flags & NT_PERSISTENT) != 0;
    } else {
      return false;
    }
  }

  void SetTopicRetained(NT_Topic topicHandle, bool value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      m_impl.SetRetained(topic, value);
    }
  }

  bool GetTopicRetained(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return (topic->flags & NT_RETAINED) != 0;
    } else {
      return false;
    }
  }

  void SetTopicCached(NT_Topic topicHandle, bool value) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      m_impl.SetCached(topic, value);
    }
  }

  bool GetTopicCached(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return (topic->flags & NT_UNCACHED) == 0;
    } else {
      return false;
    }
  }

  bool GetTopicExists(NT_Handle handle) {
    std::scoped_lock lock{m_mutex};
    TopicData* topic = m_impl.GetTopic(handle);
    return topic && topic->Exists();
  }

  wpi::json GetTopicProperty(NT_Topic topicHandle, std::string_view name) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return topic->properties.value(name, wpi::json{});
    } else {
      return {};
    }
  }

  void SetTopicProperty(NT_Topic topic, std::string_view name,
                        const wpi::json& value);

  void DeleteTopicProperty(NT_Topic topic, std::string_view name);

  wpi::json GetTopicProperties(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return topic->properties;
    } else {
      return wpi::json::object();
    }
  }

  bool SetTopicProperties(NT_Topic topic, const wpi::json& update);

  TopicInfo GetTopicInfo(NT_Topic topicHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto topic = m_impl.m_topics.Get(topicHandle)) {
      return topic->GetTopicInfo();
    } else {
      return {};
    }
  }

  NT_Subscriber Subscribe(NT_Topic topic, NT_Type type,
                          std::string_view typeStr,
                          const PubSubOptions& options);

  void Unsubscribe(NT_Subscriber subHandle) {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveSubEntry(subHandle);
  }

  NT_MultiSubscriber SubscribeMultiple(
      std::span<const std::string_view> prefixes, const PubSubOptions& options);

  void UnsubscribeMultiple(NT_MultiSubscriber subHandle) {
    std::scoped_lock lock{m_mutex};
    m_impl.RemoveMultiSubscriber(subHandle);
  }

  NT_Publisher Publish(NT_Topic topic, NT_Type type, std::string_view typeStr,
                       const wpi::json& properties,
                       const PubSubOptions& options);

  void Unpublish(NT_Handle pubentry);

  NT_Entry GetEntry(NT_Topic topic, NT_Type type, std::string_view typeStr,
                    const PubSubOptions& options);

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
      NT_Handle subentry, typename TypeInfo<T>::View defaultValue);

  template <SmallArrayType T>
  Timestamped<typename TypeInfo<T>::SmallRet> GetAtomic(
      NT_Handle subentry,
      wpi::SmallVectorImpl<typename TypeInfo<T>::SmallElem>& buf,
      typename TypeInfo<T>::View defaultValue);

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
      NT_Handle subentry);

  //
  // Backwards compatible user functions
  //

  Value GetEntryValue(NT_Handle subentry);

  void SetEntryFlags(NT_Entry entryHandle, unsigned int flags) {
    std::scoped_lock lock{m_mutex};
    if (auto entry = m_impl.m_entries.Get(entryHandle)) {
      m_impl.SetFlags(entry->subscriber->topic, flags);
    }
  }

  unsigned int GetEntryFlags(NT_Entry entryHandle) {
    std::scoped_lock lock{m_mutex};
    if (auto entry = m_impl.m_entries.Get(entryHandle)) {
      return entry->subscriber->topic->flags;
    } else {
      return 0;
    }
  }

  // Index-only
  NT_Entry GetEntry(std::string_view name);

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

  void AddListener(NT_Listener listener,
                   std::span<const std::string_view> prefixes,
                   unsigned int mask);
  void AddListener(NT_Listener listener, NT_Handle handle, unsigned int mask);

  void RemoveListener(NT_Listener listener, unsigned int mask);

  //
  // Data log functions
  //
  NT_DataLogger StartDataLog(wpi::log::DataLog& log, std::string_view prefix,
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
  static constexpr bool IsSpecial(std::string_view name) {
    return name.empty() ? false : name.front() == '$';
  }

  struct EntryData;
  struct PublisherData;
  struct SubscriberData;
  struct MultiSubscriberData;

  struct DataLoggerEntry {
    DataLoggerEntry(wpi::log::DataLog& log, int entry, NT_DataLogger logger)
        : log{&log}, entry{entry}, logger{logger} {}

    static std::string MakeMetadata(std::string_view properties);

    void Append(const Value& v);

    wpi::log::DataLog* log;
    int entry;
    NT_DataLogger logger;
  };

  struct TopicData {
    static constexpr auto kType = Handle::kTopic;

    TopicData(NT_Topic handle, std::string_view name)
        : handle{handle}, name{name}, special{IsSpecial(name)} {}

    bool Exists() const { return onNetwork || !localPublishers.empty(); }

    bool IsCached() const { return (flags & NT_UNCACHED) == 0; }

    TopicInfo GetTopicInfo() const;

    // invariants
    wpi::SignalObject<NT_Topic> handle;
    std::string name;
    bool special;

    Value lastValue;  // also stores timestamp
    Value lastValueNetwork;
    NT_Type type{NT_UNASSIGNED};
    std::string typeStr;
    unsigned int flags{0};            // for NT3 APIs
    std::string propertiesStr{"{}"};  // cached string for GetTopicInfo() et al
    wpi::json properties = wpi::json::object();
    NT_Entry entry{0};  // cached entry for GetEntry()

    bool onNetwork{false};  // true if there are any remote publishers
    bool lastValueFromNetwork{false};

    wpi::SmallVector<DataLoggerEntry, 1> datalogs;
    NT_Type datalogType{NT_UNASSIGNED};

    VectorSet<PublisherData*> localPublishers;
    VectorSet<SubscriberData*> localSubscribers;
    VectorSet<MultiSubscriberData*> multiSubscribers;
    VectorSet<EntryData*> entries;
    VectorSet<NT_Listener> listeners;
  };

  struct PubSubConfig : public PubSubOptionsImpl {
    PubSubConfig() = default;
    PubSubConfig(NT_Type type, std::string_view typeStr,
                 const PubSubOptions& options)
        : PubSubOptionsImpl{options}, type{type}, typeStr{typeStr} {
      prefixMatch = false;
    }

    NT_Type type{NT_UNASSIGNED};
    std::string typeStr;
  };

  struct PublisherData {
    static constexpr auto kType = Handle::kPublisher;

    PublisherData(NT_Publisher handle, TopicData* topic, PubSubConfig config)
        : handle{handle}, topic{topic}, config{std::move(config)} {}

    void UpdateActive() {
      active = config.type == topic->type && config.typeStr == topic->typeStr;
    }

    // invariants
    wpi::SignalObject<NT_Publisher> handle;
    TopicData* topic;
    PubSubConfig config;

    // whether or not the publisher should actually publish values
    bool active{false};
  };

  struct SubscriberData {
    static constexpr auto kType = Handle::kSubscriber;

    SubscriberData(NT_Subscriber handle, TopicData* topic, PubSubConfig config)
        : handle{handle},
          topic{topic},
          config{std::move(config)},
          pollStorage{config.pollStorage} {}

    void UpdateActive() {
      // for subscribers, unassigned is a wildcard
      // also allow numerically compatible subscribers
      active =
          config.type == NT_UNASSIGNED ||
          (config.type == topic->type && config.typeStr == topic->typeStr) ||
          IsNumericCompatible(config.type, topic->type);
    }

    // invariants
    wpi::SignalObject<NT_Subscriber> handle;
    TopicData* topic;
    PubSubConfig config;

    // whether or not the subscriber should actually receive values
    bool active{false};

    // polling storage
    ValueCircularBuffer pollStorage;

    // value listeners
    VectorSet<NT_Listener> valueListeners;
  };

  struct EntryData {
    static constexpr auto kType = Handle::kEntry;

    EntryData(NT_Entry handle, SubscriberData* subscriber)
        : handle{handle}, topic{subscriber->topic}, subscriber{subscriber} {}

    // invariants
    wpi::SignalObject<NT_Entry> handle;
    TopicData* topic;
    SubscriberData* subscriber;

    // the publisher (created on demand)
    PublisherData* publisher{nullptr};
  };

  struct MultiSubscriberData {
    static constexpr auto kType = Handle::kMultiSubscriber;

    MultiSubscriberData(NT_MultiSubscriber handle,
                        std::span<const std::string_view> prefixes,
                        const PubSubOptionsImpl& options)
        : handle{handle}, options{options} {
      this->options.prefixMatch = true;
      this->prefixes.reserve(prefixes.size());
      for (auto&& prefix : prefixes) {
        this->prefixes.emplace_back(prefix);
      }
    }

    bool Matches(std::string_view name, bool special);

    // invariants
    wpi::SignalObject<NT_MultiSubscriber> handle;
    std::vector<std::string> prefixes;
    PubSubOptionsImpl options;

    // value listeners
    VectorSet<NT_Listener> valueListeners;
  };

  struct ListenerData {
    ListenerData(NT_Listener handle, SubscriberData* subscriber,
                 unsigned int eventMask, bool subscriberOwned)
        : handle{handle},
          eventMask{eventMask},
          subscriber{subscriber},
          subscriberOwned{subscriberOwned} {}
    ListenerData(NT_Listener handle, MultiSubscriberData* subscriber,
                 unsigned int eventMask, bool subscriberOwned)
        : handle{handle},
          eventMask{eventMask},
          multiSubscriber{subscriber},
          subscriberOwned{subscriberOwned} {}

    NT_Listener handle;
    unsigned int eventMask;
    SubscriberData* subscriber{nullptr};
    MultiSubscriberData* multiSubscriber{nullptr};
    bool subscriberOwned;
  };

  struct DataLoggerData {
    static constexpr auto kType = Handle::kDataLogger;

    DataLoggerData(NT_DataLogger handle, wpi::log::DataLog& log,
                   std::string_view prefix, std::string_view logPrefix)
        : handle{handle}, log{log}, prefix{prefix}, logPrefix{logPrefix} {}

    int Start(TopicData* topic, int64_t time);

    NT_DataLogger handle;
    wpi::log::DataLog& log;
    std::string prefix;
    std::string logPrefix;
  };

  // inner struct to protect against accidentally deadlocking on the mutex
  struct Impl {
    Impl(int inst, IListenerStorage& listenerStorage, wpi::Logger& logger);

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

    // topic functions
    void NotifyTopic(TopicData* topic, unsigned int eventFlags);

    void CheckReset(TopicData* topic);

    bool SetValue(TopicData* topic, const Value& value, unsigned int eventFlags,
                  bool suppressIfDuplicate, const PublisherData* publisher);
    void NotifyValue(TopicData* topic, const Value& value,
                     unsigned int eventFlags, bool isDuplicate,
                     const PublisherData* publisher);

    void SetFlags(TopicData* topic, unsigned int flags);
    void SetPersistent(TopicData* topic, bool value);
    void SetRetained(TopicData* topic, bool value);
    void SetCached(TopicData* topic, bool value);
    void SetProperties(TopicData* topic, const wpi::json& update,
                       bool sendNetwork);
    void PropertiesUpdated(TopicData* topic, const wpi::json& update,
                           unsigned int eventFlags, bool sendNetwork,
                           bool updateFlags = true);

    void RefreshPubSubActive(TopicData* topic, bool warnOnSubMismatch);

    void NetworkAnnounce(TopicData* topic, std::string_view typeStr,
                         const wpi::json& properties,
                         std::optional<int> pubuid);
    void RemoveNetworkPublisher(TopicData* topic);
    void NetworkPropertiesUpdate(TopicData* topic, const wpi::json& update,
                                 bool ack);
    void StartNetwork(net::ClientMessageHandler* network);

    PublisherData* AddLocalPublisher(TopicData* topic,
                                     const wpi::json& properties,
                                     const PubSubConfig& options);
    std::unique_ptr<PublisherData> RemoveLocalPublisher(NT_Publisher pubHandle);

    SubscriberData* AddLocalSubscriber(TopicData* topic,
                                       const PubSubConfig& options);
    std::unique_ptr<SubscriberData> RemoveLocalSubscriber(
        NT_Subscriber subHandle);

    EntryData* AddEntry(SubscriberData* subscriber);
    std::unique_ptr<EntryData> RemoveEntry(NT_Entry entryHandle);

    MultiSubscriberData* AddMultiSubscriber(
        std::span<const std::string_view> prefixes,
        const PubSubOptions& options);
    std::unique_ptr<MultiSubscriberData> RemoveMultiSubscriber(
        NT_MultiSubscriber subHandle);

    void AddListenerImpl(NT_Listener listenerHandle, TopicData* topic,
                         unsigned int eventMask);
    void AddListenerImpl(NT_Listener listenerHandle, SubscriberData* subscriber,
                         unsigned int eventMask, NT_Handle subentryHandle,
                         bool subscriberOwned);
    void AddListenerImpl(NT_Listener listenerHandle,
                         MultiSubscriberData* subscriber,
                         unsigned int eventMask, bool subscriberOwned);
    void AddListenerImpl(NT_Listener listenerHandle,
                         std::span<const std::string_view> prefixes,
                         unsigned int eventMask);

    TopicData* GetOrCreateTopic(std::string_view name);
    TopicData* GetTopic(NT_Handle handle);
    SubscriberData* GetSubEntry(NT_Handle subentryHandle);
    PublisherData* PublishEntry(EntryData* entry, NT_Type type);

    Value* GetSubEntryValue(NT_Handle subentryHandle) {
      if (auto subscriber = GetSubEntry(subentryHandle)) {
        return &subscriber->topic->lastValue;
      } else {
        return nullptr;
      }
    }

    bool PublishLocalValue(PublisherData* publisher, const Value& value,
                           bool force = false);

    bool SetEntryValue(NT_Handle pubentryHandle, const Value& value);
    bool SetDefaultEntryValue(NT_Handle pubsubentryHandle, const Value& value);

    void RemoveSubEntry(NT_Handle subentryHandle);
  };

  wpi::mutex m_mutex;
  Impl m_impl;
};

template <ValidType T>
Timestamped<typename TypeInfo<T>::Value> LocalStorage::GetAtomic(
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
Timestamped<typename TypeInfo<T>::SmallRet> LocalStorage::GetAtomic(
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

template <ValidType T>
std::vector<Timestamped<typename TypeInfo<T>::Value>> LocalStorage::ReadQueue(
    NT_Handle subentry) {
  std::scoped_lock lock{m_mutex};
  auto subscriber = m_impl.GetSubEntry(subentry);
  if (!subscriber) {
    return {};
  }
  return subscriber->pollStorage.Read<T>();
}

}  // namespace nt
