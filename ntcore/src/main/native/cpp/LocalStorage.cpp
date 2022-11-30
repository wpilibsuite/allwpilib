// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalStorage.h"

#include <algorithm>

#include <wpi/DataLog.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/Synchronization.h>
#include <wpi/UidVector.h>
#include <wpi/circular_buffer.h>
#include <wpi/json.h>

#include "Handle.h"
#include "HandleMap.h"
#include "IListenerStorage.h"
#include "Log.h"
#include "PubSubOptions.h"
#include "Types_internal.h"
#include "Value_internal.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"

using namespace nt;

// maximum number of local publishers / subscribers to any given topic
static constexpr size_t kMaxPublishers = 512;
static constexpr size_t kMaxSubscribers = 512;
static constexpr size_t kMaxMultiSubscribers = 512;
static constexpr size_t kMaxListeners = 512;

namespace {

static constexpr bool IsSpecial(std::string_view name) {
  return name.empty() ? false : name.front() == '$';
}

static constexpr bool PrefixMatch(std::string_view name,
                                  std::string_view prefix, bool special) {
  return (!special || !prefix.empty()) && wpi::starts_with(name, prefix);
}

// Utility wrapper for making a set-like vector
template <typename T>
class VectorSet : public std::vector<T> {
 public:
  void Add(T value) { this->push_back(value); }
  void Remove(T value) { std::erase(*this, value); }
};

struct EntryData;
struct PublisherData;
struct SubscriberData;
struct MultiSubscriberData;

struct DataLoggerEntry {
  DataLoggerEntry(wpi::log::DataLog& log, int entry, NT_DataLogger logger)
      : log{&log}, entry{entry}, logger{logger} {}

  static std::string MakeMetadata(std::string_view properties) {
    return fmt::format("{{\"properties\":{},\"source\":\"NT\"}}", properties);
  }

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

  wpi::SmallVector<DataLoggerEntry, 1> datalogs;
  NT_Type datalogType{NT_UNASSIGNED};

  VectorSet<PublisherData*> localPublishers;
  VectorSet<SubscriberData*> localSubscribers;
  VectorSet<MultiSubscriberData*> multiSubscribers;
  VectorSet<EntryData*> entries;
  VectorSet<NT_Listener> listeners;
};

struct PubSubConfig : public PubSubOptions {
  PubSubConfig() = default;
  PubSubConfig(NT_Type type, std::string_view typeStr,
               std::span<const PubSubOption> options)
      : PubSubOptions{options}, type{type}, typeStr{typeStr} {}

  NT_Type type{NT_UNASSIGNED};
  std::string typeStr;
};

struct PublisherData {
  static constexpr auto kType = Handle::kPublisher;

  PublisherData(NT_Publisher handle, TopicData* topic, PubSubConfig config)
      : handle{handle}, topic{topic}, config{std::move(config)} {}

  void UpdateActive();

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
        pollStorage{config.pollStorageSize} {}

  void UpdateActive();

  // invariants
  wpi::SignalObject<NT_Subscriber> handle;
  TopicData* topic;
  PubSubConfig config;

  // whether or not the subscriber should actually receive values
  bool active{false};

  // polling storage
  wpi::circular_buffer<Value> pollStorage;

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
                      PubSubOptions options)
      : handle{handle}, options{std::move(options)} {
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
  PubSubOptions options;

  // value listeners
  VectorSet<NT_Listener> valueListeners;
};

bool MultiSubscriberData::Matches(std::string_view name, bool special) {
  for (auto&& prefix : prefixes) {
    if (PrefixMatch(name, prefix, special)) {
      return true;
    }
  }
  return false;
}

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

  int Start(TopicData* topic, int64_t time) {
    return log.Start(fmt::format("{}{}", logPrefix,
                                 wpi::drop_front(topic->name, prefix.size())),
                     topic->typeStr,
                     DataLoggerEntry::MakeMetadata(topic->propertiesStr), time);
  }

  NT_DataLogger handle;
  wpi::log::DataLog& log;
  std::string prefix;
  std::string logPrefix;
};

struct LSImpl {
  LSImpl(int inst, IListenerStorage& listenerStorage, wpi::Logger& logger)
      : m_inst{inst}, m_listenerStorage{listenerStorage}, m_logger{logger} {}

  int m_inst;
  IListenerStorage& m_listenerStorage;
  wpi::Logger& m_logger;
  net::NetworkInterface* m_network{nullptr};

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

  // topic functions
  void NotifyTopic(TopicData* topic, unsigned int eventFlags);

  void CheckReset(TopicData* topic);

  bool SetValue(TopicData* topic, const Value& value, unsigned int eventFlags,
                bool isDuplicate);
  void NotifyValue(TopicData* topic, unsigned int eventFlags, bool isDuplicate);

  void SetFlags(TopicData* topic, unsigned int flags);
  void SetPersistent(TopicData* topic, bool value);
  void SetRetained(TopicData* topic, bool value);
  void SetProperties(TopicData* topic, const wpi::json& update,
                     bool sendNetwork);
  void PropertiesUpdated(TopicData* topic, const wpi::json& update,
                         unsigned int eventFlags, bool sendNetwork,
                         bool updateFlags = true);

  void RefreshPubSubActive(TopicData* topic, bool warnOnSubMismatch);

  void NetworkAnnounce(TopicData* topic, std::string_view typeStr,
                       const wpi::json& properties, NT_Publisher pubHandle);
  void RemoveNetworkPublisher(TopicData* topic);
  void NetworkPropertiesUpdate(TopicData* topic, const wpi::json& update,
                               bool ack);

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
      std::span<const std::string_view> prefixes, const PubSubOptions& options);
  std::unique_ptr<MultiSubscriberData> RemoveMultiSubscriber(
      NT_MultiSubscriber subHandle);

  void AddListenerImpl(NT_Listener listenerHandle, TopicData* topic,
                       unsigned int eventMask);
  void AddListenerImpl(NT_Listener listenerHandle, SubscriberData* subscriber,
                       unsigned int eventMask, NT_Handle subentryHandle,
                       bool subscriberOwned);
  void AddListenerImpl(NT_Listener listenerHandle,
                       MultiSubscriberData* subscriber, unsigned int eventMask,
                       bool subscriberOwned);
  void AddListenerImpl(NT_Listener listenerHandle,
                       std::span<const std::string_view> prefixes,
                       unsigned int eventMask);

  void AddListener(NT_Listener listenerHandle,
                   std::span<const std::string_view> prefixes,
                   unsigned int mask);
  void AddListener(NT_Listener listenerHandle, NT_Handle handle,
                   unsigned int mask);
  void RemoveListener(NT_Listener listenerHandle, unsigned int mask);

  TopicData* GetOrCreateTopic(std::string_view name);
  TopicData* GetTopic(NT_Handle handle);
  SubscriberData* GetSubEntry(NT_Handle subentryHandle);
  PublisherData* PublishEntry(EntryData* entry, NT_Type type);
  Value* GetSubEntryValue(NT_Handle subentryHandle);

  bool PublishLocalValue(PublisherData* publisher, const Value& value,
                         bool force = false);

  bool SetEntryValue(NT_Handle pubentryHandle, const Value& value);
  bool SetDefaultEntryValue(NT_Handle pubsubentryHandle, const Value& value);

  void RemoveSubEntry(NT_Handle subentryHandle);
};

}  // namespace

void DataLoggerEntry::Append(const Value& v) {
  auto time = v.time();
  switch (v.type()) {
    case NT_BOOLEAN:
      log->AppendBoolean(entry, v.GetBoolean(), time);
      break;
    case NT_INTEGER:
      log->AppendInteger(entry, v.GetInteger(), time);
      break;
    case NT_FLOAT:
      log->AppendFloat(entry, v.GetFloat(), time);
      break;
    case NT_DOUBLE:
      log->AppendDouble(entry, v.GetDouble(), time);
      break;
    case NT_STRING:
      log->AppendString(entry, v.GetString(), time);
      break;
    case NT_RAW: {
      auto val = v.GetRaw();
      log->AppendRaw(entry,
                     {reinterpret_cast<const uint8_t*>(val.data()), val.size()},
                     time);
      break;
    }
    case NT_BOOLEAN_ARRAY:
      log->AppendBooleanArray(entry, v.GetBooleanArray(), time);
      break;
    case NT_INTEGER_ARRAY:
      log->AppendIntegerArray(entry, v.GetIntegerArray(), time);
      break;
    case NT_FLOAT_ARRAY:
      log->AppendFloatArray(entry, v.GetFloatArray(), time);
      break;
    case NT_DOUBLE_ARRAY:
      log->AppendDoubleArray(entry, v.GetDoubleArray(), time);
      break;
    case NT_STRING_ARRAY:
      log->AppendStringArray(entry, v.GetStringArray(), time);
      break;
    default:
      break;
  }
}

TopicInfo TopicData::GetTopicInfo() const {
  TopicInfo info;
  info.topic = handle;
  info.name = name;
  info.type = type;
  info.type_str = typeStr;
  info.properties = propertiesStr;
  return info;
}

