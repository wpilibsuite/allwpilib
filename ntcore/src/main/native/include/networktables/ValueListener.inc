// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include "networktables/MultiSubscriber.h"
#include "networktables/NetworkTableEntry.h"
#include "networktables/NetworkTableInstance.h"
#include "networktables/Topic.h"
#include "networktables/ValueListener.h"
#include "ntcore_cpp.h"

namespace nt {

inline ValueListener::ValueListener(
    Subscriber& subscriber, unsigned int mask,
    std::function<void(const ValueNotification&)> listener)
    : m_handle{AddValueListener(subscriber.GetHandle(), mask, listener)} {}

inline ValueListener::ValueListener(
    MultiSubscriber& subscriber, unsigned int mask,
    std::function<void(const ValueNotification&)> listener)
    : m_handle{AddValueListener(subscriber.GetHandle(), mask, listener)} {}

inline ValueListener::ValueListener(
    NetworkTableEntry& entry, unsigned int mask,
    std::function<void(const ValueNotification&)> listener)
    : m_handle{AddValueListener(entry.GetHandle(), mask, listener)} {}

inline ValueListener::ValueListener(ValueListener&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline ValueListener& ValueListener::operator=(ValueListener&& rhs) {
  std::swap(m_handle, rhs.m_handle);
  return *this;
}

inline ValueListener::~ValueListener() {
  if (m_handle != 0) {
    nt::RemoveValueListener(m_handle);
  }
}

inline bool ValueListener::WaitForQueue(double timeout) {
  if (m_handle != 0) {
    return nt::WaitForValueListenerQueue(m_handle, timeout);
  } else {
    return false;
  }
}

inline ValueListenerPoller::ValueListenerPoller(NetworkTableInstance inst)
    : m_handle(nt::CreateValueListenerPoller(inst.GetHandle())) {}

inline ValueListenerPoller::ValueListenerPoller(ValueListenerPoller&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline ValueListenerPoller& ValueListenerPoller::operator=(
    ValueListenerPoller&& rhs) {
  std::swap(m_handle, rhs.m_handle);
  return *this;
}

inline ValueListenerPoller::~ValueListenerPoller() {
  if (m_handle != 0) {
    nt::DestroyValueListenerPoller(m_handle);
  }
}

inline NT_ValueListener ValueListenerPoller::Add(Subscriber& subscriber,
                                                 unsigned int mask) {
  return nt::AddPolledValueListener(m_handle, subscriber.GetHandle(), mask);
}

inline NT_ValueListener ValueListenerPoller::Add(MultiSubscriber& subscriber,
                                                 unsigned int mask) {
  return nt::AddPolledValueListener(m_handle, subscriber.GetHandle(), mask);
}

inline NT_ValueListener ValueListenerPoller::Add(NetworkTableEntry& entry,
                                                 unsigned int mask) {
  return nt::AddPolledValueListener(m_handle, entry.GetHandle(), mask);
}

inline void ValueListenerPoller::Remove(NT_ValueListener listener) {
  nt::RemoveValueListener(listener);
}

inline std::vector<ValueNotification> ValueListenerPoller::ReadQueue() {
  return nt::ReadValueListenerQueue(m_handle);
}

}  // namespace nt
