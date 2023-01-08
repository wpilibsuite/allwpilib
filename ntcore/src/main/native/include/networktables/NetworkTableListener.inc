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
#include "networktables/NetworkTableListener.h"
#include "networktables/Topic.h"
#include "ntcore_cpp.h"

namespace nt {

inline NetworkTableListener NetworkTableListener::CreateListener(
    NetworkTableInstance inst, std::span<const std::string_view> prefixes,
    unsigned int mask, ListenerCallback listener) {
  return NetworkTableListener{
      ::nt::AddListener(inst.GetHandle(), prefixes, mask, std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateListener(
    Topic topic, unsigned int mask, ListenerCallback listener) {
  return NetworkTableListener{
      nt::AddListener(topic.GetHandle(), mask, std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateListener(
    Subscriber& subscriber, unsigned int mask, ListenerCallback listener) {
  return NetworkTableListener{
      ::nt::AddListener(subscriber.GetHandle(), mask, std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateListener(
    MultiSubscriber& subscriber, unsigned int mask, ListenerCallback listener) {
  return NetworkTableListener{
      ::nt::AddListener(subscriber.GetHandle(), mask, std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateListener(
    NetworkTableEntry& entry, unsigned int mask, ListenerCallback listener) {
  return NetworkTableListener{
      ::nt::AddListener(entry.GetHandle(), mask, std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateConnectionListener(
    NetworkTableInstance inst, bool immediate_notify,
    ListenerCallback listener) {
  return NetworkTableListener{::nt::AddListener(
      inst.GetHandle(),
      NT_EVENT_CONNECTION | (immediate_notify ? NT_EVENT_IMMEDIATE : 0),
      std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateTimeSyncListener(
    NetworkTableInstance inst, bool immediate_notify,
    ListenerCallback listener) {
  return NetworkTableListener{::nt::AddListener(
      inst.GetHandle(),
      NT_EVENT_TIMESYNC | (immediate_notify ? NT_EVENT_IMMEDIATE : 0),
      std::move(listener))};
}

inline NetworkTableListener NetworkTableListener::CreateLogger(
    NetworkTableInstance inst, unsigned int minLevel, unsigned int maxLevel,
    ListenerCallback listener) {
  return NetworkTableListener{::nt::AddLogger(inst.GetHandle(), minLevel,
                                              maxLevel, std::move(listener))};
}

inline NetworkTableListener::NetworkTableListener(NetworkTableListener&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline NetworkTableListener& NetworkTableListener::operator=(
    NetworkTableListener&& rhs) {
  if (m_handle != 0) {
    nt::RemoveListener(m_handle);
  }
  m_handle = rhs.m_handle;
  rhs.m_handle = 0;
  return *this;
}

inline NetworkTableListener::~NetworkTableListener() {
  if (m_handle != 0) {
    nt::RemoveListener(m_handle);
  }
}

inline bool NetworkTableListener::WaitForQueue(double timeout) {
  if (m_handle != 0) {
    return nt::WaitForListenerQueue(m_handle, timeout);
  } else {
    return false;
  }
}

inline NetworkTableListenerPoller::NetworkTableListenerPoller(
    NetworkTableInstance inst)
    : m_handle(nt::CreateListenerPoller(inst.GetHandle())) {}

inline NetworkTableListenerPoller::NetworkTableListenerPoller(
    NetworkTableListenerPoller&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline NetworkTableListenerPoller& NetworkTableListenerPoller::operator=(
    NetworkTableListenerPoller&& rhs) {
  if (m_handle != 0) {
    nt::DestroyListenerPoller(m_handle);
  }
  m_handle = rhs.m_handle;
  rhs.m_handle = 0;
  return *this;
}

inline NetworkTableListenerPoller::~NetworkTableListenerPoller() {
  if (m_handle != 0) {
    nt::DestroyListenerPoller(m_handle);
  }
}

inline NT_Listener NetworkTableListenerPoller::AddListener(
    std::span<const std::string_view> prefixes, unsigned int mask) {
  return nt::AddPolledListener(m_handle, prefixes, mask);
}

inline NT_Listener NetworkTableListenerPoller::AddListener(Topic topic,
                                                           unsigned int mask) {
  return ::nt::AddPolledListener(m_handle, topic.GetHandle(), mask);
}

inline NT_Listener NetworkTableListenerPoller::AddListener(
    Subscriber& subscriber, unsigned int mask) {
  return ::nt::AddPolledListener(m_handle, subscriber.GetHandle(), mask);
}

inline NT_Listener NetworkTableListenerPoller::AddListener(
    MultiSubscriber& subscriber, unsigned int mask) {
  return ::nt::AddPolledListener(m_handle, subscriber.GetHandle(), mask);
}

inline NT_Listener NetworkTableListenerPoller::AddListener(
    NetworkTableEntry& entry, unsigned int mask) {
  return ::nt::AddPolledListener(m_handle, entry.GetHandle(), mask);
}

inline NT_Listener NetworkTableListenerPoller::AddConnectionListener(
    bool immediate_notify) {
  return ::nt::AddPolledListener(
      m_handle, ::nt::GetInstanceFromHandle(m_handle),
      NT_EVENT_CONNECTION | (immediate_notify ? NT_EVENT_IMMEDIATE : 0));
}

inline NT_Listener NetworkTableListenerPoller::AddTimeSyncListener(
    bool immediate_notify) {
  return ::nt::AddPolledListener(
      m_handle, ::nt::GetInstanceFromHandle(m_handle),
      NT_EVENT_TIMESYNC | (immediate_notify ? NT_EVENT_IMMEDIATE : 0));
}

inline NT_Listener NetworkTableListenerPoller::AddLogger(
    unsigned int minLevel, unsigned int maxLevel) {
  return ::nt::AddPolledLogger(m_handle, minLevel, maxLevel);
}

inline void NetworkTableListenerPoller::RemoveListener(NT_Listener listener) {
  ::nt::RemoveListener(listener);
}

inline std::vector<Event> NetworkTableListenerPoller::ReadQueue() {
  return ::nt::ReadListenerQueue(m_handle);
}

}  // namespace nt
