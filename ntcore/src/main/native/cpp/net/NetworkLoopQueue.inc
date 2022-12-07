// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <vector>

#include "NetworkLoopQueue.h"
#include "ntcore_c.h"

namespace nt::net {

inline void NetworkLoopQueue::ReadQueue(std::vector<ClientMessage>* out) {
  std::scoped_lock lock{m_mutex};
  out->swap(m_queue);
  m_queue.resize(0);
  m_queue.reserve(out->capacity());  // keep the same running capacity
  m_size = 0;
  m_sizeErrored = false;
}

inline void NetworkLoopQueue::ClearQueue() {
  std::scoped_lock lock{m_mutex};
  m_queue.resize(0);
  m_size = 0;
  m_sizeErrored = false;
}

inline void NetworkLoopQueue::Publish(NT_Publisher pubHandle,
                                      NT_Topic topicHandle,
                                      std::string_view name,
                                      std::string_view typeStr,
                                      const wpi::json& properties,
                                      const PubSubOptions& options) {
  std::scoped_lock lock{m_mutex};
  m_queue.emplace_back(
      ClientMessage{PublishMsg{pubHandle, topicHandle, std::string{name},
                               std::string{typeStr}, properties, options}});
}

inline void NetworkLoopQueue::Unpublish(NT_Publisher pubHandle,
                                        NT_Topic topicHandle) {
  std::scoped_lock lock{m_mutex};
  m_queue.emplace_back(ClientMessage{UnpublishMsg{pubHandle, topicHandle}});
}

inline void NetworkLoopQueue::SetProperties(NT_Topic topicHandle,
                                            std::string_view name,
                                            const wpi::json& update) {
  std::scoped_lock lock{m_mutex};
  m_queue.emplace_back(
      ClientMessage{SetPropertiesMsg{topicHandle, std::string{name}, update}});
}

inline void NetworkLoopQueue::Subscribe(NT_Subscriber subHandle,
                                        std::span<const std::string> topicNames,
                                        const PubSubOptions& options) {
  std::scoped_lock lock{m_mutex};
  m_queue.emplace_back(ClientMessage{SubscribeMsg{
      subHandle, {topicNames.begin(), topicNames.end()}, options}});
}

inline void NetworkLoopQueue::Unsubscribe(NT_Subscriber subHandle) {
  std::scoped_lock lock{m_mutex};
  m_queue.emplace_back(ClientMessage{UnsubscribeMsg{subHandle}});
}

}  // namespace nt::net
