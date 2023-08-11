// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ClientImpl.h"

#include <numeric>
#include <optional>
#include <string>
#include <variant>

#include <fmt/format.h>
#include <wpi/Logger.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Log.h"
#include "Message.h"
#include "NetworkInterface.h"
#include "WireConnection.h"
#include "WireEncoder.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;
using namespace nt::net;

static constexpr uint32_t kMinPeriodMs = 5;

// maximum amount of time the wire can be not ready to send another
// transmission before we close the connection
static constexpr uint32_t kWireMaxNotReadyUs = 1000000;

ClientImpl::ClientImpl(
    uint64_t curTimeMs, int inst, WireConnection& wire, wpi::Logger& logger,
    std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
        timeSyncUpdated,
    std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_inst{inst},
      m_wire{wire},
      m_logger{logger},
      m_timeSyncUpdated{std::move(timeSyncUpdated)},
      m_setPeriodic{std::move(setPeriodic)},
      m_nextPingTimeMs{curTimeMs + kPingIntervalMs} {
  // immediately send RTT ping
  auto out = m_wire.SendBinary();
  auto now = wpi::Now();
  DEBUG4("Sending initial RTT ping {}", now);
  WireEncodeBinary(out.Add(), -1, 0, Value::MakeInteger(now));
  m_wire.Flush();
  m_setPeriodic(m_periodMs);
}

void ClientImpl::ProcessIncomingBinary(uint64_t curTimeMs,
                                       std::span<const uint8_t> data) {
  for (;;) {
    if (data.empty()) {
      break;
    }

    // decode message
    int64_t id;
    Value value;
    std::string error;
    if (!WireDecodeBinary(&data, &id, &value, &error, -m_serverTimeOffsetUs)) {
      ERR("binary decode error: {}", error);
      break;  // FIXME
    }
    DEBUG4("BinaryMessage({})", id);

    // handle RTT ping response
    if (id == -1) {
      if (!value.IsInteger()) {
        WARN("RTT ping response with non-integer type {}",
             static_cast<int>(value.type()));
        continue;
      }
      DEBUG4("RTT ping response time {} value {}", value.time(),
             value.GetInteger());
      m_pongTimeMs = curTimeMs;
      int64_t now = wpi::Now();
      int64_t rtt2 = (now - value.GetInteger()) / 2;
      if (rtt2 < m_rtt2Us) {
        m_rtt2Us = rtt2;
        m_serverTimeOffsetUs = value.server_time() + rtt2 - now;
        DEBUG3("Time offset: {}", m_serverTimeOffsetUs);
        m_haveTimeOffset = true;
        m_timeSyncUpdated(m_serverTimeOffsetUs, m_rtt2Us, true);
      }
      continue;
    }

    // otherwise it's a value message, get the local topic handle for it
    auto topicIt = m_topicMap.find(id);
    if (topicIt == m_topicMap.end()) {
      WARN("received unknown id {}", id);
      continue;
    }

    // pass along to local handler
    if (m_local) {
      m_local->NetworkSetValue(topicIt->second, value);
    }
  }
}

void ClientImpl::HandleLocal(std::vector<ClientMessage>&& msgs) {
  DEBUG4("HandleLocal()");
  for (auto&& elem : msgs) {
    // common case is value
    if (auto msg = std::get_if<ClientValueMsg>(&elem.contents)) {
      SetValue(msg->pubHandle, msg->value);
      // setvalue puts on individual publish outgoing queues
    } else if (auto msg = std::get_if<PublishMsg>(&elem.contents)) {
      Publish(msg->pubHandle, msg->topicHandle, msg->name, msg->typeStr,
              msg->properties, msg->options);
      m_outgoing.emplace_back(std::move(elem));
    } else if (auto msg = std::get_if<UnpublishMsg>(&elem.contents)) {
      if (Unpublish(msg->pubHandle, msg->topicHandle)) {
        m_outgoing.emplace_back(std::move(elem));
      }
    } else {
      m_outgoing.emplace_back(std::move(elem));
    }
  }
}

bool ClientImpl::DoSendControl(uint64_t curTimeMs) {
  DEBUG4("SendControl({})", curTimeMs);

  // rate limit sends
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return false;
  }

  // start a timestamp RTT ping if it's time to do one
  if (curTimeMs >= m_nextPingTimeMs) {
    // if we didn't receive a response to our last ping, disconnect
    if (m_nextPingTimeMs != 0 && m_pongTimeMs == 0) {
      m_wire.Disconnect("timed out");
      return false;
    }

    if (!CheckNetworkReady(curTimeMs)) {
      return false;
    }
    auto now = wpi::Now();
    DEBUG4("Sending RTT ping {}", now);
    WireEncodeBinary(m_wire.SendBinary().Add(), -1, 0, Value::MakeInteger(now));
    // drift isn't critical here, so just go from current time
    m_nextPingTimeMs = curTimeMs + kPingIntervalMs;
    m_pongTimeMs = 0;
  }

  if (!m_outgoing.empty()) {
    if (!CheckNetworkReady(curTimeMs)) {
      return false;
    }
    auto writer = m_wire.SendText();
    for (auto&& msg : m_outgoing) {
      auto& stream = writer.Add();
      if (!WireEncodeText(stream, msg)) {
        // shouldn't happen, but just in case...
        stream << "{}";
      }
    }
    m_outgoing.resize(0);
  }

  m_lastSendMs = curTimeMs;
  return true;
}