void PublisherData::UpdateActive() {
  active = config.type == topic->type && config.typeStr == topic->typeStr;
}

void SubscriberData::UpdateActive() {
  // for subscribers, unassigned is a wildcard
  // also allow numerically compatible subscribers
  active = config.type == NT_UNASSIGNED ||
           (config.type == topic->type && config.typeStr == topic->typeStr) ||
           IsNumericCompatible(config.type, topic->type);
}

void LSImpl::NotifyTopic(TopicData* topic, unsigned int eventFlags) {
  DEBUG4("NotifyTopic({}, {})\n", topic->name, eventFlags);
  auto topicInfo = topic->GetTopicInfo();
  if (!topic->listeners.empty()) {
    m_listenerStorage.Notify(topic->listeners, eventFlags, topicInfo);
  }

  wpi::SmallVector<NT_Listener, 32> listeners;
  for (auto listener : m_topicPrefixListeners) {
    if (listener->multiSubscriber &&
        listener->multiSubscriber->Matches(topic->name, topic->special)) {
      listeners.emplace_back(listener->handle);
    }
  }
  if (!listeners.empty()) {
    m_listenerStorage.Notify(listeners, eventFlags, topicInfo);
  }

  if ((eventFlags & (NT_EVENT_PUBLISH | NT_EVENT_UNPUBLISH)) != 0) {
    if (!m_dataloggers.empty()) {
      auto now = Now();
      for (auto&& datalogger : m_dataloggers) {
        if (wpi::starts_with(topic->name, datalogger->prefix)) {
          auto it = std::find_if(topic->datalogs.begin(), topic->datalogs.end(),
                                 [&](const auto& elem) {
                                   return elem.logger == datalogger->handle;
                                 });
          if ((eventFlags & NT_EVENT_PUBLISH) != 0 &&
              it == topic->datalogs.end()) {
            topic->datalogs.emplace_back(datalogger->log,
                                         datalogger->Start(topic, now),
                                         datalogger->handle);
            topic->datalogType = topic->type;
          } else if ((eventFlags & NT_EVENT_UNPUBLISH) != 0 &&
                     it != topic->datalogs.end()) {
            it->log->Finish(it->entry, now);
            topic->datalogType = NT_UNASSIGNED;
            topic->datalogs.erase(it);
          }
        }
      }
    }
  } else if ((eventFlags & NT_EVENT_PROPERTIES) != 0) {
    if (!topic->datalogs.empty()) {
      auto metadata = DataLoggerEntry::MakeMetadata(topic->propertiesStr);
      for (auto&& datalog : topic->datalogs) {
        datalog.log->SetMetadata(datalog.entry, metadata);
      }
    }
  }
}

void LSImpl::CheckReset(TopicData* topic) {
  if (topic->Exists()) {
    return;
  }
  topic->lastValue = {};
  topic->lastValueNetwork = {};
  topic->type = NT_UNASSIGNED;
  topic->typeStr.clear();
  topic->flags = 0;
  topic->properties = wpi::json::object();
  topic->propertiesStr = "{}";
}

bool LSImpl::SetValue(TopicData* topic, const Value& value,
                      unsigned int eventFlags, bool isDuplicate) {
  DEBUG4("SetValue({}, {}, {}, {})", topic->name, value.time(), eventFlags,
         isDuplicate);
  if (topic->type != NT_UNASSIGNED && topic->type != value.type()) {
    return false;
  }
  if (!topic->lastValue || value.time() >= topic->lastValue.time()) {
    // TODO: notify option even if older value
    topic->type = value.type();
    topic->lastValue = value;
    NotifyValue(topic, eventFlags, isDuplicate);
  }
  if (!isDuplicate && topic->datalogType == value.type()) {
    for (auto&& datalog : topic->datalogs) {
      datalog.Append(value);
    }
  }
  return true;
}

void LSImpl::NotifyValue(TopicData* topic, unsigned int eventFlags,
                         bool isDuplicate) {
  bool isNetwork = (eventFlags & NT_EVENT_VALUE_REMOTE) != 0;
  for (auto&& subscriber : topic->localSubscribers) {
    if (subscriber->active &&
        (subscriber->config.keepDuplicates || !isDuplicate) &&
        ((isNetwork && subscriber->config.fromRemote) ||
         (!isNetwork && subscriber->config.fromLocal))) {
      subscriber->pollStorage.emplace_back(topic->lastValue);
      subscriber->handle.Set();
      if (!subscriber->valueListeners.empty()) {
        m_listenerStorage.Notify(subscriber->valueListeners, eventFlags,
                                 topic->handle, 0, topic->lastValue);
      }
    }
  }

  for (auto&& subscriber : topic->multiSubscribers) {
    if (subscriber->options.keepDuplicates || !isDuplicate) {
      subscriber->handle.Set();
      if (!subscriber->valueListeners.empty()) {
        m_listenerStorage.Notify(subscriber->valueListeners, eventFlags,
                                 topic->handle, 0, topic->lastValue);
      }
    }
  }
}

void LSImpl::SetFlags(TopicData* topic, unsigned int flags) {
  wpi::json update = wpi::json::object();
  if ((flags & NT_PERSISTENT) != 0) {
    topic->properties["persistent"] = true;
    update["persistent"] = true;
  } else {
    topic->properties.erase("persistent");
    update["persistent"] = wpi::json();
  }
  if ((flags & NT_RETAINED) != 0) {
    topic->properties["retained"] = true;
    update["retained"] = true;
  } else {
    topic->properties.erase("retained");
    update["retained"] = wpi::json();
  }
  topic->flags = flags;
  if (!update.empty()) {
    PropertiesUpdated(topic, update, NT_EVENT_NONE, true, false);
  }
}

void LSImpl::SetPersistent(TopicData* topic, bool value) {
  wpi::json update = wpi::json::object();
  if (value) {
    topic->flags |= NT_PERSISTENT;
    topic->properties["persistent"] = true;
    update["persistent"] = true;
  } else {
    topic->flags &= ~NT_PERSISTENT;
    topic->properties.erase("persistent");
    update["persistent"] = wpi::json();
  }
  PropertiesUpdated(topic, update, NT_EVENT_NONE, true, false);
}

void LSImpl::SetRetained(TopicData* topic, bool value) {
  wpi::json update = wpi::json::object();
  if (value) {
    topic->flags |= NT_RETAINED;
    topic->properties["retained"] = true;
    update["retained"] = true;
  } else {
    topic->flags &= ~NT_RETAINED;
    topic->properties.erase("retained");
    update["retained"] = wpi::json();
  }
  PropertiesUpdated(topic, update, NT_EVENT_NONE, true, false);
}

void LSImpl::SetProperties(TopicData* topic, const wpi::json& update,
                           bool sendNetwork) {
  if (!update.is_object()) {
    return;
  }
  DEBUG4("SetProperties({},{})", topic->name, sendNetwork);
  for (auto&& change : update.items()) {
    if (change.value().is_null()) {
      topic->properties.erase(change.key());
    } else {
      topic->properties[change.key()] = change.value();
    }
  }
  PropertiesUpdated(topic, update, NT_EVENT_NONE, sendNetwork);
}

void LSImpl::PropertiesUpdated(TopicData* topic, const wpi::json& update,
                               unsigned int eventFlags, bool sendNetwork,
                               bool updateFlags) {
  DEBUG4("PropertiesUpdated({}, {}, {}, {}, {})", topic->name, update.dump(),
         eventFlags, sendNetwork, updateFlags);
  if (updateFlags) {
    // set flags from properties
    auto it = topic->properties.find("persistent");
    if (it != topic->properties.end()) {
      if (auto val = it->get_ptr<bool*>()) {
        if (*val) {
          topic->flags |= NT_PERSISTENT;
        } else {
          topic->flags &= ~NT_PERSISTENT;
        }
      }
    }
    it = topic->properties.find("retained");
    if (it != topic->properties.end()) {
      if (auto val = it->get_ptr<bool*>()) {
        if (*val) {
          topic->flags |= NT_RETAINED;
        } else {
          topic->flags &= ~NT_RETAINED;
        }
      }
    }
  }

  topic->propertiesStr = topic->properties.dump();
  NotifyTopic(topic, eventFlags | NT_EVENT_PROPERTIES);
  // check local flag so we don't echo back received properties changes
  if (m_network && sendNetwork) {
    m_network->SetProperties(topic->handle, topic->name, update);
  }
}

void LSImpl::RefreshPubSubActive(TopicData* topic, bool warnOnSubMismatch) {
  for (auto&& publisher : topic->localPublishers) {
    publisher->UpdateActive();
  }
  for (auto&& subscriber : topic->localSubscribers) {
    subscriber->UpdateActive();
    if (warnOnSubMismatch && topic->Exists() && !subscriber->active) {
      // warn on type mismatch
      INFO(
          "local subscribe to '{}' disabled due to type mismatch (wanted '{}', "
          "published as '{}')",
          topic->name, subscriber->config.typeStr, topic->typeStr);
    }
  }
}

