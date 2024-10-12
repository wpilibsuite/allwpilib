// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalStorage.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/ranges.h>
#include <wpi/DataLog.h>
#include <wpi/SmallString.h>
#include <wpi/StringExtras.h>
#include <wpi/json.h>

#include "IListenerStorage.h"
#include "Log.h"
#include "Types_internal.h"
#include "Value_internal.h"
#include "net/MessageHandler.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;

// maximum number of local publishers / subscribers to any given topic
static constexpr size_t kMaxPublishers = 512;
static constexpr size_t kMaxSubscribers = 512;
static constexpr size_t kMaxMultiSubscribers = 512;
static constexpr size_t kMaxListeners = 512;

static constexpr bool PrefixMatch(std::string_view name,
                                  std::string_view prefix, bool special) {
  return (!special || !prefix.empty()) && wpi::starts_with(name, prefix);
}

std::string LocalStorage::DataLoggerEntry::MakeMetadata(
    std::string_view properties) {
  return fmt::format("{{\"properties\":{},\"source\":\"NT\"}}", properties);
}

bool LocalStorage::MultiSubscriberData::Matches(std::string_view name,
                                                bool special) {
  for (auto&& prefix : prefixes) {
    if (PrefixMatch(name, prefix, special)) {
      return true;
    }
  }
  return false;
}

int LocalStorage::DataLoggerData::Start(TopicData* topic, int64_t time) {
  std::string_view typeStr = topic->typeStr;
  // NT and DataLog use different standard representations for int and int[]
  if (typeStr == "int") {
    typeStr = "int64";
  } else if (typeStr == "int[]") {
    typeStr = "int64[]";
  }
  return log.Start(
      fmt::format(
          "{}{}", logPrefix,
          wpi::remove_prefix(topic->name, prefix).value_or(topic->name)),
      typeStr, DataLoggerEntry::MakeMetadata(topic->propertiesStr), time);
}

