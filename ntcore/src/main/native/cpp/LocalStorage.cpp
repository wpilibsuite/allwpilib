// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "LocalStorage.h"

#include <vector>

using namespace nt;

std::vector<NT_Topic> LocalStorage::GetTopics(std::string_view prefix,
                                              unsigned int types) {
  std::scoped_lock lock(m_mutex);
  std::vector<NT_Topic> rv;
  m_impl.ForEachTopic(prefix, types,
                      [&](auto& topic) { rv.push_back(topic.handle); });
  return rv;
}

std::vector<NT_Topic> LocalStorage::GetTopics(
    std::string_view prefix, std::span<const std::string_view> types) {
  std::scoped_lock lock(m_mutex);
  std::vector<NT_Topic> rv;
  m_impl.ForEachTopic(prefix, types,
                      [&](auto& topic) { rv.push_back(topic.handle); });
  return rv;
}

std::vector<TopicInfo> LocalStorage::GetTopicInfo(std::string_view prefix,
                                                  unsigned int types) {
  std::scoped_lock lock(m_mutex);
  std::vector<TopicInfo> rv;
  m_impl.ForEachTopic(prefix, types, [&](auto& topic) {
    rv.emplace_back(topic.GetTopicInfo());
  });
  return rv;
}

std::vector<TopicInfo> LocalStorage::GetTopicInfo(
    std::string_view prefix, std::span<const std::string_view> types) {
  std::scoped_lock lock(m_mutex);
  std::vector<TopicInfo> rv;
  m_impl.ForEachTopic(prefix, types, [&](auto& topic) {
    rv.emplace_back(topic.GetTopicInfo());
  });
  return rv;
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

void LocalStorage::AddListener(NT_Listener listenerHandle, NT_Handle handle,
                               unsigned int mask) {
  mask &= (NT_EVENT_TOPIC | NT_EVENT_VALUE_ALL | NT_EVENT_IMMEDIATE);
  std::scoped_lock lock{m_mutex};
  if (auto topic = m_impl.GetTopicByHandle(handle)) {
    m_impl.AddListenerImpl(listenerHandle, topic, mask);
  } else if (auto sub = m_impl.GetMultiSubscriberByHandle(handle)) {
    m_impl.AddListenerImpl(listenerHandle, sub, mask, false);
  } else if (auto sub = m_impl.GetSubscriberByHandle(handle)) {
    m_impl.AddListenerImpl(listenerHandle, sub, mask, sub->handle, false);
  } else if (auto entry = m_impl.GetEntryByHandle(handle)) {
    m_impl.AddListenerImpl(listenerHandle, entry->subscriber, mask,
                           entry->handle, false);
  }
}