void LSImpl::NetworkAnnounce(TopicData* topic, std::string_view typeStr,
                             const wpi::json& properties,
                             NT_Publisher pubHandle) {
  DEBUG4("LS NetworkAnnounce({}, {}, {}, {})", topic->name, typeStr,
         properties.dump(), pubHandle);
  if (pubHandle != 0) {
    return;  // ack of our publish; ignore
  }

  unsigned int event = NT_EVENT_NONE;
  // fresh non-local publish; the network publish always sets the type even
  // if it was locally published, but output a diagnostic for this case
  bool didExist = topic->Exists();
  topic->onNetwork = true;
  NT_Type type = StringToType(typeStr);
  if (topic->type != type || topic->typeStr != typeStr) {
    if (didExist) {
      INFO(
          "network announce of '{}' overriding local publish (was '{}', now "
          "'{}')",
          topic->name, topic->typeStr, typeStr);
    }
    topic->type = type;
    topic->typeStr = typeStr;
    RefreshPubSubActive(topic, true);
  }
  if (!didExist) {
    event |= NT_EVENT_PUBLISH;
  }

  // may be properties update, but need to compare to see if it actually
  // changed to determine whether to update string / send event
  wpi::json update = wpi::json::object();
  // added/changed
  for (auto&& prop : properties.items()) {
    auto it = topic->properties.find(prop.key());
    if (it == topic->properties.end() || *it != prop.value()) {
      update[prop.key()] = prop.value();
    }
  }
  // removed
  for (auto&& prop : topic->properties.items()) {
    if (properties.find(prop.key()) == properties.end()) {
      update[prop.key()] = wpi::json();
    }
  }
  if (!update.empty()) {
    topic->properties = properties;
    PropertiesUpdated(topic, update, event, false);
  } else if (event != NT_EVENT_NONE) {
    NotifyTopic(topic, event);
  }
}

void LSImpl::RemoveNetworkPublisher(TopicData* topic) {
  DEBUG4("LS RemoveNetworkPublisher({}, {})", topic->handle, topic->name);
  // this acts as an unpublish
  bool didExist = topic->Exists();
  topic->onNetwork = false;
  if (didExist && !topic->Exists()) {
    DEBUG4("Unpublished {}", topic->name);
    CheckReset(topic);
    NotifyTopic(topic, NT_EVENT_UNPUBLISH);
  }

  if (!topic->localPublishers.empty()) {
    // some other publisher still exists; if it has a different type, refresh
    // and publish it over the network
    auto& nextPub = topic->localPublishers.front();
    if (nextPub->config.type != topic->type ||
        nextPub->config.typeStr != topic->typeStr) {
      topic->type = nextPub->config.type;
      topic->typeStr = nextPub->config.typeStr;
      RefreshPubSubActive(topic, false);
      // this may result in a duplicate publish warning on the server side,
      // but send one anyway in this case just to be sure
      if (nextPub->active && m_network) {
        m_network->Publish(nextPub->handle, topic->handle, topic->name,
                           topic->typeStr, topic->properties, nextPub->config);
      }
    }
  }
}

void LSImpl::NetworkPropertiesUpdate(TopicData* topic, const wpi::json& update,
                                     bool ack) {
  DEBUG4("NetworkPropertiesUpdate({},{})", topic->name, ack);
  if (ack) {
    return;  // ignore acks
  }
  SetProperties(topic, update, false);
}

PublisherData* LSImpl::AddLocalPublisher(TopicData* topic,
                                         const wpi::json& properties,
                                         const PubSubConfig& config) {
  bool didExist = topic->Exists();
  auto publisher = m_publishers.Add(m_inst, topic, config);
  topic->localPublishers.Add(publisher);

  if (!didExist) {
    DEBUG4("AddLocalPublisher: setting {} type {} typestr {}", topic->name,
           static_cast<int>(config.type), config.typeStr);
    // set the type to the published type
    topic->type = config.type;
    topic->typeStr = config.typeStr;
    RefreshPubSubActive(topic, true);

    if (properties.is_null()) {
      topic->properties = wpi::json::object();
    } else if (properties.is_object()) {
      topic->properties = properties;
    } else {
      WARNING("ignoring non-object properties when publishing '{}'",
              topic->name);
      topic->properties = wpi::json::object();
    }

    if (topic->properties.empty()) {
      NotifyTopic(topic, NT_EVENT_PUBLISH);
    } else {
      PropertiesUpdated(topic, topic->properties, NT_EVENT_PUBLISH, false);
    }
  } else {
    // only need to update just this publisher
    publisher->UpdateActive();
    if (!publisher->active) {
      // warn on type mismatch
      INFO(
          "local publish to '{}' disabled due to type mismatch (wanted '{}', "
          "currently '{}')",
          topic->name, config.typeStr, topic->typeStr);
    }
  }

  if (publisher->active && m_network) {
    m_network->Publish(publisher->handle, topic->handle, topic->name,
                       topic->typeStr, topic->properties, config);
  }
  return publisher;
}

std::unique_ptr<PublisherData> LSImpl::RemoveLocalPublisher(
    NT_Publisher pubHandle) {
  auto publisher = m_publishers.Remove(pubHandle);
  if (publisher) {
    auto topic = publisher->topic;
    bool didExist = topic->Exists();
    topic->localPublishers.Remove(publisher.get());
    if (didExist && !topic->Exists()) {
      CheckReset(topic);
      NotifyTopic(topic, NT_EVENT_UNPUBLISH);
    }

    if (publisher->active && m_network) {
      m_network->Unpublish(publisher->handle, topic->handle);
    }

    if (publisher->active && !topic->localPublishers.empty()) {
      // some other publisher still exists; if it has a different type, refresh
      // and publish it over the network
      auto& nextPub = topic->localPublishers.front();
      if (nextPub->config.type != topic->type ||
          nextPub->config.typeStr != topic->typeStr) {
        topic->type = nextPub->config.type;
        topic->typeStr = nextPub->config.typeStr;
        RefreshPubSubActive(topic, false);
        if (nextPub->active && m_network) {
          m_network->Publish(nextPub->handle, topic->handle, topic->name,
                             topic->typeStr, topic->properties,
                             nextPub->config);
        }
      }
    }
  }
  return publisher;
}

SubscriberData* LSImpl::AddLocalSubscriber(TopicData* topic,
                                           const PubSubConfig& config) {
  DEBUG4("AddLocalSubscriber({})", topic->name);
  auto subscriber = m_subscribers.Add(m_inst, topic, config);
  topic->localSubscribers.Add(subscriber);
  // set subscriber to active if the type matches
  subscriber->UpdateActive();
  if (topic->Exists() && !subscriber->active) {
    // warn on type mismatch
    INFO(
        "local subscribe to '{}' disabled due to type mismatch (wanted '{}', "
        "published as '{}')",
        topic->name, config.typeStr, topic->typeStr);
  }
  if (m_network) {
    DEBUG4("-> NetworkSubscribe({})", topic->name);
    m_network->Subscribe(subscriber->handle, {{topic->name}}, config);
  }
  return subscriber;
}

std::unique_ptr<SubscriberData> LSImpl::RemoveLocalSubscriber(
    NT_Subscriber subHandle) {
  auto subscriber = m_subscribers.Remove(subHandle);
  if (subscriber) {
    auto topic = subscriber->topic;
    topic->localSubscribers.Remove(subscriber.get());
    for (auto&& listener : m_listeners) {
      if (listener.getSecond()->subscriber == subscriber.get()) {
        listener.getSecond()->subscriber = nullptr;
      }
    }
    if (m_network) {
      m_network->Unsubscribe(subscriber->handle);
    }
  }
  return subscriber;
}

EntryData* LSImpl::AddEntry(SubscriberData* subscriber) {
  auto entry = m_entries.Add(m_inst, subscriber);
  subscriber->topic->entries.Add(entry);
  return entry;
}

std::unique_ptr<EntryData> LSImpl::RemoveEntry(NT_Entry entryHandle) {
  auto entry = m_entries.Remove(entryHandle);
  if (entry) {
    entry->topic->entries.Remove(entry.get());
  }
  return entry;
}

MultiSubscriberData* LSImpl::AddMultiSubscriber(
    std::span<const std::string_view> prefixes, const PubSubOptions& options) {
  auto subscriber = m_multiSubscribers.Add(m_inst, prefixes, options);
  // subscribe to any already existing topics
  for (auto&& topic : m_topics) {
    for (auto&& prefix : prefixes) {
      if (PrefixMatch(topic->name, prefix, topic->special)) {
        topic->multiSubscribers.Add(subscriber);
        break;
      }
    }
  }
  if (m_network) {
    m_network->Subscribe(subscriber->handle, subscriber->prefixes,
                         subscriber->options);
  }
  return subscriber;
}

