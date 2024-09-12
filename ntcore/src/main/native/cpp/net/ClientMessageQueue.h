// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>

#include <wpi/FastQueue.h>
#include <wpi/mutex.h>

#include "Message.h"
#include "MessageHandler.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {

class ClientMessageQueue {
 public:
  virtual ~ClientMessageQueue() = default;

  virtual std::span<ClientMessage> ReadQueue(std::span<ClientMessage> out) = 0;
  virtual void ClearQueue() = 0;
};

namespace detail {

template <size_t MaxValueSize, bool IsMutexed>
class ClientMessageQueueImpl final : public ClientMessageHandler,
                                     public ClientMessageQueue {
 public:
  static constexpr size_t kBlockSize = 64;

  explicit ClientMessageQueueImpl(wpi::Logger& logger) : m_logger{logger} {}

  bool empty() const { return m_queue.empty(); }

  // ClientMessageQueue - calls to these read the queue
  std::span<ClientMessage> ReadQueue(std::span<ClientMessage> out) final;
  void ClearQueue() final;

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
  wpi::FastQueue<ClientMessage, kBlockSize> m_queue{kBlockSize - 1};
  wpi::Logger& m_logger;

  class NoMutex {
   public:
    void lock() {}
    void unlock() {}
  };
  [[no_unique_address]]
  std::conditional_t<IsMutexed, wpi::mutex, NoMutex> m_mutex;

  struct ValueSize {
    size_t size{0};
    bool errored{false};
  };
  struct Empty {};
  [[no_unique_address]]
  std::conditional_t<MaxValueSize != 0, ValueSize, Empty> m_valueSize;
};

}  // namespace detail

using LocalClientMessageQueue =
    detail::ClientMessageQueueImpl<2 * 1024 * 1024, true>;
using NetworkIncomingClientQueue = detail::ClientMessageQueueImpl<0, false>;

}  // namespace nt::net

#include "ClientMessageQueue.inc"