void LocalStorage::DataLoggerEntry::Append(const Value& v) {
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

TopicInfo LocalStorage::TopicData::GetTopicInfo() const {
  TopicInfo info;
  info.topic = handle;
  info.name = name;
  info.type = type;
  info.type_str = typeStr;
  info.properties = propertiesStr;
  return info;
}

void LocalStorage::Impl::NotifyTopic(TopicData* topic,
                                     unsigned int eventFlags) {
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

void LocalStorage::Impl::CheckReset(TopicData* topic) {
  if (topic->Exists()) {
    return;
  }
  topic->lastValue = {};
  topic->lastValueNetwork = {};
  topic->lastValueFromNetwork = false;
  topic->type = NT_UNASSIGNED;
  topic->typeStr.clear();
  topic->flags = 0;
  topic->properties = wpi::json::object();
  topic->propertiesStr = "{}";
}

bool LocalStorage::Impl::SetValue(TopicData* topic, const Value& value,
                                  unsigned int eventFlags,
                                  bool suppressIfDuplicate,
                                  const PublisherData* publisher) {
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

void LocalStorage::Impl::NotifyValue(TopicData* topic, const Value& value,
                                     unsigned int eventFlags, bool isDuplicate,
                                     const PublisherData* publisher) {
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

void LocalStorage::Impl::SetFlags(TopicData* topic, unsigned int flags) {
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
  if ((flags & NT_UNCACHED) != 0) {
    topic->properties["cached"] = false;
    update["cached"] = false;
  } else {
    topic->properties.erase("cached");
    update["cached"] = wpi::json();
  }
  if ((flags & NT_UNCACHED) != 0) {
    topic->lastValue = {};
    topic->lastValueNetwork = {};
    topic->lastValueFromNetwork = false;
  }
  if ((flags & NT_UNCACHED) != 0 && (flags & NT_PERSISTENT) != 0) {
    WARN("topic {}: disabling cached property disables persistent storage",
         topic->name);
  }
  topic->flags = flags;
  if (!update.empty()) {
    PropertiesUpdated(topic, update, NT_EVENT_NONE, true, false);
  }
}

void LocalStorage::Impl::SetPersistent(TopicData* topic, bool value) {
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

void LocalStorage::Impl::SetRetained(TopicData* topic, bool value) {
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

void LocalStorage::Impl::SetCached(TopicData* topic, bool value) {
  wpi::json update = wpi::json::object();
  if (value) {
    topic->flags &= ~NT_UNCACHED;
    topic->properties.erase("cached");
    update["cached"] = wpi::json();
  } else {
    topic->flags |= NT_UNCACHED;
    topic->properties["cached"] = false;
    update["cached"] = false;
  }
  PropertiesUpdated(topic, update, NT_EVENT_NONE, true, false);
}

void LocalStorage::Impl::SetProperties(TopicData* topic,
                                       const wpi::json& update,
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

void LocalStorage::Impl::PropertiesUpdated(TopicData* topic,
                                           const wpi::json& update,
                                           unsigned int eventFlags,
                                           bool sendNetwork, bool updateFlags) {
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
    it = topic->properties.find("cached");
    if (it != topic->properties.end()) {
      if (auto val = it->get_ptr<bool*>()) {
        if (*val) {
          topic->flags &= ~NT_UNCACHED;
        } else {
          topic->flags |= NT_UNCACHED;
        }
      }
    }

    if ((topic->flags & NT_UNCACHED) != 0) {
      topic->lastValue = {};
      topic->lastValueNetwork = {};
      topic->lastValueFromNetwork = false;
    }

    if ((topic->flags & NT_UNCACHED) != 0 &&
        (topic->flags & NT_PERSISTENT) != 0) {
      WARN("topic {}: disabling cached property disables persistent storage",
           topic->name);
    }
  }

  topic->propertiesStr = topic->properties.dump();
  NotifyTopic(topic, eventFlags | NT_EVENT_PROPERTIES);
  // check local flag so we don't echo back received properties changes
  if (m_network && sendNetwork) {
    m_network->ClientSetProperties(topic->name, update);
  }
}

void LocalStorage::Impl::RefreshPubSubActive(TopicData* topic,
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

void LocalStorage::Impl::NetworkAnnounce(TopicData* topic,
                                         std::string_view typeStr,
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

void LocalStorage::Impl::RemoveNetworkPublisher(TopicData* topic) {
  DEBUG4("LS RemoveNetworkPublisher({}, {})", topic->handle.GetHandle(),
         topic->name);
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
        m_network->ClientPublish(Handle{nextPub->handle}.GetIndex(),
                                 topic->name, topic->typeStr, topic->properties,
                                 nextPub->config);
      }
    }
  }
}

void LocalStorage::Impl::NetworkPropertiesUpdate(TopicData* topic,
                                                 const wpi::json& update,
                                                 bool ack) {
  DEBUG4("NetworkPropertiesUpdate({},{})", topic->name, ack);
  if (ack) {
    return;  // ignore acks
  }
  SetProperties(topic, update, false);
}

LocalStorage::PublisherData* LocalStorage::Impl::AddLocalPublisher(
    TopicData* topic, const wpi::json& properties, const PubSubConfig& config) {
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

std::unique_ptr<LocalStorage::PublisherData>
LocalStorage::Impl::RemoveLocalPublisher(NT_Publisher pubHandle) {
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

LocalStorage::SubscriberData* LocalStorage::Impl::AddLocalSubscriber(
    TopicData* topic, const PubSubConfig& config) {
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

std::unique_ptr<LocalStorage::SubscriberData>
LocalStorage::Impl::RemoveLocalSubscriber(NT_Subscriber subHandle) {
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

LocalStorage::EntryData* LocalStorage::Impl::AddEntry(
    SubscriberData* subscriber) {
  auto entry = m_entries.Add(m_inst, subscriber);
  subscriber->topic->entries.Add(entry);
  return entry;
}

std::unique_ptr<LocalStorage::EntryData> LocalStorage::Impl::RemoveEntry(
    NT_Entry entryHandle) {
  auto entry = m_entries.Remove(entryHandle);
  if (entry) {
    entry->topic->entries.Remove(entry.get());
  }
  return entry;
}

LocalStorage::MultiSubscriberData* LocalStorage::Impl::AddMultiSubscriber(
    std::span<const std::string_view> prefixes, const PubSubOptions& options) {
  DEBUG4("AddMultiSubscriber({})", fmt::join(prefixes, ","));
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

std::unique_ptr<LocalStorage::MultiSubscriberData>
LocalStorage::Impl::RemoveMultiSubscriber(NT_MultiSubscriber subHandle) {
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

void LocalStorage::Impl::AddListenerImpl(NT_Listener listenerHandle,
                                         TopicData* topic,
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

void LocalStorage::Impl::AddListenerImpl(NT_Listener listenerHandle,
                                         SubscriberData* subscriber,
                                         unsigned int eventMask,
                                         NT_Handle subentryHandle,
                                         bool subscriberOwned) {
  m_listeners.try_emplace(listenerHandle, std::make_unique<ListenerData>(
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

void LocalStorage::Impl::AddListenerImpl(NT_Listener listenerHandle,
                                         MultiSubscriberData* subscriber,
                                         unsigned int eventMask,
                                         bool subscriberOwned) {
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

LocalStorage::TopicData* LocalStorage::Impl::GetOrCreateTopic(
    std::string_view name) {
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

LocalStorage::TopicData* LocalStorage::Impl::GetTopic(NT_Handle handle) {
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

LocalStorage::SubscriberData* LocalStorage::Impl::GetSubEntry(
    NT_Handle subentryHandle) {
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

LocalStorage::PublisherData* LocalStorage::Impl::PublishEntry(EntryData* entry,
                                                              NT_Type type) {
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

bool LocalStorage::Impl::PublishLocalValue(PublisherData* publisher,
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

bool LocalStorage::Impl::SetEntryValue(NT_Handle pubentryHandle,
                                       const Value& value) {
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

bool LocalStorage::Impl::SetDefaultEntryValue(NT_Handle pubsubentryHandle,
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

void LocalStorage::Impl::RemoveSubEntry(NT_Handle subentryHandle) {
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

LocalStorage::Impl::Impl(int inst, IListenerStorage& listenerStorage,
                         wpi::Logger& logger)
    : m_inst{inst}, m_listenerStorage{listenerStorage}, m_logger{logger} {}

LocalStorage::~LocalStorage() = default;

int LocalStorage::ServerAnnounce(std::string_view name, int id,
                                 std::string_view typeStr,
                                 const wpi::json& properties,
                                 std::optional<int> pubuid) {
  std::scoped_lock lock{m_mutex};
  auto topic = m_impl.GetOrCreateTopic(name);
  m_impl.NetworkAnnounce(topic, typeStr, properties, pubuid);
  return Handle{topic->handle}.GetIndex();
}

void LocalStorage::ServerUnannounce(std::string_view name, int id) {
  std::scoped_lock lock{m_mutex};
  auto topic = m_impl.GetOrCreateTopic(name);
  m_impl.RemoveNetworkPublisher(topic);
}

void LocalStorage::ServerPropertiesUpdate(std::string_view name,
                                          const wpi::json& update, bool ack) {
  std::scoped_lock lock{m_mutex};
  auto it = m_impl.m_nameTopics.find(name);
  if (it != m_impl.m_nameTopics.end()) {
    m_impl.NetworkPropertiesUpdate(it->second, update, ack);
  }
}

void LocalStorage::ServerSetValue(int topicId, const Value& value) {
  std::scoped_lock lock{m_mutex};
  if (auto topic =
          m_impl.m_topics.Get(Handle{m_impl.m_inst, topicId, Handle::kTopic})) {
    if (m_impl.SetValue(topic, value, NT_EVENT_VALUE_REMOTE, false, nullptr)) {
      if (topic->IsCached()) {
        topic->lastValueNetwork = value;
        topic->lastValueFromNetwork = true;
      }
    }
  }
}

void LocalStorage::StartNetwork(net::ClientMessageHandler* network) {
  std::scoped_lock lock{m_mutex};
  m_impl.StartNetwork(network);
}

void LocalStorage::Impl::StartNetwork(net::ClientMessageHandler* network) {
  DEBUG4("StartNetwork()");
  m_network = network;
  // publish all active publishers to the network and send last values
  // only send value once per topic
  for (auto&& topic : m_topics) {
    PublisherData* anyPublisher = nullptr;
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

void LocalStorage::ClearNetwork() {
  WPI_DEBUG4(m_impl.m_logger, "ClearNetwork()");
  std::scoped_lock lock{m_mutex};
  m_impl.m_network = nullptr;
  // treat as an unannounce all from the network side
  for (auto&& topic : m_impl.m_topics) {
    m_impl.RemoveNetworkPublisher(topic.get());
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
  ForEachTopic(m_impl.m_topics, prefix, types,
               [&](TopicData& topic) { rv.push_back(topic.handle); });
  return rv;
}

std::vector<NT_Topic> LocalStorage::GetTopics(
    std::string_view prefix, std::span<const std::string_view> types) {
  std::scoped_lock lock(m_mutex);
  std::vector<NT_Topic> rv;
  ForEachTopic(m_impl.m_topics, prefix, types,
               [&](TopicData& topic) { rv.push_back(topic.handle); });
  return rv;
}

std::vector<TopicInfo> LocalStorage::GetTopicInfo(std::string_view prefix,
                                                  unsigned int types) {
  std::scoped_lock lock(m_mutex);
  std::vector<TopicInfo> rv;
  ForEachTopic(m_impl.m_topics, prefix, types, [&](TopicData& topic) {
    rv.emplace_back(topic.GetTopicInfo());
  });
  return rv;
}

std::vector<TopicInfo> LocalStorage::GetTopicInfo(
    std::string_view prefix, std::span<const std::string_view> types) {
  std::scoped_lock lock(m_mutex);
  std::vector<TopicInfo> rv;
  ForEachTopic(m_impl.m_topics, prefix, types, [&](TopicData& topic) {
    rv.emplace_back(topic.GetTopicInfo());
  });
  return rv;
}

void LocalStorage::SetTopicProperty(NT_Topic topicHandle, std::string_view name,
                                    const wpi::json& value) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl.m_topics.Get(topicHandle)) {
    if (value.is_null()) {
      topic->properties.erase(name);
    } else {
      topic->properties[name] = value;
    }
    wpi::json update = wpi::json::object();
    update[name] = value;
    m_impl.PropertiesUpdated(topic, update, NT_EVENT_NONE, true);
  }
}

void LocalStorage::DeleteTopicProperty(NT_Topic topicHandle,
                                       std::string_view name) {
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl.m_topics.Get(topicHandle)) {
    topic->properties.erase(name);
    wpi::json update = wpi::json::object();
    update[name] = wpi::json();
    m_impl.PropertiesUpdated(topic, update, NT_EVENT_NONE, true);
  }
}

bool LocalStorage::SetTopicProperties(NT_Topic topicHandle,
                                      const wpi::json& update) {
  if (!update.is_object()) {
    return false;
  }
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl.m_topics.Get(topicHandle)) {
    m_impl.SetProperties(topic, update, true);
    return true;
  } else {
    return {};
  }
}

NT_Subscriber LocalStorage::Subscribe(NT_Topic topicHandle, NT_Type type,
                                      std::string_view typeStr,
                                      const PubSubOptions& options) {
  std::scoped_lock lock{m_mutex};

  // Get the topic
  auto* topic = m_impl.m_topics.Get(topicHandle);
  if (!topic) {
    return 0;
  }

  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    WPI_ERROR(m_impl.m_logger,
              "reached maximum number of subscribers to '{}', not subscribing",
              topic->name);
    return 0;
  }

  // Create subscriber
  return m_impl.AddLocalSubscriber(topic, PubSubConfig{type, typeStr, options})
      ->handle;
}

NT_MultiSubscriber LocalStorage::SubscribeMultiple(
    std::span<const std::string_view> prefixes, const PubSubOptions& options) {
  std::scoped_lock lock{m_mutex};

  if (m_impl.m_multiSubscribers.size() >= kMaxMultiSubscribers) {
    WPI_ERROR(m_impl.m_logger,
              "reached maximum number of multi-subscribers, not subscribing");
    return 0;
  }

  return m_impl.AddMultiSubscriber(prefixes, options)->handle;
}

NT_Publisher LocalStorage::Publish(NT_Topic topicHandle, NT_Type type,
                                   std::string_view typeStr,
                                   const wpi::json& properties,
                                   const PubSubOptions& options) {
  std::scoped_lock lock{m_mutex};

  // Get the topic
  auto* topic = m_impl.m_topics.Get(topicHandle);
  if (!topic) {
    WPI_ERROR(m_impl.m_logger, "trying to publish invalid topic handle ({})",
              topicHandle);
    return 0;
  }

  if (type == NT_UNASSIGNED || typeStr.empty()) {
    WPI_ERROR(
        m_impl.m_logger,
        "cannot publish '{}' with an unassigned type or empty type string",
        topic->name);
    return 0;
  }

  if (topic->localPublishers.size() >= kMaxPublishers) {
    WPI_ERROR(m_impl.m_logger,
              "reached maximum number of publishers to '{}', not publishing",
              topic->name);
    return 0;
  }

  return m_impl
      .AddLocalPublisher(topic, properties,
                         PubSubConfig{type, typeStr, options})
      ->handle;
}

void LocalStorage::Unpublish(NT_Handle pubentryHandle) {
  std::scoped_lock lock{m_mutex};

  if (Handle{pubentryHandle}.IsType(Handle::kPublisher)) {
    m_impl.RemoveLocalPublisher(pubentryHandle);
  } else if (auto entry = m_impl.m_entries.Get(pubentryHandle)) {
    if (entry->publisher) {
      m_impl.RemoveLocalPublisher(entry->publisher->handle);
      entry->publisher = nullptr;
    }
  } else {
    // TODO: report warning
    return;
  }
}

NT_Entry LocalStorage::GetEntry(NT_Topic topicHandle, NT_Type type,
                                std::string_view typeStr,
                                const PubSubOptions& options) {
  std::scoped_lock lock{m_mutex};

  // Get the topic
  auto* topic = m_impl.m_topics.Get(topicHandle);
  if (!topic) {
    return 0;
  }

  if (topic->localSubscribers.size() >= kMaxSubscribers) {
    WPI_ERROR(
        m_impl.m_logger,
        "reached maximum number of subscribers to '{}', not creating entry",
        topic->name);
    return 0;
  }

  // Create subscriber
  auto subscriber =
      m_impl.AddLocalSubscriber(topic, PubSubConfig{type, typeStr, options});

  // Create entry
  return m_impl.AddEntry(subscriber)->handle;
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

Value LocalStorage::GetEntryValue(NT_Handle subentryHandle) {
  std::scoped_lock lock{m_mutex};
  if (auto subscriber = m_impl.GetSubEntry(subentryHandle)) {
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

NT_Entry LocalStorage::GetEntry(std::string_view name) {
  if (name.empty()) {
    return {};
  }

  std::scoped_lock lock{m_mutex};

  // Get the topic data
  auto* topic = m_impl.GetOrCreateTopic(name);

  if (topic->entry == 0) {
    if (topic->localSubscribers.size() >= kMaxSubscribers) {
      WPI_ERROR(
          m_impl.m_logger,
          "reached maximum number of subscribers to '{}', not creating entry",
          topic->name);
      return 0;
    }

    // Create subscriber
    auto* subscriber = m_impl.AddLocalSubscriber(topic, {});

    // Create entry
    topic->entry = m_impl.AddEntry(subscriber)->handle;
  }

  return topic->entry;
}

void LocalStorage::AddListener(NT_Listener listenerHandle,
                               std::span<const std::string_view> prefixes,
                               unsigned int mask) {
  mask &= (NT_EVENT_TOPIC | NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE);
  std::scoped_lock lock{m_mutex};
  if (m_impl.m_multiSubscribers.size() >= kMaxMultiSubscribers) {
    WPI_ERROR(
        m_impl.m_logger,
        "reached maximum number of multi-subscribers, not adding listener");
    return;
  }
  // subscribe to make sure topic updates are received
  auto sub = m_impl.AddMultiSubscriber(
      prefixes, {.topicsOnly = (mask & NT_EVENT_VALUE_ALL) == 0});
  m_impl.AddListenerImpl(listenerHandle, sub, mask, true);
}

void LocalStorage::AddListener(NT_Listener listenerHandle, NT_Handle handle,
                               unsigned int mask) {
  mask &= (NT_EVENT_TOPIC | NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE);
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl.m_topics.Get(handle)) {
    m_impl.AddListenerImpl(listenerHandle, topic, mask);
  } else if (auto sub = m_impl.m_multiSubscribers.Get(handle)) {
    m_impl.AddListenerImpl(listenerHandle, sub, mask, false);
  } else if (auto sub = m_impl.m_subscribers.Get(handle)) {
    m_impl.AddListenerImpl(listenerHandle, sub, mask, sub->handle, false);
  } else if (auto entry = m_impl.m_entries.Get(handle)) {
    m_impl.AddListenerImpl(listenerHandle, entry->subscriber, mask,
                           entry->handle, false);
  }
}

void LocalStorage::RemoveListener(NT_Listener listenerHandle,
                                  unsigned int mask) {
  std::scoped_lock lock{m_mutex};
  auto listenerIt = m_impl.m_listeners.find(listenerHandle);
  if (listenerIt == m_impl.m_listeners.end()) {
    return;
  }
  auto listener = std::move(listenerIt->getSecond());
  m_impl.m_listeners.erase(listenerIt);
  if (!listener) {
    return;
  }

  m_impl.m_topicPrefixListeners.Remove(listener.get());
  if (listener->subscriber) {
    listener->subscriber->valueListeners.Remove(listenerHandle);
    listener->subscriber->topic->listeners.Remove(listenerHandle);
    if (listener->subscriberOwned) {
      m_impl.RemoveLocalSubscriber(listener->subscriber->handle);
    }
  }
  if (listener->multiSubscriber) {
    listener->multiSubscriber->valueListeners.Remove(listenerHandle);
    if (listener->subscriberOwned) {
      m_impl.RemoveMultiSubscriber(listener->multiSubscriber->handle);
    }
  }
}

NT_DataLogger LocalStorage::StartDataLog(wpi::log::DataLog& log,
                                         std::string_view prefix,
                                         std::string_view logPrefix) {
  std::scoped_lock lock{m_mutex};
  auto datalogger =
      m_impl.m_dataloggers.Add(m_impl.m_inst, log, prefix, logPrefix);

  // start logging any matching topics
  auto now = nt::Now();
  for (auto&& topic : m_impl.m_topics) {
    if (!PrefixMatch(topic->name, prefix, topic->special) ||
        topic->type == NT_UNASSIGNED || topic->typeStr.empty()) {
      continue;
    }
    topic->datalogs.emplace_back(log, datalogger->Start(topic.get(), now),
                                 datalogger->handle);
    topic->datalogType = topic->type;

    // log current value, if any
    if (topic->lastValue) {
      topic->datalogs.back().Append(topic->lastValue);
    }
  }

  return datalogger->handle;
}

void LocalStorage::StopDataLog(NT_DataLogger logger) {
  std::scoped_lock lock{m_mutex};
  if (auto datalogger = m_impl.m_dataloggers.Remove(logger)) {
    // finish any active entries
    auto now = Now();
    for (auto&& topic : m_impl.m_topics) {
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

bool LocalStorage::HasSchema(std::string_view name) {
  std::scoped_lock lock{m_mutex};
  wpi::SmallString<128> fullName{"/.schema/"};
  fullName += name;
  auto it = m_impl.m_schemas.find(fullName);
  return it != m_impl.m_schemas.end();
}

void LocalStorage::AddSchema(std::string_view name, std::string_view type,
                             std::span<const uint8_t> schema) {
  std::scoped_lock lock{m_mutex};
  wpi::SmallString<128> fullName{"/.schema/"};
  fullName += name;
  auto& pubHandle = m_impl.m_schemas[fullName];
  if (pubHandle != 0) {
    return;
  }

  auto topic = m_impl.GetOrCreateTopic(fullName);

  if (topic->localPublishers.size() >= kMaxPublishers) {
    WPI_ERROR(m_impl.m_logger,
              "reached maximum number of publishers to '{}', not publishing",
              topic->name);
    return;
  }

  pubHandle = m_impl
                  .AddLocalPublisher(topic, {{"retained", true}},
                                     PubSubConfig{NT_RAW, type, {}})
                  ->handle;

  m_impl.SetDefaultEntryValue(pubHandle, Value::MakeRaw(schema));
}

void LocalStorage::Reset() {
  std::scoped_lock lock{m_mutex};
  m_impl.m_network = nullptr;
  m_impl.m_topics.clear();
  m_impl.m_publishers.clear();
  m_impl.m_subscribers.clear();
  m_impl.m_entries.clear();
  m_impl.m_multiSubscribers.clear();
  m_impl.m_dataloggers.clear();
  m_impl.m_nameTopics.clear();
  m_impl.m_listeners.clear();
  m_impl.m_topicPrefixListeners.clear();
}