std::unique_ptr<MultiSubscriberData> LSImpl::RemoveMultiSubscriber(
    NT_MultiSubscriber subHandle) {
  auto subscriber = m_multiSubscribers.Remove(subHandle);
  if (subscriber) {
    for (auto&& topic : m_topics) {
      topic->multiSubscribers.Remove(subscriber.get());
    }
    for (auto&& listener : m_listeners) {
      if (listener.getSecond()->multiSubscriber == subscriber.get()) {
        listener.getSecond()->multiSubscriber = nullptr;
      }
    }
    if (m_network) {
      m_network->Unsubscribe(subscriber->handle);
    }
  }
  return subscriber;
}

void LSImpl::AddListenerImpl(NT_Listener listenerHandle, TopicData* topic,
                             unsigned int eventMask) {
  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    ERROR(
        "reached maximum number of subscribers to '{}', ignoring listener add",
        topic->name);
    return;
  }
  // subscribe to make sure topic updates are received
  PubSubConfig config;
  config.topicsOnly = (eventMask & NT_EVENT_VALUE_ALL) == 0;
  auto sub = AddLocalSubscriber(topic, config);
  AddListenerImpl(listenerHandle, sub, eventMask, sub->handle, true);
}

void LSImpl::AddListenerImpl(NT_Listener listenerHandle,
                             SubscriberData* subscriber, unsigned int eventMask,
                             NT_Handle subentryHandle, bool subscriberOwned) {
  m_listeners.try_emplace(listenerHandle, std::make_unique<ListenerData>(
                                              listenerHandle, subscriber,
                                              eventMask, subscriberOwned));

  auto topic = subscriber->topic;

  if ((eventMask & NT_EVENT_TOPIC) != 0) {
    if (topic->listeners.size() >= kMaxListeners) {
      ERROR("reached maximum number of listeners to '{}', not adding listener",
            topic->name);
      return;
    }

    m_listenerStorage.Activate(
        listenerHandle, eventMask & (NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE));

    topic->listeners.Add(listenerHandle);

    // handle immediate publish
    if ((eventMask & (NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE)) ==
            (NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE) &&
        topic->Exists()) {
      m_listenerStorage.Notify({&listenerHandle, 1},
                               NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE,
                               topic->GetTopicInfo());
    }
  }

  if ((eventMask & NT_EVENT_VALUE_ALL) != 0) {
    if (subscriber->valueListeners.size() >= kMaxListeners) {
      ERROR("reached maximum number of listeners to '{}', not adding listener",
            topic->name);
      return;
    }
    m_listenerStorage.Activate(
        listenerHandle, eventMask & (NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE),
        [subentryHandle](unsigned int mask, Event* event) {
          if (auto valueData = event->GetValueEventData()) {
            valueData->subentry = subentryHandle;
          }
          return true;
        });

    subscriber->valueListeners.Add(listenerHandle);

    // handle immediate value
    if ((eventMask & NT_EVENT_VALUE_ALL) != 0 &&
        (eventMask & NT_EVENT_IMMEDIATE) != 0 && topic->lastValue) {
      m_listenerStorage.Notify({&listenerHandle, 1},
                               NT_EVENT_IMMEDIATE | NT_EVENT_VALUE_ALL,
                               topic->handle, subentryHandle, topic->lastValue);
    }
  }
}

void LSImpl::AddListenerImpl(NT_Listener listenerHandle,
                             MultiSubscriberData* subscriber,
                             unsigned int eventMask, bool subscriberOwned) {
  auto listener =
      m_listeners
          .try_emplace(listenerHandle, std::make_unique<ListenerData>(
                                           listenerHandle, subscriber,
                                           eventMask, subscriberOwned))
          .first->getSecond()
          .get();

  // if we're doing anything immediate, get the list of matching topics
  wpi::SmallVector<TopicData*, 32> topics;
  if ((eventMask & NT_EVENT_IMMEDIATE) != 0 &&
      (eventMask & (NT_EVENT_PUBLISH | NT_EVENT_VALUE_ALL)) != 0) {
    for (auto&& topic : m_topics) {
      if (topic->Exists() && subscriber->Matches(topic->name, topic->special)) {
        topics.emplace_back(topic.get());
      }
    }
  }

  if ((eventMask & NT_EVENT_TOPIC) != 0) {
    if (m_topicPrefixListeners.size() >= kMaxListeners) {
      ERROR("reached maximum number of listeners, not adding listener");
      return;
    }

    m_listenerStorage.Activate(
        listenerHandle, eventMask & (NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE));

    m_topicPrefixListeners.Add(listener);

    // handle immediate publish
    if ((eventMask & (NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE)) ==
        (NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE)) {
      std::vector<TopicInfo> topicInfos;
      for (auto&& topic : topics) {
        topicInfos.emplace_back(topic->GetTopicInfo());
      }
      if (!topicInfos.empty()) {
        m_listenerStorage.Notify({&listenerHandle, 1},
                                 NT_EVENT_PUBLISH | NT_EVENT_IMMEDIATE,
                                 topicInfos);
      }
    }
  }

  if ((eventMask & NT_EVENT_VALUE_ALL) != 0) {
    if (subscriber->valueListeners.size() >= kMaxListeners) {
      ERROR("reached maximum number of listeners, not adding listener");
      return;
    }

    m_listenerStorage.Activate(
        listenerHandle, eventMask & (NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE),
        [subentryHandle = subscriber->handle.GetHandle()](unsigned int mask,
                                                          Event* event) {
          if (auto valueData = event->GetValueEventData()) {
            valueData->subentry = subentryHandle;
          }
          return true;
        });

    subscriber->valueListeners.Add(listenerHandle);

    // handle immediate value
    if ((eventMask & NT_EVENT_VALUE_ALL) != 0 &&
        (eventMask & NT_EVENT_IMMEDIATE) != 0) {
      for (auto&& topic : topics) {
        if (topic->lastValue) {
          m_listenerStorage.Notify(
              {&listenerHandle, 1}, NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE,
              topic->handle, subscriber->handle, topic->lastValue);
        }
      }
    }
  }
}

void LSImpl::AddListener(NT_Listener listenerHandle,
                         std::span<const std::string_view> prefixes,
                         unsigned int eventMask) {
  if (m_multiSubscribers.size() >= kMaxMultiSubscribers) {
    ERROR("reached maximum number of multi-subscribers, not adding listener");
    return;
  }
  // subscribe to make sure topic updates are received
  PubSubOptions options;
  options.topicsOnly = (eventMask & NT_EVENT_VALUE_ALL) == 0;
  options.prefixMatch = true;
  auto sub = AddMultiSubscriber(prefixes, options);
  AddListenerImpl(listenerHandle, sub, eventMask, true);
}

void LSImpl::AddListener(NT_Listener listenerHandle, NT_Handle handle,
                         unsigned int mask) {
  if (auto topic = m_topics.Get(handle)) {
    AddListenerImpl(listenerHandle, topic, mask);
  } else if (auto sub = m_multiSubscribers.Get(handle)) {
    AddListenerImpl(listenerHandle, sub, mask, false);
  } else if (auto sub = m_subscribers.Get(handle)) {
    AddListenerImpl(listenerHandle, sub, mask, sub->handle, false);
  } else if (auto entry = m_entries.Get(handle)) {
    AddListenerImpl(listenerHandle, entry->subscriber, mask, entry->handle,
                    false);
  }
}

void LSImpl::RemoveListener(NT_Listener listenerHandle, unsigned int mask) {
  auto listenerIt = m_listeners.find(listenerHandle);
  if (listenerIt == m_listeners.end()) {
    return;
  }
  auto listener = std::move(listenerIt->getSecond());
  m_listeners.erase(listenerIt);
  if (!listener) {
    return;
  }

  m_topicPrefixListeners.Remove(listener.get());
  if (listener->subscriber) {
    listener->subscriber->valueListeners.Remove(listenerHandle);
    listener->subscriber->topic->listeners.Remove(listenerHandle);
    if (listener->subscriberOwned) {
      RemoveLocalSubscriber(listener->subscriber->handle);
    }
  }
  if (listener->multiSubscriber) {
    listener->multiSubscriber->valueListeners.Remove(listenerHandle);
    if (listener->subscriberOwned) {
      RemoveMultiSubscriber(listener->multiSubscriber->handle);
    }
  }
}

TopicData* LSImpl::GetOrCreateTopic(std::string_view name) {
  auto& topic = m_nameTopics[name];
  // create if it does not already exist
  if (!topic) {
    topic = m_topics.Add(m_inst, name);
    // attach multi-subscribers
    for (auto&& sub : m_multiSubscribers) {
      if (sub->Matches(name, topic->special)) {
        topic->multiSubscribers.Add(sub.get());
      }
    }
  }
  return topic;
}

TopicData* LSImpl::GetTopic(NT_Handle handle) {
  switch (Handle{handle}.GetType()) {
    case Handle::kEntry: {
      if (auto entry = m_entries.Get(handle)) {
        return entry->topic;
      }
      break;
    }
    case Handle::kSubscriber: {
      if (auto subscriber = m_subscribers.Get(handle)) {
        return subscriber->topic;
      }
      break;
    }
    case Handle::kPublisher: {
      if (auto publisher = m_publishers.Get(handle)) {
        return publisher->topic;
      }
      break;
    }
    case Handle::kTopic:
      return m_topics.Get(handle);
    default:
      break;
  }
  return {};
}

