// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <algorithm>
#include <concepts>
#include <numeric>
#include <span>
#include <utility>
#include <vector>

#include <wpi/DenseMap.h>

#include "Message.h"
#include "WireConnection.h"
#include "WireEncoder.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"

namespace nt::net {

static constexpr uint32_t kMinPeriodMs = 5;

inline uint32_t UpdatePeriodCalc(uint32_t period, uint32_t aPeriod) {
  uint32_t newPeriod;
  if (period == UINT32_MAX) {
    newPeriod = aPeriod;
  } else {
    newPeriod = std::gcd(period, aPeriod);
  }
  if (newPeriod < kMinPeriodMs) {
    return kMinPeriodMs;
  }
  return newPeriod;
}

template <typename T, typename F>
uint32_t CalculatePeriod(const T& container, F&& getPeriod) {
  uint32_t period = UINT32_MAX;
  for (auto&& item : container) {
    if (period == UINT32_MAX) {
      period = getPeriod(item);
    } else {
      period = std::gcd(period, getPeriod(item));
    }
  }
  if (period < kMinPeriodMs) {
    return kMinPeriodMs;
  }
  return period;
}

template <typename MessageType>
concept NetworkMessage =
    std::same_as<typename MessageType::ValueMsg, ServerValueMsg> ||
    std::same_as<typename MessageType::ValueMsg, ClientValueMsg>;

enum class ValueSendMode { kDisabled = 0, kAll, kNormal, kImm };

template <NetworkMessage MessageType>
class NetworkOutgoingQueue {
 public:
  NetworkOutgoingQueue(WireConnection& wire, bool local)
      : m_wire{wire}, m_local{local} {
    m_queues.emplace_back(100);  // default queue is 100 ms period
  }

  void SetPeriod(int id, uint32_t periodMs);

  void EraseId(int id) { m_idMap.erase(id); }

  template <typename T>
  void SendMessage(int id, T&& msg) {
    m_queues[m_idMap[id].queueIndex].Append(id, std::forward<T>(msg));
    m_totalSize += sizeof(Message);
  }

  void SendValue(int id, const Value& value, ValueSendMode mode);

  void SendOutgoing(uint64_t curTimeMs, bool flush);

  void SetTimeOffset(int64_t offsetUs) { m_timeOffsetUs = offsetUs; }
  int64_t GetTimeOffset() const { return m_timeOffsetUs; }

 public:
  WireConnection& m_wire;

 private:
  using ValueMsg = typename MessageType::ValueMsg;

  void EncodeValue(wpi::raw_ostream& os, int id, const Value& value);

  struct Message {
    Message() = default;
    template <typename T>
    Message(T&& msg, int id) : msg{std::forward<T>(msg)}, id{id} {}

    MessageType msg;
    int id;
  };

  struct Queue {
    explicit Queue(uint32_t periodMs) : periodMs{periodMs} {}
    template <typename T>
    void Append(NT_Handle handle, T&& msg) {
      msgs.emplace_back(std::forward<T>(msg), handle);
    }
    std::vector<Message> msgs;
    uint64_t nextSendMs = 0;
    uint32_t periodMs;
  };

  std::vector<Queue> m_queues;

  struct HandleInfo {
    unsigned int queueIndex = 0;
    int valuePos = -1;  // -1 if not in queue
  };
  wpi::DenseMap<int, HandleInfo> m_idMap;
  size_t m_totalSize{0};
  uint64_t m_lastSendMs{0};
  int64_t m_timeOffsetUs{0};
  unsigned int m_lastSetPeriodQueueIndex = 0;
  unsigned int m_lastSetPeriod = 100;
  bool m_local;