void ClientImpl::DoSendValues(uint64_t curTimeMs, bool flush) {
  DEBUG4("SendValues({})", curTimeMs);

  // can't send value updates until we have a RTT
  if (!m_haveTimeOffset) {
    return;
  }

  // ensure all control messages are sent ahead of value updates
  if (!DoSendControl(curTimeMs)) {
    return;
  }

  // send any pending updates due to be sent
  bool checkedNetwork = false;
  auto writer = m_wire.SendBinary();
  for (auto&& pub : m_publishers) {
    if (pub && !pub->outValues.empty() &&
        (flush || curTimeMs >= pub->nextSendMs)) {
      for (auto&& val : pub->outValues) {
        if (!checkedNetwork) {
          if (!CheckNetworkReady(curTimeMs)) {
            return;
          }
          checkedNetwork = true;
        }
        DEBUG4("Sending {} value time={} server_time={} st_off={}", pub->handle,
               val.time(), val.server_time(), m_serverTimeOffsetUs);
        int64_t time = val.time();
        if (time != 0) {
          time += m_serverTimeOffsetUs;
          // make sure resultant time isn't exactly 0
          if (time == 0) {
            time = 1;
          }
        }
        WireEncodeBinary(writer.Add(), Handle{pub->handle}.GetIndex(), time,
                         val);
      }
      pub->outValues.resize(0);
      pub->nextSendMs = curTimeMs + pub->periodMs;
    }
  }
}

void ClientImpl::SendInitialValues() {
  DEBUG4("SendInitialValues()");

  // ensure all control messages are sent ahead of value updates
  if (!DoSendControl(0)) {
    return;
  }

  // only send time=0 values (as we don't have a RTT yet)
  auto writer = m_wire.SendBinary();
  for (auto&& pub : m_publishers) {
    if (pub && !pub->outValues.empty()) {
      bool sent = false;
      for (auto&& val : pub->outValues) {
        if (val.server_time() == 0) {
          DEBUG4("Sending {} value time={} server_time={}", pub->handle,
                 val.time(), val.server_time());
          WireEncodeBinary(writer.Add(), Handle{pub->handle}.GetIndex(), 0,
                           val);
          sent = true;
        }
      }
      if (sent) {
        std::erase_if(pub->outValues,
                      [](const auto& v) { return v.server_time() == 0; });
      }
    }
  }
}

bool ClientImpl::CheckNetworkReady(uint64_t curTimeMs) {
  if (!m_wire.Ready()) {
    uint64_t lastFlushTime = m_wire.GetLastFlushTime();
    uint64_t now = wpi::Now();
    if (lastFlushTime != 0 && now > (lastFlushTime + kWireMaxNotReadyUs)) {
      m_wire.Disconnect("transmit stalled");
    }
    return false;
  }
  return true;
}