SubscriberData* LSImpl::GetSubEntry(NT_Handle subentryHandle) {
  Handle h{subentryHandle};
  if (h.IsType(Handle::kSubscriber)) {
    return m_subscribers.Get(subentryHandle);
  } else if (h.IsType(Handle::kEntry)) {
    auto entry = m_entries.Get(subentryHandle);
    return entry ? entry->subscriber : nullptr;
  } else {
    return nullptr;
  }
}

PublisherData* LSImpl::PublishEntry(EntryData* entry, NT_Type type) {
  if (entry->publisher) {
    return entry->publisher;
  }
  auto typeStr = TypeToString(type);
  if (entry->subscriber->config.type == NT_UNASSIGNED) {
    entry->subscriber->config.type = type;
    entry->subscriber->config.typeStr = typeStr;
  } else if (entry->subscriber->config.type != type ||
             entry->subscriber->config.typeStr != typeStr) {
    if (!IsNumericCompatible(type, entry->subscriber->config.type)) {
      // don't allow dynamically changing the type of an entry
      ERROR("cannot publish entry {} as type {}, previously subscribed as {}",
            entry->topic->name, typeStr, entry->subscriber->config.typeStr);
      return nullptr;
    }
  }
  // create publisher
  entry->publisher = AddLocalPublisher(entry->topic, wpi::json::object(),
                                       entry->subscriber->config);
  return entry->publisher;
}

Value* LSImpl::GetSubEntryValue(NT_Handle subentryHandle) {
  if (auto subscriber = GetSubEntry(subentryHandle)) {
    return &subscriber->topic->lastValue;
  } else {
    return nullptr;
  }
}

bool LSImpl::PublishLocalValue(PublisherData* publisher, const Value& value,
                               bool force) {
  if (!value) {
    return false;
  }
  if (publisher->topic->type != NT_UNASSIGNED &&
      publisher->topic->type != value.type()) {
    if (IsNumericCompatible(publisher->topic->type, value.type())) {
      return PublishLocalValue(
          publisher, ConvertNumericValue(value, publisher->topic->type));
    }
    return false;
  }
  if (publisher->active) {
    bool isDuplicate, isNetworkDuplicate;
    if (force || publisher->config.keepDuplicates) {
      isDuplicate = false;
      isNetworkDuplicate = false;
    } else {
      isDuplicate = (publisher->topic->lastValue == value);
      isNetworkDuplicate = (publisher->topic->lastValueNetwork == value);
    }
    if (!isNetworkDuplicate && m_network) {
      publisher->topic->lastValueNetwork = value;
      m_network->SetValue(publisher->handle, value);
    }
    return SetValue(publisher->topic, value, NT_EVENT_VALUE_LOCAL, isDuplicate);
  } else {
    return false;
  }
}

bool LSImpl::SetEntryValue(NT_Handle pubentryHandle, const Value& value) {
  if (!value) {
    return false;
  }
  auto publisher = m_publishers.Get(pubentryHandle);
  if (!publisher) {
    if (auto entry = m_entries.Get(pubentryHandle)) {
      publisher = PublishEntry(entry, value.type());
    }
    if (!publisher) {
      return false;
    }
  }
  return PublishLocalValue(publisher, value);
}

bool LSImpl::SetDefaultEntryValue(NT_Handle pubsubentryHandle,
                                  const Value& value) {
  DEBUG4("SetDefaultEntryValue({}, {})", pubsubentryHandle,
         static_cast<int>(value.type()));
  if (!value) {
    return false;
  }
  if (auto topic = GetTopic(pubsubentryHandle)) {
    if (!topic->lastValue &&
        (topic->type == NT_UNASSIGNED || topic->type == value.type() ||
         IsNumericCompatible(topic->type, value.type()))) {
      // publish if we haven't yet
      auto publisher = m_publishers.Get(pubsubentryHandle);
      if (!publisher) {
        if (auto entry = m_entries.Get(pubsubentryHandle)) {
          publisher = PublishEntry(entry, value.type());
        }
      }

      // force value timestamps to 0
      if (topic->type == NT_UNASSIGNED) {
        topic->type = value.type();
      }
      if (topic->type == value.type()) {
        topic->lastValue = value;
      } else if (IsNumericCompatible(topic->type, value.type())) {
        topic->lastValue = ConvertNumericValue(value, topic->type);
      } else {
        return true;
      }
      topic->lastValue.SetTime(0);
      topic->lastValue.SetServerTime(0);
      if (publisher) {
        PublishLocalValue(publisher, topic->lastValue, true);
      }
      return true;
    }
  }
  return false;
}

void LSImpl::RemoveSubEntry(NT_Handle subentryHandle) {
  Handle h{subentryHandle};
  if (h.IsType(Handle::kSubscriber)) {
    RemoveLocalSubscriber(subentryHandle);
  } else if (h.IsType(Handle::kMultiSubscriber)) {
    RemoveMultiSubscriber(subentryHandle);
  } else if (h.IsType(Handle::kEntry)) {
    if (auto entry = RemoveEntry(subentryHandle)) {
      RemoveLocalSubscriber(entry->subscriber->handle);
      if (entry->publisher) {
        RemoveLocalPublisher(entry->publisher->handle);
      }
    }
  }
}

class LocalStorage::Impl : public LSImpl {
 public:
  Impl(int inst, IListenerStorage& listenerStorage, wpi::Logger& logger)
      : LSImpl{inst, listenerStorage, logger} {}
};

LocalStorage::LocalStorage(int inst, IListenerStorage& listenerStorage,
                           wpi::Logger& logger)
    : m_impl{std::make_unique<Impl>(inst, listenerStorage, logger)} {}

LocalStorage::~LocalStorage() = default;

NT_Topic LocalStorage::NetworkAnnounce(std::string_view name,
                                       std::string_view typeStr,
                                       const wpi::json& properties,
                                       NT_Publisher pubHandle) {
  std::scoped_lock lock{m_mutex};
  auto topic = m_impl->GetOrCreateTopic(name);
  m_impl->NetworkAnnounce(topic, typeStr, properties, pubHandle);
  return topic->handle;
}

void LocalStorage::NetworkUnannounce(std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto topic = m_impl->GetOrCreateTopic(name);
  m_impl->RemoveNetworkPublisher(topic);
}

void LocalStorage::NetworkPropertiesUpdate(std::string_view name,
                                           const wpi::json& update, bool ack) {
  std::scoped_lock lock{m_mutex};
  auto it = m_impl->m_nameTopics.find(name);
  if (it != m_impl->m_nameTopics.end()) {
    m_impl->NetworkPropertiesUpdate(it->second, update, ack);
  }
}

void LocalStorage::NetworkSetValue(NT_Topic topicHandle, const Value& value) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    if (m_impl->SetValue(topic, value, NT_EVENT_VALUE_REMOTE,
                         value == topic->lastValue)) {
      topic->lastValueNetwork = value;
    }
  }
}

void LocalStorage::StartNetwork(net::NetworkStartupInterface& startup,
                                net::NetworkInterface* network) {
  std::scoped_lock lock{m_mutex};
  // publish all active publishers to the network and send last values
  // only send value once per topic
  for (auto&& topic : m_impl->m_topics) {
    PublisherData* anyPublisher = nullptr;
    for (auto&& publisher : topic->localPublishers) {
      if (publisher->active) {
        startup.Publish(publisher->handle, topic->handle, topic->name,
                        topic->typeStr, topic->properties, publisher->config);
        anyPublisher = publisher;
      }
    }
    if (anyPublisher && topic->lastValue) {
      startup.SetValue(anyPublisher->handle, topic->lastValue);
    }
  }
  for (auto&& subscriber : m_impl->m_subscribers) {
    startup.Subscribe(subscriber->handle, {{subscriber->topic->name}},
                      subscriber->config);
  }
  for (auto&& subscriber : m_impl->m_multiSubscribers) {
    startup.Subscribe(subscriber->handle, subscriber->prefixes,
                      subscriber->options);
  }
  m_impl->m_network = network;
}

void LocalStorage::ClearNetwork() {
  std::scoped_lock lock{m_mutex};
  m_impl->m_network = nullptr;
  // treat as an unannounce all from the network side
  for (auto&& topic : m_impl->m_topics) {
    m_impl->RemoveNetworkPublisher(topic.get());
  }
}

