// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include <wpi/span.h>

#include "networktables/ConnectionListener.h"
#include "networktables/NetworkTableInstance.h"
#include "ntcore_cpp.h"

namespace nt {

inline ConnectionListener::ConnectionListener(
    NetworkTableInstance inst, bool immediateNotify,
    std::function<void(const ConnectionNotification&)> listener)
    : m_handle{
          AddConnectionListener(inst.GetHandle(), listener, immediateNotify)} {}

inline ConnectionListener::ConnectionListener(ConnectionListener&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline ConnectionListener& ConnectionListener::operator=(
    ConnectionListener&& rhs) {
  std::swap(m_handle, rhs.m_handle);
  return *this;
}

inline ConnectionListener::~ConnectionListener() {
  if (m_handle != 0) {
    nt::RemoveConnectionListener(m_handle);
  }
}

inline ConnectionListenerPoller::ConnectionListenerPoller(
    NetworkTableInstance inst)
    : m_handle(nt::CreateConnectionListenerPoller(inst.GetHandle())) {}

inline ConnectionListenerPoller::ConnectionListenerPoller(
    ConnectionListenerPoller&& rhs)
    : m_handle(rhs.m_handle) {
  rhs.m_handle = 0;
}

inline ConnectionListenerPoller& ConnectionListenerPoller::operator=(
    ConnectionListenerPoller&& rhs) {
  std::swap(m_handle, rhs.m_handle);
  return *this;
}

inline ConnectionListenerPoller::~ConnectionListenerPoller() {
  if (m_handle != 0) {
    nt::DestroyConnectionListenerPoller(m_handle);
  }
}

inline NT_ConnectionListener ConnectionListenerPoller::Add(
    bool immediateNotify) {
  return nt::AddPolledConnectionListener(m_handle, immediateNotify);
}

inline void ConnectionListenerPoller::Remove(NT_ConnectionListener listener) {
  nt::RemoveConnectionListener(listener);
}

inline std::vector<ConnectionNotification>
ConnectionListenerPoller::ReadQueue() {
  return nt::ReadConnectionListenerQueue(m_handle);
}

}  // namespace nt
