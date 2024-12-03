// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalStorageImpl.h"

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <wpi/DataLog.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>

#include "IListenerStorage.h"
#include "Log.h"
#include "net/MessageHandler.h"

using namespace nt;
using namespace nt::local;

// maximum number of local publishers / subscribers to any given topic
static constexpr size_t kMaxPublishers = 512;
static constexpr size_t kMaxSubscribers = 512;
static constexpr size_t kMaxMultiSubscribers = 512;
static constexpr size_t kMaxListeners = 512;

StorageImpl::StorageImpl(int inst, IListenerStorage& listenerStorage,
                         wpi::Logger& logger)
    : m_inst{inst}, m_listenerStorage{listenerStorage}, m_logger{logger} {}

//
// Network interface functions
//

void StorageImpl::NetworkAnnounce(LocalTopic* topic, std::string_view typeStr,
                                  const wpi::json& properties,
                                  std::optional<int> pubuid) {
  DEBUG4("LS NetworkAnnounce({}, {}, {}, {})", topic->name, typeStr,
         properties.dump(), pubuid.value_or(-1));
  if (pubuid.has_value()) {
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
  wpi::json update = topic->CompareProperties(properties);
  if (!update.empty()) {
    topic->properties = properties;
    PropertiesUpdated(topic, update, event, false);
  } else if (event != NT_EVENT_NONE) {
    NotifyTopic(topic, event);
  }
}

void StorageImpl::RemoveNetworkPublisher(LocalTopic* topic) {
  DEBUG4("LS RemoveNetworkPublisher({}, {})", topic->handle.GetHandle(),
         topic->name);
  // this acts as an unpublish
  bool didExist = topic->Exists();
  topic->onNetwork = false;
  if (didExist && !topic->Exists()) {
    DEBUG4("Unpublished {}", topic->name);
    topic->ResetIfDoesNotExist();
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
        m_network->ClientPublish(Handle{nextPub->handle}.GetIndex(),
                                 topic->name, topic->typeStr, topic->properties,
                                 nextPub->config);
      }
    }
  }
}

void StorageImpl::NetworkPropertiesUpdate(LocalTopic* topic,
                                          const wpi::json& update, bool ack) {
  DEBUG4("NetworkPropertiesUpdate({},{})", topic->name, ack);
  if (ack) {
    return;  // ignore acks
  }
  SetProperties(topic, update, false);
}

void StorageImpl::StartNetwork(net::ClientMessageHandler* network) {
  DEBUG4("StartNetwork()");
  m_network = network;
  // publish all active publishers to the network and send last values
  // only send value once per topic
  for (auto&& topic : m_topics) {
    LocalPublisher* anyPublisher = nullptr;
    for (auto&& publisher : topic->localPublishers) {
      if (publisher->active) {
        network->ClientPublish(Handle{publisher->handle}.GetIndex(),
                               topic->name, topic->typeStr, topic->properties,
                               publisher->config);
        anyPublisher = publisher;
      }
    }
    if (anyPublisher && topic->lastValue) {
      network->ClientSetValue(Handle{anyPublisher->handle}.GetIndex(),
                              topic->lastValue);
    }
  }
  for (auto&& subscriber : m_subscribers) {
    if (!subscriber->config.hidden) {
      network->ClientSubscribe(1 + Handle{subscriber->handle}.GetIndex(),
                               {{subscriber->topic->name}}, subscriber->config);
    }
  }
  for (auto&& subscriber : m_multiSubscribers) {
    if (!subscriber->options.hidden) {
      network->ClientSubscribe(-1 - Handle{subscriber->handle}.GetIndex(),
                               subscriber->prefixes, subscriber->options);
    }
  }
}

void StorageImpl::ClearNetwork() {
  DEBUG4("ClearNetwork()");
  m_network = nullptr;
  // treat as an unannounce all from the network side
  for (auto&& topic : m_topics) {
    RemoveNetworkPublisher(topic.get());
  }
}

//
// Topic functions
//