template <typename T, typename F>
static void ForEachTopic(T& topics, std::string_view prefix, unsigned int types,
                         F func) {
  for (auto&& topic : topics) {
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

template <typename T, typename F>
static void ForEachTopic(T& topics, std::string_view prefix,
                         std::span<const std::string_view> types, F func) {
  for (auto&& topic : topics) {
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

std::vector<NT_Topic> LocalStorage::GetTopics(std::string_view prefix,
                                              unsigned int types) {
  std::scoped_lock lock(m_mutex);
  std::vector<NT_Topic> rv;
  ForEachTopic(m_impl->m_topics, prefix, types,
               [&](TopicData& topic) { rv.push_back(topic.handle); });
  return rv;
}

std::vector<NT_Topic> LocalStorage::GetTopics(
    std::string_view prefix, std::span<const std::string_view> types) {
  std::scoped_lock lock(m_mutex);
  std::vector<NT_Topic> rv;
  ForEachTopic(m_impl->m_topics, prefix, types,
               [&](TopicData& topic) { rv.push_back(topic.handle); });
  return rv;
}

std::vector<TopicInfo> LocalStorage::GetTopicInfo(std::string_view prefix,
                                                  unsigned int types) {
  std::scoped_lock lock(m_mutex);
  std::vector<TopicInfo> rv;
  ForEachTopic(m_impl->m_topics, prefix, types, [&](TopicData& topic) {
    rv.emplace_back(topic.GetTopicInfo());
  });
  return rv;
}

std::vector<TopicInfo> LocalStorage::GetTopicInfo(
    std::string_view prefix, std::span<const std::string_view> types) {
  std::scoped_lock lock(m_mutex);
  std::vector<TopicInfo> rv;
  ForEachTopic(m_impl->m_topics, prefix, types, [&](TopicData& topic) {
    rv.emplace_back(topic.GetTopicInfo());
  });
  return rv;
}

NT_Topic LocalStorage::GetTopic(std::string_view name) {
  if (name.empty()) {
    return {};
  }
  std::scoped_lock lock{m_mutex};
  return m_impl->GetOrCreateTopic(name)->handle;
}

std::string LocalStorage::GetTopicName(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return topic->name;
  } else {
    return {};
  }
}

NT_Type LocalStorage::GetTopicType(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return topic->type;
  } else {
    return {};
  }
}

std::string LocalStorage::GetTopicTypeString(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return topic->typeStr;
  } else {
    return {};
  }
}

void LocalStorage::SetTopicPersistent(NT_Topic topicHandle, bool value) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    m_impl->SetPersistent(topic, value);
  }
}

bool LocalStorage::GetTopicPersistent(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return (topic->flags & NT_PERSISTENT) != 0;
  } else {
    return false;
  }
}

void LocalStorage::SetTopicRetained(NT_Topic topicHandle, bool value) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    m_impl->SetRetained(topic, value);
  }
}

bool LocalStorage::GetTopicRetained(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return (topic->flags & NT_RETAINED) != 0;
  } else {
    return false;
  }
}

bool LocalStorage::GetTopicExists(NT_Handle handle) {
  std::scoped_lock lock{m_mutex};
  TopicData* topic = m_impl->GetTopic(handle);
  return topic && topic->Exists();
}

wpi::json LocalStorage::GetTopicProperty(NT_Topic topicHandle,
                                         std::string_view name) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return topic->properties.value(name, wpi::json{});
  } else {
    return {};
  }
}

void LocalStorage::SetTopicProperty(NT_Topic topicHandle, std::string_view name,
                                    const wpi::json& value) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    if (value.is_null()) {
      topic->properties.erase(name);
    } else {
      topic->properties[name] = value;
    }
    wpi::json update = wpi::json::object();
    update[name] = value;
    m_impl->PropertiesUpdated(topic, update, NT_EVENT_NONE, true);
  }
}

void LocalStorage::DeleteTopicProperty(NT_Topic topicHandle,
                                       std::string_view name) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    topic->properties.erase(name);
    wpi::json update = wpi::json::object();
    update[name] = wpi::json();
    m_impl->PropertiesUpdated(topic, update, NT_EVENT_NONE, true);
  }
}

wpi::json LocalStorage::GetTopicProperties(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return topic->properties;
  } else {
    return wpi::json::object();
  }
}

bool LocalStorage::SetTopicProperties(NT_Topic topicHandle,
                                      const wpi::json& update) {
  if (!update.is_object()) {
    return false;
  }
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    m_impl->SetProperties(topic, update, true);
    return true;
  } else {
    return {};
  }
}

TopicInfo LocalStorage::GetTopicInfo(NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->m_topics.Get(topicHandle)) {
    return topic->GetTopicInfo();
  } else {
    return {};
  }
}

NT_Subscriber LocalStorage::Subscribe(NT_Topic topicHandle, NT_Type type,
                                      std::string_view typeStr,
                                      std::span<const PubSubOption> options) {
  std::scoped_lock lock{m_mutex};

  // Get the topic
  auto* topic = m_impl->m_topics.Get(topicHandle);
  if (!topic) {
    return 0;
  }

  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    WPI_ERROR(m_impl->m_logger,
              "reached maximum number of subscribers to '{}', not subscribing",
              topic->name);
    return 0;
  }

  // Create subscriber
  return m_impl->AddLocalSubscriber(topic, PubSubConfig{type, typeStr, options})
      ->handle;
}

void LocalStorage::Unsubscribe(NT_Subscriber subHandle) {
  std::scoped_lock lock{m_mutex};
  m_impl->RemoveSubEntry(subHandle);
}

NT_MultiSubscriber LocalStorage::SubscribeMultiple(
    std::span<const std::string_view> prefixes,
    std::span<const PubSubOption> options) {
  std::scoped_lock lock{m_mutex};

  if (m_impl->m_multiSubscribers.size() >= kMaxMultiSubscribers) {
    WPI_ERROR(m_impl->m_logger,
              "reached maximum number of multi-subscribers, not subscribing");
    return 0;
  }

  PubSubOptions opts{options};
  opts.prefixMatch = true;
  return m_impl->AddMultiSubscriber(prefixes, opts)->handle;
}

void LocalStorage::UnsubscribeMultiple(NT_MultiSubscriber subHandle) {
  std::scoped_lock lock{m_mutex};
  m_impl->RemoveMultiSubscriber(subHandle);
}

NT_Publisher LocalStorage::Publish(NT_Topic topicHandle, NT_Type type,
                                   std::string_view typeStr,
                                   const wpi::json& properties,
                                   std::span<const PubSubOption> options) {
  std::scoped_lock lock{m_mutex};

  // Get the topic
  auto* topic = m_impl->m_topics.Get(topicHandle);
  if (!topic) {
    WPI_ERROR(m_impl->m_logger, "trying to publish invalid topic handle ({})",
              topicHandle);
    return 0;
  }

  if (type == NT_UNASSIGNED || typeStr.empty()) {
    WPI_ERROR(
        m_impl->m_logger,
        "cannot publish '{}' with an unassigned type or empty type string",
        topic->name);
    return 0;
  }

  if (topic->localPublishers.size() >= kMaxPublishers) {
    WPI_ERROR(m_impl->m_logger,
              "reached maximum number of publishers to '{}', not publishing",
              topic->name);
    return 0;
  }

  return m_impl
      ->AddLocalPublisher(topic, properties,
                          PubSubConfig{type, typeStr, options})
      ->handle;
}

void LocalStorage::Unpublish(NT_Handle pubentryHandle) {
  std::scoped_lock lock{m_mutex};

  if (Handle{pubentryHandle}.IsType(Handle::kPublisher)) {
    m_impl->RemoveLocalPublisher(pubentryHandle);
  } else if (auto entry = m_impl->m_entries.Get(pubentryHandle)) {
    if (entry->publisher) {
      m_impl->RemoveLocalPublisher(entry->publisher->handle);
      entry->publisher = nullptr;
    }
  } else {
    // TODO: report warning
    return;
  }
}

NT_Entry LocalStorage::GetEntry(NT_Topic topicHandle, NT_Type type,
                                std::string_view typeStr,
                                std::span<const PubSubOption> options) {
  std::scoped_lock lock{m_mutex};

  // Get the topic
  auto* topic = m_impl->m_topics.Get(topicHandle);
  if (!topic) {
    return 0;
  }

  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    WPI_ERROR(
        m_impl->m_logger,
        "reached maximum number of subscribers to '{}', not creating entry",
        topic->name);
    return 0;
  }

  // Create subscriber
  auto subscriber =
      m_impl->AddLocalSubscriber(topic, PubSubConfig{type, typeStr, options});

  // Create entry
  return m_impl->AddEntry(subscriber)->handle;
}

void LocalStorage::ReleaseEntry(NT_Entry entryHandle) {
  std::scoped_lock lock{m_mutex};
  m_impl->RemoveSubEntry(entryHandle);
}

void LocalStorage::Release(NT_Handle pubsubentryHandle) {
  switch (Handle{pubsubentryHandle}.GetType()) {
    case Handle::kEntry:
      ReleaseEntry(pubsubentryHandle);
      break;
    case Handle::kPublisher:
      Unpublish(pubsubentryHandle);
      break;
    case Handle::kSubscriber:
      Unsubscribe(pubsubentryHandle);
      break;
    case Handle::kMultiSubscriber:
      UnsubscribeMultiple(pubsubentryHandle);
      break;
    default:
      break;
  }
}

NT_Topic LocalStorage::GetTopicFromHandle(NT_Handle pubsubentryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl->GetTopic(pubsubentryHandle)) {
    return topic->handle;
  } else {
    return {};
  }
}

