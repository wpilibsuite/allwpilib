// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>
#include <vector>

#include <wpi/mutex.h>

#include "Message.h"
#include "MessageHandler.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {

class NetworkLoopQueue : public ClientMessageHandler {
 public:
  static constexpr size_t kInitialQueueSize = 2000;

  explicit NetworkLoopQueue(wpi::Logger& logger) : m_logger{logger} {
    m_queue.reserve(kInitialQueueSize);
  }

  void ReadQueue(std::vector<ClientMessage>* out);
  void ClearQueue();

  // ClientMessageHandler - calls to these append to the queue
  void ClientPublish(int pubuid, std::string_view name,
                     std::string_view typeStr, const wpi::json& properties,
                     const PubSubOptionsImpl& options) final;
  void ClientUnpublish(int pubuid) final;
  void ClientSetProperties(std::string_view name,
                           const wpi::json& update) final;
  void ClientSubscribe(int subuid, std::span<const std::string> topicNames,
                       const PubSubOptionsImpl& options) final;
  void ClientUnsubscribe(int subuid) final;
  void ClientSetValue(int pubuid, const Value& value) final;

 private:
  wpi::mutex m_mutex;
  std::vector<ClientMessage> m_queue;
  wpi::Logger& m_logger;
  size_t m_size{0};
  bool m_sizeErrored{false};
};

}  // namespace nt::net

#include "NetworkLoopQueue.inc"