LocalTopic* StorageImpl::GetOrCreateTopic(std::string_view name) {
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

//
// Topic property functions
//

void StorageImpl::SetFlags(LocalTopic* topic, unsigned int flags) {
  wpi::json update = topic->SetFlags(flags);
  if ((flags & NT_UNCACHED) != 0 && (flags & NT_PERSISTENT) != 0) {
    WARN("topic {}: disabling cached property disables persistent storage",
         topic->name);
  }
  if (!update.empty()) {
    PropertiesUpdated(topic, update, NT_EVENT_NONE, true, false);
  }
}

void StorageImpl::SetPersistent(LocalTopic* topic, bool value) {
  PropertiesUpdated(topic, topic->SetPersistent(value), NT_EVENT_NONE, true,
                    false);
}

void StorageImpl::SetRetained(LocalTopic* topic, bool value) {
  PropertiesUpdated(topic, topic->SetRetained(value), NT_EVENT_NONE, true,
                    false);
}

void StorageImpl::SetCached(LocalTopic* topic, bool value) {
  PropertiesUpdated(topic, topic->SetCached(value), NT_EVENT_NONE, true, false);
}

void StorageImpl::SetProperty(LocalTopic* topic, std::string_view name,
                              const wpi::json& value) {
  PropertiesUpdated(topic, topic->SetProperty(name, value), NT_EVENT_NONE,
                    true);
}

bool StorageImpl::SetProperties(LocalTopic* topic, const wpi::json& update,
                                bool sendNetwork) {
  DEBUG4("SetProperties({},{})", topic->name, sendNetwork);
  if (!topic->SetProperties(update)) {
    return false;
  }
  PropertiesUpdated(topic, update, NT_EVENT_NONE, sendNetwork);
  return true;
}

void StorageImpl::DeleteProperty(LocalTopic* topic, std::string_view name) {
  PropertiesUpdated(topic, topic->DeleteProperty(name), NT_EVENT_NONE, true);
}

//
// Value functions
//

bool StorageImpl::SetEntryValue(NT_Handle pubentryHandle, const Value& value) {
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

bool StorageImpl::SetDefaultEntryValue(NT_Handle pubsubentryHandle,
                                       const Value& value) {
  DEBUG4("SetDefaultEntryValue({}, {})", pubsubentryHandle,
         static_cast<int>(value.type()));
  if (!value) {
    return false;
  }
  if (auto topic = GetTopic(pubsubentryHandle)) {
    if (!topic->IsCached()) {
      WARN("ignoring default value on non-cached topic '{}'", topic->name);
      return false;
    }
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
      Value newValue;
      if (topic->type == value.type()) {
        newValue = value;
      } else if (IsNumericCompatible(topic->type, value.type())) {
        newValue = ConvertNumericValue(value, topic->type);
      } else {
        return true;
      }
      newValue.SetTime(0);
      newValue.SetServerTime(0);
      if (publisher) {
        PublishLocalValue(publisher, newValue, true);
      } else {
        topic->lastValue = newValue;
      }
      return true;
    }
  }
  return false;
}

//
// Publish/Subscribe/Entry functions
//

LocalSubscriber* StorageImpl::Subscribe(LocalTopic* topic, NT_Type type,
                                        std::string_view typeStr,
                                        const PubSubOptions& options) {
  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    ERR("reached maximum number of subscribers to '{}', not subscribing",
        topic->name);
    return nullptr;
  }

  // Create subscriber
  return AddLocalSubscriber(topic, PubSubConfig{type, typeStr, options});
}

LocalPublisher* StorageImpl::Publish(LocalTopic* topic, NT_Type type,
                                     std::string_view typeStr,
                                     const wpi::json& properties,
                                     const PubSubOptions& options) {
  if (type == NT_UNASSIGNED || typeStr.empty()) {
    ERR("cannot publish '{}' with an unassigned type or empty type string",
        topic->name);
    return nullptr;
  }

  if (topic->localPublishers.size() >= kMaxPublishers) {
    ERR("reached maximum number of publishers to '{}', not publishing",
        topic->name);
    return nullptr;
  }

  return AddLocalPublisher(topic, properties,
                           PubSubConfig{type, typeStr, options});
}

