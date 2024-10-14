// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>

#include <wpi/Logger.h>

#include "ClientMessageQueue.h"

namespace nt::net::detail {

template <size_t MaxValueSize, bool IsMutexed>
inline void ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClientPublish(
    int pubuid, std::string_view name, std::string_view typeStr,
    const wpi::json& properties, const PubSubOptionsImpl& options) {
  std::scoped_lock lock{m_mutex};
  m_queue.enqueue(ClientMessage{PublishMsg{
      pubuid, std::string{name}, std::string{typeStr}, properties, options}});
}

template <size_t MaxValueSize, bool IsMutexed>
inline void ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClientUnpublish(
    int pubuid) {
  std::scoped_lock lock{m_mutex};
  m_queue.enqueue(ClientMessage{UnpublishMsg{pubuid}});
}

template <size_t MaxValueSize, bool IsMutexed>
inline void
ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClientSetProperties(
    std::string_view name, const wpi::json& update) {
  std::scoped_lock lock{m_mutex};
  m_queue.enqueue(ClientMessage{SetPropertiesMsg{std::string{name}, update}});
}

template <size_t MaxValueSize, bool IsMutexed>
inline void ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClientSubscribe(
    int subuid, std::span<const std::string> topicNames,
    const PubSubOptionsImpl& options) {
  std::scoped_lock lock{m_mutex};
  m_queue.enqueue(ClientMessage{
      SubscribeMsg{subuid, {topicNames.begin(), topicNames.end()}, options}});
}

template <size_t MaxValueSize, bool IsMutexed>
inline void ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClientUnsubscribe(
    int subuid) {
  std::scoped_lock lock{m_mutex};
  m_queue.enqueue(ClientMessage{UnsubscribeMsg{subuid}});
}

template <size_t MaxValueSize, bool IsMutexed>
std::span<ClientMessage>
ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ReadQueue(
    std::span<ClientMessage> out) {
  std::scoped_lock lock{m_mutex};
  size_t count = 0;
  for (auto&& msg : out) {
    if (!m_queue.try_dequeue(msg)) {
      break;
    }
    if constexpr (MaxValueSize != 0) {
      if (auto* val = std::get_if<ClientValueMsg>(&msg.contents)) {
        m_valueSize.size -= sizeof(ClientMessage) + val->value.size();
        m_valueSize.errored = false;
      }
    }
    ++count;
  }
  return out.subspan(0, count);
}

template <size_t MaxValueSize, bool IsMutexed>
void ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClearQueue() {
  std::scoped_lock lock{m_mutex};
  ClientMessage msg;
  while (m_queue.try_dequeue(msg)) {
  }
  if constexpr (MaxValueSize != 0) {
    m_valueSize.size = 0;
    m_valueSize.errored = false;
  }
}

template <size_t MaxValueSize, bool IsMutexed>
void ClientMessageQueueImpl<MaxValueSize, IsMutexed>::ClientSetValue(
    int pubuid, const Value& value) {
  std::scoped_lock lock{m_mutex};
  if constexpr (MaxValueSize != 0) {
    m_valueSize.size += sizeof(ClientMessage) + value.size();
    if (m_valueSize.size > MaxValueSize) {
      if (!m_valueSize.errored) {
        WPI_ERROR(m_logger, "NT: dropping value set due to memory limits");
        m_valueSize.errored = true;
      }
      return;  // avoid potential out of memory
    }
  }
  m_queue.enqueue(ClientMessage{ClientValueMsg{pubuid, value}});
}

}  // namespace nt::net::detail