bool LocalStorage::SetEntryValue(NT_Handle pubentryHandle, const Value& value) {
  std::scoped_lock lock{m_mutex};
  return m_impl->SetEntryValue(pubentryHandle, value);
}

bool LocalStorage::SetDefaultEntryValue(NT_Handle pubsubentryHandle,
                                        const Value& value) {
  std::scoped_lock lock{m_mutex};
  return m_impl->SetDefaultEntryValue(pubsubentryHandle, value);
}

TimestampedBoolean LocalStorage::GetAtomicBoolean(NT_Handle subentryHandle,
                                                  bool defaultValue) {
  std::scoped_lock lock{m_mutex};
  Value* value = m_impl->GetSubEntryValue(subentryHandle);
  if (value && value->type() == NT_BOOLEAN) {
    return {value->time(), value->server_time(), value->GetBoolean()};
  } else {
    return {0, 0, defaultValue};
  }
}

TimestampedString LocalStorage::GetAtomicString(NT_Handle subentryHandle,
                                                std::string_view defaultValue) {
  std::scoped_lock lock{m_mutex};
  Value* value = m_impl->GetSubEntryValue(subentryHandle);
  if (value && value->type() == NT_STRING) {
    return {value->time(), value->server_time(),
            std::string{value->GetString()}};
  } else {
    return {0, 0, std::string{defaultValue}};
  }
}

TimestampedStringView LocalStorage::GetAtomicString(
    NT_Handle subentryHandle, wpi::SmallVectorImpl<char>& buf,
    std::string_view defaultValue) {
  std::scoped_lock lock{m_mutex};
  Value* value = m_impl->GetSubEntryValue(subentryHandle);
  if (value && value->type() == NT_STRING) {
    auto str = value->GetString();
    buf.assign(str.begin(), str.end());
    return {value->time(), value->server_time(), {buf.data(), buf.size()}};
  } else {
    return {0, 0, defaultValue};
  }
}

template <typename T, typename U>
static T GetAtomicNumber(Value* value, U defaultValue) {
  if (value && value->type() == NT_INTEGER) {
    return {value->time(), value->server_time(),
            static_cast<U>(value->GetInteger())};
  } else if (value && value->type() == NT_FLOAT) {
    return {value->time(), value->server_time(),
            static_cast<U>(value->GetFloat())};
  } else if (value && value->type() == NT_DOUBLE) {
    return {value->time(), value->server_time(),
            static_cast<U>(value->GetDouble())};
  } else {
    return {0, 0, defaultValue};
  }
}

template <typename T, typename U>
static T GetAtomicNumberArray(Value* value, std::span<const U> defaultValue) {
  if (value && value->type() == NT_INTEGER_ARRAY) {
    auto arr = value->GetIntegerArray();
    return {value->time(), value->server_time(), {arr.begin(), arr.end()}};
  } else if (value && value->type() == NT_FLOAT_ARRAY) {
    auto arr = value->GetFloatArray();
    return {value->time(), value->server_time(), {arr.begin(), arr.end()}};
  } else if (value && value->type() == NT_DOUBLE_ARRAY) {
    auto arr = value->GetDoubleArray();
    return {value->time(), value->server_time(), {arr.begin(), arr.end()}};
  } else {
    return {0, 0, {defaultValue.begin(), defaultValue.end()}};
  }
}

template <typename T, typename U>
static T GetAtomicNumberArray(Value* value, wpi::SmallVectorImpl<U>& buf,
                              std::span<const U> defaultValue) {
  if (value && value->type() == NT_INTEGER_ARRAY) {
    auto str = value->GetIntegerArray();
    buf.assign(str.begin(), str.end());
    return {value->time(), value->server_time(), {buf.data(), buf.size()}};
  } else if (value && value->type() == NT_FLOAT_ARRAY) {
    auto str = value->GetFloatArray();
    buf.assign(str.begin(), str.end());
    return {value->time(), value->server_time(), {buf.data(), buf.size()}};
  } else if (value && value->type() == NT_DOUBLE_ARRAY) {
    auto str = value->GetDoubleArray();
    buf.assign(str.begin(), str.end());
    return {value->time(), value->server_time(), {buf.data(), buf.size()}};
  } else {
    buf.assign(defaultValue.begin(), defaultValue.end());
    return {0, 0, {buf.data(), buf.size()}};
  }
}

#define GET_ATOMIC_NUMBER(Name, dtype)                                  \
  Timestamped##Name LocalStorage::GetAtomic##Name(NT_Handle subentry,   \
                                                  dtype defaultValue) { \
    std::scoped_lock lock{m_mutex};                                     \
    return GetAtomicNumber<Timestamped##Name>(                          \
        m_impl->GetSubEntryValue(subentry), defaultValue);              \
  }                                                                     \
                                                                        \
  Timestamped##Name##Array LocalStorage::GetAtomic##Name##Array(        \
      NT_Handle subentry, std::span<const dtype> defaultValue) {        \
    std::scoped_lock lock{m_mutex};                                     \
    return GetAtomicNumberArray<Timestamped##Name##Array>(              \
        m_impl->GetSubEntryValue(subentry), defaultValue);              \
  }                                                                     \
                                                                        \
  Timestamped##Name##ArrayView LocalStorage::GetAtomic##Name##Array(    \
      NT_Handle subentry, wpi::SmallVectorImpl<dtype>& buf,             \
      std::span<const dtype> defaultValue) {                            \
    std::scoped_lock lock{m_mutex};                                     \
    return GetAtomicNumberArray<Timestamped##Name##ArrayView>(          \
        m_impl->GetSubEntryValue(subentry), buf, defaultValue);         \
  }

GET_ATOMIC_NUMBER(Integer, int64_t)
GET_ATOMIC_NUMBER(Float, float)
GET_ATOMIC_NUMBER(Double, double)

#define GET_ATOMIC_ARRAY(Name, dtype)                                         \
  Timestamped##Name LocalStorage::GetAtomic##Name(                            \
      NT_Handle subentry, std::span<const dtype> defaultValue) {              \
    std::scoped_lock lock{m_mutex};                                           \
    Value* value = m_impl->GetSubEntryValue(subentry);                        \
    if (value && value->Is##Name()) {                                         \
      auto arr = value->Get##Name();                                          \
      return {value->time(), value->server_time(), {arr.begin(), arr.end()}}; \
    } else {                                                                  \
      return {0, 0, {defaultValue.begin(), defaultValue.end()}};              \
    }                                                                         \
  }

GET_ATOMIC_ARRAY(Raw, uint8_t)
GET_ATOMIC_ARRAY(BooleanArray, int)
GET_ATOMIC_ARRAY(StringArray, std::string)

#define GET_ATOMIC_SMALL_ARRAY(Name, dtype)                                   \
  Timestamped##Name##View LocalStorage::GetAtomic##Name(                      \
      NT_Handle subentry, wpi::SmallVectorImpl<dtype>& buf,                   \
      std::span<const dtype> defaultValue) {                                  \
    std::scoped_lock lock{m_mutex};                                           \
    Value* value = m_impl->GetSubEntryValue(subentry);                        \
    if (value && value->Is##Name()) {                                         \
      auto str = value->Get##Name();                                          \
      buf.assign(str.begin(), str.end());                                     \
      return {value->time(), value->server_time(), {buf.data(), buf.size()}}; \
    } else {                                                                  \
      buf.assign(defaultValue.begin(), defaultValue.end());                   \
      return {0, 0, {buf.data(), buf.size()}};                                \
    }                                                                         \
  }

GET_ATOMIC_SMALL_ARRAY(Raw, uint8_t)
GET_ATOMIC_SMALL_ARRAY(BooleanArray, int)

std::vector<Value> LocalStorage::ReadQueueValue(NT_Handle subentry) {
  std::scoped_lock lock{m_mutex};
  auto subscriber = m_impl->GetSubEntry(subentry);
  if (!subscriber) {
    return {};
  }
  std::vector<Value> rv;
  rv.reserve(subscriber->pollStorage.size());
  for (auto&& val : subscriber->pollStorage) {
    rv.emplace_back(std::move(val));
  }
  subscriber->pollStorage.reset();
  return rv;
}

std::vector<TimestampedBoolean> LocalStorage::ReadQueueBoolean(
    NT_Handle subentry) {
  std::scoped_lock lock{m_mutex};
  auto subscriber = m_impl->GetSubEntry(subentry);
  if (!subscriber) {
    return {};
  }
  std::vector<TimestampedBoolean> rv;
  rv.reserve(subscriber->pollStorage.size());
  for (auto&& val : subscriber->pollStorage) {
    if (val.IsBoolean()) {
      rv.emplace_back(val.time(), val.server_time(), val.GetBoolean());
    }
  }
  subscriber->pollStorage.reset();
  return rv;
}