void ClientImpl::Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                         std::string_view name, std::string_view typeStr,
                         const wpi::json& properties,
                         const PubSubOptionsImpl& options) {
  unsigned int index = Handle{pubHandle}.GetIndex();
  if (index >= m_publishers.size()) {
    m_publishers.resize(index + 1);
  }
  auto& publisher = m_publishers[index];
  if (!publisher) {
    publisher = std::make_unique<PublisherData>();
  }
  publisher->handle = pubHandle;
  publisher->options = options;
  publisher->periodMs = std::lround(options.periodicMs / 10.0) * 10;
  if (publisher->periodMs < kMinPeriodMs) {
    publisher->periodMs = kMinPeriodMs;
  }

  // update period
  m_periodMs = std::gcd(m_periodMs, publisher->periodMs);
  if (m_periodMs < kMinPeriodMs) {
    m_periodMs = kMinPeriodMs;
  }
  m_setPeriodic(m_periodMs);
}

bool ClientImpl::Unpublish(NT_Publisher pubHandle, NT_Topic topicHandle) {
  unsigned int index = Handle{pubHandle}.GetIndex();
  if (index >= m_publishers.size()) {
    return false;
  }
  bool doSend = true;
  if (m_publishers[index]) {
    // Look through outgoing queue to see if the publish hasn't been sent yet;
    // if it hasn't, delete it and don't send the server a message.
    // The outgoing queue doesn't contain values; those are deleted with the
    // publisher object.
    auto it = std::find_if(
        m_outgoing.begin(), m_outgoing.end(), [&](const auto& elem) {
          if (auto msg = std::get_if<PublishMsg>(&elem.contents)) {
            return msg->pubHandle == pubHandle;
          }
          return false;
        });
    if (it != m_outgoing.end()) {
      m_outgoing.erase(it);
      doSend = false;
    }
  }
  m_publishers[index].reset();

  // loop over all publishers to update period
  m_periodMs = kPingIntervalMs + 10;
  for (auto&& pub : m_publishers) {
    if (pub) {
      m_periodMs = std::gcd(m_periodMs, pub->periodMs);
    }
  }
  if (m_periodMs < kMinPeriodMs) {
    m_periodMs = kMinPeriodMs;
  }
  m_setPeriodic(m_periodMs);

  return doSend;
}

void ClientImpl::SetValue(NT_Publisher pubHandle, const Value& value) {
  DEBUG4("SetValue({}, time={}, server_time={}, st_off={})", pubHandle,
         value.time(), value.server_time(), m_serverTimeOffsetUs);
  unsigned int index = Handle{pubHandle}.GetIndex();
  if (index >= m_publishers.size() || !m_publishers[index]) {
    return;
  }
  auto& publisher = *m_publishers[index];
  if (publisher.outValues.empty() || publisher.options.sendAll) {
    publisher.outValues.emplace_back(value);
  } else {
    publisher.outValues.back() = value;
  }
}

void ClientImpl::ServerAnnounce(std::string_view name, int64_t id,
                                std::string_view typeStr,
                                const wpi::json& properties,
                                std::optional<int64_t> pubuid) {
  DEBUG4("ServerAnnounce({}, {}, {})", name, id, typeStr);
  assert(m_local);
  NT_Publisher pubHandle{0};
  if (pubuid) {
    pubHandle = Handle(m_inst, pubuid.value(), Handle::kPublisher);
  }
  m_topicMap[id] =
      m_local->NetworkAnnounce(name, typeStr, properties, pubHandle);
}

void ClientImpl::ServerUnannounce(std::string_view name, int64_t id) {
  DEBUG4("ServerUnannounce({}, {})", name, id);
  assert(m_local);
  m_local->NetworkUnannounce(name);
  m_topicMap.erase(id);
}

void ClientImpl::ServerPropertiesUpdate(std::string_view name,
                                        const wpi::json& update, bool ack) {
  DEBUG4("ServerProperties({}, {}, {})", name, update.dump(), ack);
  assert(m_local);
  m_local->NetworkPropertiesUpdate(name, update, ack);
}

void ClientImpl::ProcessIncomingText(std::string_view data) {
  if (!m_local) {
    return;
  }
  WireDecodeText(data, *this, m_logger);
}

void ClientImpl::SendControl(uint64_t curTimeMs) {
  DoSendControl(curTimeMs);
  m_wire.Flush();
}

void ClientImpl::SendValues(uint64_t curTimeMs, bool flush) {
  DoSendValues(curTimeMs, flush);
  m_wire.Flush();
}

void ClientImpl::SendInitial() {
  SendInitialValues();
  m_wire.Flush();
}
