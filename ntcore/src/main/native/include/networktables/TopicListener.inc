// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "networktables/MultiSubscriber.h"
#include "networktables/NetworkTableEntry.h"
#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"
#include "networktables/TopicListener.h"
#include "ntcore_cpp.h"

namespace nt {

inline TopicListener::TopicListener(
    NetworkTableInstance inst, std::span<const std::string_view> prefixes,
    unsigned int mask, std::function<void(const TopicNotification&)> listener)
    : m_handle{AddTopicListener(inst.GetHandle(), prefixes, mask, listener)} {}

inline TopicListener::TopicListener(
    Topic topic, unsigned int mask,
    std::function<void(const TopicNotification&)> listener)
    : m_handle{AddTopicListener(topic.GetHandle(), mask, listener)} {}

inline TopicListener::TopicListener(
    const Subscriber& subscriber, unsigned int mask,
    std::function<void(const TopicNotification&)> listener)
    : m_handle{AddTopicListener(subscriber.GetHandle(), mask, listener)} {}

inline TopicListener::TopicListener(
    const MultiSubscriber& subscriber, unsigned int mask,
    std::function<void(const TopicNotification&)> listener)
    : m_handle{AddTopicListener(subscriber.GetHandle(), mask, listener)} {}

inline TopicListener::TopicListener(
    const NetworkTableEntry& entry, unsigned int mask,
    std::function<void(const TopicNotification&)> listener)
    : m_handle{AddTopicListener(entry.GetHandle(), mask, listener)} {}

inline TopicListener::TopicListener(TopicListener&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline TopicListener& TopicListener::operator=(TopicListener&& rhs) {
  std::swap(m_handle, rhs.m_handle);
  return *this;
}

inline TopicListener::~TopicListener() {
  if (m_handle != 0) {
    nt::RemoveTopicListener(m_handle);
  }
}

inline TopicListenerPoller::TopicListenerPoller(NetworkTableInstance inst)
    : m_handle(nt::CreateTopicListenerPoller(inst.GetHandle())) {}

inline TopicListenerPoller::TopicListenerPoller(TopicListenerPoller&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline TopicListenerPoller& TopicListenerPoller::operator=(
    TopicListenerPoller&& rhs) {
  std::swap(m_handle, rhs.m_handle);
  return *this;
}

inline TopicListenerPoller::~TopicListenerPoller() {
  if (m_handle != 0) {
    nt::DestroyTopicListenerPoller(m_handle);
  }
}

inline NT_TopicListener TopicListenerPoller::Add(
    std::span<const std::string_view> prefixes, unsigned int mask) {
  return nt::AddPolledTopicListener(m_handle, prefixes, mask);
}

inline NT_TopicListener TopicListenerPoller::Add(Topic topic,
                                                 unsigned int mask) {
  return nt::AddPolledTopicListener(m_handle, topic.GetHandle(), mask);
}

inline NT_TopicListener TopicListenerPoller::Add(Subscriber& subscriber,
                                                 unsigned int mask) {
  return nt::AddPolledTopicListener(m_handle, subscriber.GetHandle(), mask);
}

inline NT_TopicListener TopicListenerPoller::Add(MultiSubscriber& subscriber,
                                                 unsigned int mask) {
  return nt::AddPolledTopicListener(m_handle, subscriber.GetHandle(), mask);
}

inline NT_TopicListener TopicListenerPoller::Add(NetworkTableEntry& entry,
                                                 unsigned int mask) {
  return nt::AddPolledTopicListener(m_handle, entry.GetHandle(), mask);
}

inline void TopicListenerPoller::Remove(NT_TopicListener listener) {
  nt::RemoveTopicListener(listener);
}

inline std::vector<TopicNotification> TopicListenerPoller::ReadQueue() {
  return nt::ReadTopicListenerQueue(m_handle);
}

}  // namespace nt