std::vector<TimestampedString> LocalStorage::ReadQueueString(
    NT_Handle subentry) {
  std::scoped_lock lock{m_mutex};
  auto subscriber = m_impl->GetSubEntry(subentry);
  if (!subscriber) {
    return {};
  }
  std::vector<TimestampedString> rv;
  rv.reserve(subscriber->pollStorage.size());
  for (auto&& val : subscriber->pollStorage) {
    if (val.IsString()) {
      rv.emplace_back(val.time(), val.server_time(),
                      std::string{val.GetString()});
    }
  }
  subscriber->pollStorage.reset();
  return rv;
}

#define READ_QUEUE_ARRAY(Name)                                         \
  std::vector<Timestamped##Name> LocalStorage::ReadQueue##Name(        \
      NT_Handle subentry) {                                            \
    std::scoped_lock lock{m_mutex};                                    \
    auto subscriber = m_impl->GetSubEntry(subentry);                   \
    if (!subscriber) {                                                 \
      return {};                                                       \
    }                                                                  \
    std::vector<Timestamped##Name> rv;                                 \
    rv.reserve(subscriber->pollStorage.size());                        \
    for (auto&& val : subscriber->pollStorage) {                       \
      if (val.Is##Name()) {                                            \
        auto arr = val.Get##Name();                                    \
        rv.emplace_back(Timestamped##Name{                             \
            val.time(), val.server_time(), {arr.begin(), arr.end()}}); \
      }                                                                \
    }                                                                  \
    subscriber->pollStorage.reset();                                   \
    return rv;                                                         \
  }

READ_QUEUE_ARRAY(Raw)
READ_QUEUE_ARRAY(BooleanArray)
READ_QUEUE_ARRAY(StringArray)

template <typename T>
static std::vector<T> ReadQueueNumber(SubscriberData* subscriber) {
  if (!subscriber) {
    return {};
  }
  std::vector<T> rv;
  rv.reserve(subscriber->pollStorage.size());
  for (auto&& val : subscriber->pollStorage) {
    auto ts = val.time();
    auto sts = val.server_time();
    if (val.IsInteger()) {
      rv.emplace_back(T(ts, sts, val.GetInteger()));
    } else if (val.IsFloat()) {
      rv.emplace_back(T(ts, sts, val.GetFloat()));
    } else if (val.IsDouble()) {
      rv.emplace_back(T(ts, sts, val.GetDouble()));
    }
  }
  subscriber->pollStorage.reset();
  return rv;
}

template <typename T>
static std::vector<T> ReadQueueNumberArray(SubscriberData* subscriber) {
  if (!subscriber) {
    return {};
  }
  std::vector<T> rv;
  rv.reserve(subscriber->pollStorage.size());
  for (auto&& val : subscriber->pollStorage) {
    auto ts = val.time();
    auto sts = val.server_time();
    if (val.IsIntegerArray()) {
      auto arr = val.GetIntegerArray();
      rv.emplace_back(T{ts, sts, {arr.begin(), arr.end()}});
    } else if (val.IsFloatArray()) {
      auto arr = val.GetFloatArray();
      rv.emplace_back(T{ts, sts, {arr.begin(), arr.end()}});
    } else if (val.IsDoubleArray()) {
      auto arr = val.GetDoubleArray();
      rv.emplace_back(T{ts, sts, {arr.begin(), arr.end()}});
    }
  }
  subscriber->pollStorage.reset();
  return rv;
}

#define READ_QUEUE_NUMBER(Name)                                               \
  std::vector<Timestamped##Name> LocalStorage::ReadQueue##Name(               \
      NT_Handle subentry) {                                                   \
    std::scoped_lock lock{m_mutex};                                           \
    return ReadQueueNumber<Timestamped##Name>(m_impl->GetSubEntry(subentry)); \
  }                                                                           \
                                                                              \
  std::vector<Timestamped##Name##Array> LocalStorage::ReadQueue##Name##Array( \
      NT_Handle subentry) {                                                   \
    std::scoped_lock lock{m_mutex};                                           \
    return ReadQueueNumberArray<Timestamped##Name##Array>(                    \
        m_impl->GetSubEntry(subentry));                                       \
  }

READ_QUEUE_NUMBER(Integer)
READ_QUEUE_NUMBER(Float)
READ_QUEUE_NUMBER(Double)

Value LocalStorage::GetEntryValue(NT_Handle subentryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto subscriber = m_impl->GetSubEntry(subentryHandle)) {
    if (subscriber->config.type == NT_UNASSIGNED ||
        !subscriber->topic->lastValue ||
        subscriber->config.type == subscriber->topic->lastValue.type()) {
      return subscriber->topic->lastValue;
    } else if (IsNumericCompatible(subscriber->config.type,
                                   subscriber->topic->lastValue.type())) {
      return ConvertNumericValue(subscriber->topic->lastValue,
                                 subscriber->config.type);
    }
  }
  return {};
}

void LocalStorage::SetEntryFlags(NT_Entry entryHandle, unsigned int flags) {
  std::scoped_lock lock{m_mutex};
  if (auto entry = m_impl->m_entries.Get(entryHandle)) {
    m_impl->SetFlags(entry->subscriber->topic, flags);
  }
}

unsigned int LocalStorage::GetEntryFlags(NT_Entry entryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto entry = m_impl->m_entries.Get(entryHandle)) {
    return entry->subscriber->topic->flags;
  } else {
    return 0;
  }
}

NT_Entry LocalStorage::GetEntry(std::string_view name) {
  if (name.empty()) {
    return {};
  }

  std::scoped_lock lock{m_mutex};

  // Get the topic data
  auto* topic = m_impl->GetOrCreateTopic(name);

  if (topic->entry == 0) {
    if (topic->localSubscribers.size() >= kMaxSubscribers) {
      WPI_ERROR(
          m_impl->m_logger,
          "reached maximum number of subscribers to '{}', not creating entry",
          topic->name);
      return 0;
    }

    // Create subscriber
    auto* subscriber = m_impl->AddLocalSubscriber(topic, {});

    // Create entry
    topic->entry = m_impl->AddEntry(subscriber)->handle;
  }

  return topic->entry;
}

std::string LocalStorage::GetEntryName(NT_Handle subentryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto subscriber = m_impl->GetSubEntry(subentryHandle)) {
    return subscriber->topic->name;
  } else {
    return {};
  }
}

NT_Type LocalStorage::GetEntryType(NT_Handle subentryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto subscriber = m_impl->GetSubEntry(subentryHandle)) {
    return subscriber->topic->type;
  } else {
    return {};
  }
}

int64_t LocalStorage::GetEntryLastChange(NT_Handle subentryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto subscriber = m_impl->GetSubEntry(subentryHandle)) {
    return subscriber->topic->lastValue.time();
  } else {
    return 0;
  }
}

void LocalStorage::AddListener(NT_Listener listener,
                               std::span<const std::string_view> prefixes,
                               unsigned int mask) {
  mask &= (NT_EVENT_TOPIC | NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE);
  std::scoped_lock lock{m_mutex};
  m_impl->AddListener(listener, prefixes, mask);
}

void LocalStorage::AddListener(NT_Listener listener, NT_Handle handle,
                               unsigned int mask) {
  mask &= (NT_EVENT_TOPIC | NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE);
  std::scoped_lock lock{m_mutex};
  m_impl->AddListener(listener, handle, mask);
}

void LocalStorage::RemoveListener(NT_Listener listener, unsigned int mask) {
  std::scoped_lock lock{m_mutex};
  m_impl->RemoveListener(listener, mask);
}

NT_DataLogger LocalStorage::StartDataLog(wpi::log::DataLog& log,
                                         std::string_view prefix,
                                         std::string_view logPrefix) {
  std::scoped_lock lock{m_mutex};
  auto datalogger =
      m_impl->m_dataloggers.Add(m_impl->m_inst, log, prefix, logPrefix);

  // start logging any matching topics
  auto now = nt::Now();
  for (auto&& topic : m_impl->m_topics) {
    if (!wpi::starts_with(topic->name, prefix) ||
        topic->type == NT_UNASSIGNED || topic->typeStr.empty()) {
      continue;
    }
    topic->datalogs.emplace_back(log, datalogger->Start(topic.get(), now),
                                 datalogger->handle);

    // log current value, if any
    if (!topic->lastValue) {
      continue;
    }
    topic->datalogType = topic->type;
    topic->datalogs.back().Append(topic->lastValue);
  }

  return datalogger->handle;
}

void LocalStorage::StopDataLog(NT_DataLogger logger) {
  std::scoped_lock lock{m_mutex};
  if (auto datalogger = m_impl->m_dataloggers.Remove(logger)) {
    // finish any active entries
    auto now = Now();
    for (auto&& topic : m_impl->m_topics) {
      auto it =
          std::find_if(topic->datalogs.begin(), topic->datalogs.end(),
                       [&](const auto& elem) { return elem.logger == logger; });
      if (it != topic->datalogs.end()) {
        it->log->Finish(it->entry, now);
        topic->datalogs.erase(it);
      }
    }
  }
}

void LocalStorage::Reset() {
  std::scoped_lock lock{m_mutex};
  m_impl = std::make_unique<Impl>(m_impl->m_inst, m_impl->m_listenerStorage,
                                  m_impl->m_logger);
}