  // maximum total size of outgoing queues in bytes (approximate)
  static constexpr size_t kOutgoingLimit = 1024 * 1024;
};

template <NetworkMessage MessageType>
void NetworkOutgoingQueue<MessageType>::SetPeriod(int id, uint32_t periodMs) {
  // it's quite common to set a lot of things in a row with the same period
  unsigned int queueIndex;
  if (m_lastSetPeriod == periodMs) {
    queueIndex = m_lastSetPeriodQueueIndex;
  } else {
    // find and possibly create queue for this period
    auto it =
        std::find_if(m_queues.begin(), m_queues.end(),
                     [&](const auto& q) { return q.periodMs == periodMs; });
    if (it == m_queues.end()) {
      queueIndex = m_queues.size();
      m_queues.emplace_back(periodMs);
    } else {
      queueIndex = it - m_queues.begin();
    }
    m_lastSetPeriodQueueIndex = queueIndex;
    m_lastSetPeriod = periodMs;
  }

  // map the handle to the queue
  auto [infoIt, created] = m_idMap.try_emplace(id);
  if (!created && infoIt->getSecond().queueIndex != queueIndex) {
    // need to move any items from old queue to new queue
    auto& oldMsgs = m_queues[infoIt->getSecond().queueIndex].msgs;
    auto it = std::stable_partition(oldMsgs.begin(), oldMsgs.end(),
                                    [&](const auto& e) { return e.id != id; });
    auto& newMsgs = m_queues[queueIndex].msgs;
    for (auto i = it, end = oldMsgs.end(); i != end; ++i) {
      newMsgs.emplace_back(std::move(*i));
    }
    oldMsgs.erase(it, oldMsgs.end());
  }

  infoIt->getSecond().queueIndex = queueIndex;
}

template <NetworkMessage MessageType>
void NetworkOutgoingQueue<MessageType>::SendValue(int id, const Value& value,
                                                  ValueSendMode mode) {
  if (m_local) {
    mode = ValueSendMode::kImm;  // always send local immediately
  }
  // backpressure by stopping sending all if the buffer is too full
  if (mode == ValueSendMode::kAll && m_totalSize >= kOutgoingLimit) {
    mode = ValueSendMode::kNormal;
  }
  switch (mode) {
    case ValueSendMode::kDisabled:  // do nothing
      break;
    case ValueSendMode::kImm:  // send immediately
      m_wire.SendBinary([&](auto& os) { EncodeValue(os, id, value); });
      break;
    case ValueSendMode::kAll: {  // append to outgoing
      auto& info = m_idMap[id];
      auto& queue = m_queues[info.queueIndex];
      info.valuePos = queue.msgs.size();
      queue.Append(id, ValueMsg{id, value});
      m_totalSize += sizeof(Message) + value.size();
      break;
    }
    case ValueSendMode::kNormal: {
      // replace, or append if not present
      auto& info = m_idMap[id];
      auto& queue = m_queues[info.queueIndex];
      if (info.valuePos != -1 &&
          static_cast<unsigned int>(info.valuePos) < queue.msgs.size()) {
        auto& elem = queue.msgs[info.valuePos];
        if (auto m = std::get_if<ValueMsg>(&elem.msg.contents)) {
          // double-check handle, and only replace if timestamp newer
          if (elem.id == id &&
              (m->value.time() == 0 || value.time() >= m->value.time())) {
            int delta = value.size() - m->value.size();
            m->value = value;
            m_totalSize += delta;
            return;
          }
        }
      }
      info.valuePos = queue.msgs.size();
      queue.Append(id, ValueMsg{id, value});
      m_totalSize += sizeof(Message) + value.size();
      break;
    }
  }
}

template <NetworkMessage MessageType>
void NetworkOutgoingQueue<MessageType>::SendOutgoing(uint64_t curTimeMs,
                                                     bool flush) {
  if (m_totalSize == 0) {
    return;  // nothing to do
  }

  // rate limit frequency of transmissions
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return;
  }

  if (!m_wire.Ready()) {
    return;  // don't bother, still sending the last batch
  }

  // what queues are ready to send?
  wpi::SmallVector<unsigned int, 16> queues;
  for (unsigned int i = 0; i < m_queues.size(); ++i) {
    if (!m_queues[i].msgs.empty() &&
        (flush || curTimeMs >= m_queues[i].nextSendMs)) {
      queues.emplace_back(i);
    }
  }
  if (queues.empty()) {
    return;  // nothing needs to be sent yet
  }

  // Sort transmission order by what queue has been waiting the longest time.
  // XXX: byte-weighted fair queueing might be better, but is much more complex
  // to implement.
  std::sort(queues.begin(), queues.end(), [&](const auto& a, const auto& b) {
    return m_queues[a].nextSendMs < m_queues[b].nextSendMs;
  });

  for (unsigned int queueIndex : queues) {
    auto& queue = m_queues[queueIndex];
    auto& msgs = queue.msgs;
    auto it = msgs.begin();
    auto end = msgs.end();
    int unsent = 0;
    for (; it != end && unsent == 0; ++it) {
      if (auto m = std::get_if<ValueMsg>(&it->msg.contents)) {
        unsent = m_wire.WriteBinary(
            [&](auto& os) { EncodeValue(os, it->id, m->value); });
      } else {
        unsent = m_wire.WriteText([&](auto& os) {
          if (!WireEncodeText(os, it->msg)) {
            os << "{}";
          }
        });
      }
    }
    if (unsent < 0) {
      return;  // error
    }
    if (unsent == 0) {
      // finish writing any partial buffers
      unsent = m_wire.Flush();
      if (unsent < 0) {
        return;  // error
      }
    }
    int delta = it - msgs.begin() - unsent;
    for (auto&& msg : std::span{msgs}.subspan(0, delta)) {
      if (auto m = std::get_if<ValueMsg>(&msg.msg.contents)) {
        m_totalSize -= sizeof(Message) + m->value.size();
      } else {
        m_totalSize -= sizeof(Message);
      }
    }
    msgs.erase(msgs.begin(), it - unsent);
    for (auto&& kv : m_idMap) {
      auto& info = kv.getSecond();
      if (info.queueIndex == queueIndex) {
        if (info.valuePos < delta) {
          info.valuePos = -1;
        } else {
          info.valuePos -= delta;
        }
      }
    }

    // try to stay on periodic timing, unless it's falling behind current time
    if (unsent == 0) {
      queue.nextSendMs += queue.periodMs;
      if (queue.nextSendMs < curTimeMs) {
        queue.nextSendMs = curTimeMs + queue.periodMs;
      }
    }
  }

  m_lastSendMs = curTimeMs;
}

template <NetworkMessage MessageType>
void NetworkOutgoingQueue<MessageType>::EncodeValue(wpi::raw_ostream& os,
                                                    int id,
                                                    const Value& value) {
  int64_t time = value.time();
  if constexpr (std::same_as<ValueMsg, ClientValueMsg>) {
    if (time != 0) {
      time += m_timeOffsetUs;
      // make sure resultant time isn't exactly 0
      if (time == 0) {
        time = 1;
      }
    }
  }
  WireEncodeBinary(os, id, time, value);
}

}  // namespace nt::net