LocalEntry* StorageImpl::GetEntry(LocalTopic* topic, NT_Type type,
                                  std::string_view typeStr,
                                  const PubSubOptions& options) {
  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    ERR("reached maximum number of subscribers to '{}', not creating entry",
        topic->name);
    return nullptr;
  }

  // Create subscriber
  auto subscriber =
      AddLocalSubscriber(topic, PubSubConfig{type, typeStr, options});

  // Create entry
  return AddEntry(subscriber);
}

LocalEntry* StorageImpl::GetEntry(std::string_view name) {
  if (name.empty()) {
    return nullptr;
  }

  // Get the topic data
  auto* topic = GetOrCreateTopic(name);

  if (!topic->entry) {
    if (topic->localSubscribers.size() >= kMaxSubscribers) {
      ERR("reached maximum number of subscribers to '{}', not creating entry",
          topic->name);
      return nullptr;
    }

    // Create subscriber
    auto* subscriber = AddLocalSubscriber(topic, {});

    // Create entry
    topic->entry = AddEntry(subscriber);
  }

  return topic->entry;
}

void StorageImpl::RemoveSubEntry(NT_Handle subentryHandle) {
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

void StorageImpl::Unpublish(NT_Handle pubentryHandle) {
  if (Handle{pubentryHandle}.IsType(Handle::kPublisher)) {
    RemoveLocalPublisher(pubentryHandle);
  } else if (auto entry = GetEntryByHandle(pubentryHandle)) {
    if (entry->publisher) {
      RemoveLocalPublisher(entry->publisher->handle);
      entry->publisher = nullptr;
    }
  } else {
    // TODO: report warning
    return;
  }
}

//
// Multi-subscriber functions
//

LocalMultiSubscriber* StorageImpl::AddMultiSubscriber(
    std::span<const std::string_view> prefixes, const PubSubOptions& options) {
  DEBUG4("AddMultiSubscriber({})", fmt::join(prefixes, ","));
  if (m_multiSubscribers.size() >= kMaxMultiSubscribers) {
    ERR("reached maximum number of multi-subscribers, not subscribing");
    return nullptr;
  }
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
  if (m_network && !subscriber->options.hidden) {
    DEBUG4("-> NetworkSubscribe");
    m_network->ClientSubscribe(-1 - Handle{subscriber->handle}.GetIndex(),
                               subscriber->prefixes, subscriber->options);
  }
  return subscriber;
}

std::unique_ptr<LocalMultiSubscriber> StorageImpl::RemoveMultiSubscriber(
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
    if (m_network && !subscriber->options.hidden) {
      m_network->ClientUnsubscribe(-1 - Handle{subscriber->handle}.GetIndex());
    }
  }
  return subscriber;
}

//
// Lookup functions
//

