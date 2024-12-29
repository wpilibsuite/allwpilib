// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string>

#include <wpi/FastQueue.h>
#include <wpi/Logger.h>
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
  std::span<ClientMessage> ReadQueue(std::span<ClientMessage> out) final {
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

  void ClearQueue() final {
    std::scoped_lock lock{m_mutex};
    ClientMessage msg;
    while (m_queue.try_dequeue(msg)) {
    }
    if constexpr (MaxValueSize != 0) {
      m_valueSize.size = 0;
      m_valueSize.errored = false;
    }
  }

  // ClientMessageHandler - calls to these append to the queue
  void ClientPublish(int pubuid, std::string_view name,
                     std::string_view typeStr, const wpi::json& properties,
                     const PubSubOptionsImpl& options) final {
    std::scoped_lock lock{m_mutex};
    m_queue.enqueue(ClientMessage{PublishMsg{
        pubuid, std::string{name}, std::string{typeStr}, properties, options}});
  }

  void ClientUnpublish(int pubuid) final {
    std::scoped_lock lock{m_mutex};
    m_queue.enqueue(ClientMessage{UnpublishMsg{pubuid}});
  }

  void ClientSetProperties(std::string_view name,
                           const wpi::json& update) final {
    std::scoped_lock lock{m_mutex};
    m_queue.enqueue(ClientMessage{SetPropertiesMsg{std::string{name}, update}});
  }

  void ClientSubscribe(int subuid, std::span<const std::string> topicNames,
                       const PubSubOptionsImpl& options) final {
    std::scoped_lock lock{m_mutex};
    m_queue.enqueue(ClientMessage{
        SubscribeMsg{subuid, {topicNames.begin(), topicNames.end()}, options}});
  }

  void ClientUnsubscribe(int subuid) final {
    std::scoped_lock lock{m_mutex};
    m_queue.enqueue(ClientMessage{UnsubscribeMsg{subuid}});
  }

  void ClientSetValue(int pubuid, const Value& value) final {
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