LocalTopic* StorageImpl::GetTopic(NT_Handle handle) {
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

LocalSubscriber* StorageImpl::GetSubEntry(NT_Handle subentryHandle) {
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

//
// Listener functions
//

void StorageImpl::AddListenerImpl(NT_Listener listenerHandle, LocalTopic* topic,
                                  unsigned int eventMask) {
  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    ERR("reached maximum number of subscribers to '{}', ignoring listener add",
        topic->name);
    return;
  }
  // subscribe to make sure topic updates are received
  PubSubConfig config;
  config.topicsOnly = (eventMask & NT_EVENT_VALUE_ALL) == 0;
  auto sub = AddLocalSubscriber(topic, config);
  AddListenerImpl(listenerHandle, sub, eventMask, sub->handle, true);
}

void StorageImpl::AddListenerImpl(NT_Listener listenerHandle,
                                  LocalSubscriber* subscriber,
                                  unsigned int eventMask,
                                  NT_Handle subentryHandle,
                                  bool subscriberOwned) {
  m_listeners.try_emplace(listenerHandle, std::make_unique<LocalListener>(
                                              listenerHandle, subscriber,
                                              eventMask, subscriberOwned));

  auto topic = subscriber->topic;

  if ((eventMask & NT_EVENT_TOPIC) != 0) {
    if (topic->listeners.size() >= kMaxListeners) {
      ERR("reached maximum number of listeners to '{}', not adding listener",
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
      ERR("reached maximum number of listeners to '{}', not adding listener",
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

void StorageImpl::AddListenerImpl(NT_Listener listenerHandle,
                                  LocalMultiSubscriber* subscriber,
                                  unsigned int eventMask,
                                  bool subscriberOwned) {
  auto listener =
      m_listeners
          .try_emplace(listenerHandle, std::make_unique<LocalListener>(
                                           listenerHandle, subscriber,
                                           eventMask, subscriberOwned))
          .first->getSecond()
          .get();

  // if we're doing anything immediate, get the list of matching topics
  wpi::SmallVector<LocalTopic*, 32> topics;
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
      ERR("reached maximum number of listeners, not adding listener");
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
      ERR("reached maximum number of listeners, not adding listener");
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

void StorageImpl::RemoveListener(NT_Listener listenerHandle,
                                 unsigned int mask) {
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

//
// Data log functions
//

LocalDataLogger* StorageImpl::StartDataLog(wpi::log::DataLog& log,
                                           std::string_view prefix,
                                           std::string_view logPrefix) {
  auto datalogger = m_dataloggers.Add(m_inst, log, prefix, logPrefix);

  // start logging any matching topics
  auto now = nt::Now();
  for (auto&& topic : m_topics) {
    if (!PrefixMatch(topic->name, prefix, topic->special) ||
        topic->type == NT_UNASSIGNED || topic->typeStr.empty()) {
      continue;
    }
    topic->StartStopDataLog(datalogger, now, true);
    // log current value, if any
    if (topic->lastValue) {
      topic->datalogs.back().Append(topic->lastValue);
    }
  }

  return datalogger;
}

void StorageImpl::StopDataLog(NT_DataLogger logger) {
  if (auto datalogger = m_dataloggers.Remove(logger)) {
    // finish any active entries
    auto now = Now();
    for (auto&& topic : m_topics) {
      topic->StartStopDataLog(datalogger.get(), now, false);
    }
  }
}

//
// Schema functions
//

bool StorageImpl::HasSchema(std::string_view name) {
  wpi::SmallString<128> fullName{"/.schema/"};
  fullName += name;
  auto it = m_schemas.find(fullName);
  return it != m_schemas.end();
}

void StorageImpl::AddSchema(std::string_view name, std::string_view type,
                            std::span<const uint8_t> schema) {
  wpi::SmallString<128> fullName{"/.schema/"};
  fullName += name;
  auto& pubHandle = m_schemas[fullName];
  if (pubHandle != 0) {
    return;
  }

  auto topic = GetOrCreateTopic(fullName);

  if (topic->localPublishers.size() >= kMaxPublishers) {
    ERR("reached maximum number of publishers to '{}', not publishing",
        topic->name);
    return;
  }

  pubHandle = AddLocalPublisher(topic, {{"retained", true}},
                                PubSubConfig{NT_RAW, type, {}})
                  ->handle;

  SetDefaultEntryValue(pubHandle, Value::MakeRaw(schema));
}

void StorageImpl::Reset() {
  m_network = nullptr;
  m_topics.clear();
  m_publishers.clear();
  m_subscribers.clear();
  m_entries.clear();
  m_multiSubscribers.clear();
  m_dataloggers.clear();
  m_nameTopics.clear();
  m_listeners.clear();
  m_topicPrefixListeners.clear();
}

void StorageImpl::NotifyTopic(LocalTopic* topic, unsigned int eventFlags) {
  DEBUG4("NotifyTopic({}, {})", topic->name, eventFlags);
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
        if (PrefixMatch(topic->name, datalogger->prefix, topic->special)) {
          topic->StartStopDataLog(datalogger.get(), now,
                                  (eventFlags & NT_EVENT_PUBLISH) != 0);
        }
      }
    }
  } else if ((eventFlags & NT_EVENT_PROPERTIES) != 0) {
    topic->UpdateDataLogProperties();
  }
}

bool StorageImpl::SetValue(LocalTopic* topic, const Value& value,
                           unsigned int eventFlags, bool suppressIfDuplicate,
                           const LocalPublisher* publisher) {
  const bool isDuplicate = topic->IsCached() && topic->lastValue == value;
  DEBUG4("SetValue({}, {}, {}, {})", topic->name, value.time(), eventFlags,
         isDuplicate);
  if (topic->type != NT_UNASSIGNED && topic->type != value.type()) {
    return false;
  }
  // Make sure value isn't older than last value
  if (!topic->lastValue || topic->lastValue.time() == 0 ||
      value.time() >= topic->lastValue.time()) {
    // TODO: notify option even if older value
    if (!(suppressIfDuplicate && isDuplicate)) {
      topic->type = value.type();
      if (topic->IsCached()) {
        topic->lastValue = value;
        topic->lastValueFromNetwork = false;
      }
      NotifyValue(topic, value, eventFlags, isDuplicate, publisher);
      if (topic->datalogType == value.type()) {
        for (auto&& datalog : topic->datalogs) {
          datalog.Append(value);
        }
      }
    }
  }

  return true;
}

void StorageImpl::NotifyValue(LocalTopic* topic, const Value& value,
                              unsigned int eventFlags, bool isDuplicate,
                              const LocalPublisher* publisher) {
  bool isNetwork = (eventFlags & NT_EVENT_VALUE_REMOTE) != 0;
  for (auto&& subscriber : topic->localSubscribers) {
    if (subscriber->active &&
        (subscriber->config.keepDuplicates || !isDuplicate) &&
        ((isNetwork && !subscriber->config.disableRemote) ||
         (!isNetwork && !subscriber->config.disableLocal)) &&
        (!publisher || (publisher && (subscriber->config.excludePublisher !=
                                      publisher->handle)))) {
      subscriber->pollStorage.emplace_back(value);
      subscriber->handle.Set();
      if (!subscriber->valueListeners.empty()) {
        m_listenerStorage.Notify(subscriber->valueListeners, eventFlags,
                                 topic->handle, 0, value);
      }
    }
  }

  for (auto&& subscriber : topic->multiSubscribers) {
    if (subscriber->options.keepDuplicates || !isDuplicate) {
      subscriber->handle.Set();
      if (!subscriber->valueListeners.empty()) {
        m_listenerStorage.Notify(subscriber->valueListeners, eventFlags,
                                 topic->handle, 0, value);
      }
    }
  }
}

void StorageImpl::PropertiesUpdated(LocalTopic* topic, const wpi::json& update,
                                    unsigned int eventFlags, bool sendNetwork,
                                    bool updateFlags) {
  DEBUG4("PropertiesUpdated({}, {}, {}, {}, {})", topic->name, update.dump(),
         eventFlags, sendNetwork, updateFlags);
  topic->RefreshProperties(updateFlags);
  unsigned int flags = topic->GetFlags();
  if (updateFlags && (flags & NT_UNCACHED) != 0 &&
      (flags & NT_PERSISTENT) != 0) {
    WARN("topic {}: disabling cached property disables persistent storage",
         topic->name);
  }

  NotifyTopic(topic, eventFlags | NT_EVENT_PROPERTIES);
  // check local flag so we don't echo back received properties changes
  if (m_network && sendNetwork) {
    m_network->ClientSetProperties(topic->name, update);
  }
}

void StorageImpl::RefreshPubSubActive(LocalTopic* topic,
                                      bool warnOnSubMismatch) {
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

LocalPublisher* StorageImpl::AddLocalPublisher(LocalTopic* topic,
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
      WARN("ignoring non-object properties when publishing '{}'", topic->name);
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
    m_network->ClientPublish(Handle{publisher->handle}.GetIndex(), topic->name,
                             topic->typeStr, topic->properties, config);
  }
  return publisher;
}

std::unique_ptr<LocalPublisher> StorageImpl::RemoveLocalPublisher(
    NT_Publisher pubHandle) {
  auto publisher = m_publishers.Remove(pubHandle);
  if (publisher) {
    auto topic = publisher->topic;
    bool didExist = topic->Exists();
    topic->localPublishers.Remove(publisher.get());
    if (didExist && !topic->Exists()) {
      topic->ResetIfDoesNotExist();
      NotifyTopic(topic, NT_EVENT_UNPUBLISH);
    }

    if (publisher->active && m_network) {
      m_network->ClientUnpublish(Handle{publisher->handle}.GetIndex());
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
          m_network->ClientPublish(Handle{nextPub->handle}.GetIndex(),
                                   topic->name, topic->typeStr,
                                   topic->properties, nextPub->config);
        }
      }
    }
  }
  return publisher;
}

LocalSubscriber* StorageImpl::AddLocalSubscriber(LocalTopic* topic,
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
  if (m_network && !subscriber->config.hidden) {
    DEBUG4("-> NetworkSubscribe({})", topic->name);
    m_network->ClientSubscribe(1 + Handle{subscriber->handle}.GetIndex(),
                               {{topic->name}}, config);
  }

  // queue current value
  if (subscriber->active) {
    if (!topic->lastValueFromNetwork && !config.disableLocal) {
      subscriber->pollStorage.emplace_back(topic->lastValue);
      subscriber->handle.Set();
    } else if (topic->lastValueFromNetwork && !config.disableRemote) {
      subscriber->pollStorage.emplace_back(topic->lastValueNetwork);
      subscriber->handle.Set();
    }
  }
  return subscriber;
}

std::unique_ptr<LocalSubscriber> StorageImpl::RemoveLocalSubscriber(
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
    if (m_network && !subscriber->config.hidden) {
      m_network->ClientUnsubscribe(1 + Handle{subscriber->handle}.GetIndex());
    }
  }
  return subscriber;
}

LocalPublisher* StorageImpl::PublishEntry(LocalEntry* entry, NT_Type type) {
  if (entry->publisher) {
    return entry->publisher;
  }
  if (entry->subscriber->config.type == NT_UNASSIGNED) {
    auto typeStr = TypeToString(type);
    entry->subscriber->config.type = type;
    entry->subscriber->config.typeStr = typeStr;
  } else if (entry->subscriber->config.type != type) {
    if (!IsNumericCompatible(type, entry->subscriber->config.type)) {
      // don't allow dynamically changing the type of an entry
      auto typeStr = TypeToString(type);
      ERR("cannot publish entry {} as type {}, previously subscribed as {}",
          entry->topic->name, typeStr, entry->subscriber->config.typeStr);
      return nullptr;
    }
  }
  // create publisher
  entry->publisher = AddLocalPublisher(entry->topic, wpi::json::object(),
                                       entry->subscriber->config);
  // exclude publisher if requested
  if (entry->subscriber->config.excludeSelf) {
    entry->subscriber->config.excludePublisher = entry->publisher->handle;
  }
  return entry->publisher;
}

bool StorageImpl::PublishLocalValue(LocalPublisher* publisher,
                                    const Value& value, bool force) {
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
    bool isNetworkDuplicate, suppressDuplicates;
    if (force || publisher->config.keepDuplicates) {
      suppressDuplicates = false;
      isNetworkDuplicate = false;
    } else {
      suppressDuplicates = true;
      isNetworkDuplicate = publisher->topic->IsCached() &&
                           (publisher->topic->lastValueNetwork == value);
    }
    if (!isNetworkDuplicate && m_network) {
      if (publisher->topic->IsCached()) {
        publisher->topic->lastValueNetwork = value;
      }
      m_network->ClientSetValue(Handle{publisher->handle}.GetIndex(), value);
    }
    return SetValue(publisher->topic, value, NT_EVENT_VALUE_LOCAL,
                    suppressDuplicates, publisher);
  } else {
    return false;
  }
}
