// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ClientImpl.h"

#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <utility>
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

ClientImpl::ClientImpl(
    uint64_t curTimeMs, WireConnection& wire, wpi::Logger& logger,
    std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
        timeSyncUpdated,
    std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_wire{wire},
      m_logger{logger},
      m_timeSyncUpdated{std::move(timeSyncUpdated)},
      m_setPeriodic{std::move(setPeriodic)},
      m_ping{wire},
      m_nextPingTimeMs{curTimeMs + (wire.GetVersion() >= 0x0401
                                        ? NetworkPing::kPingIntervalMs
                                        : kRttIntervalMs)},
      m_outgoing{wire, false} {
  // immediately send RTT ping
  auto now = wpi::Now();
  DEBUG4("Sending initial RTT ping {}", now);
  m_wire.SendBinary(
      [&](auto& os) { WireEncodeBinary(os, -1, 0, Value::MakeInteger(now)); });
  m_setPeriodic(m_periodMs);
}

void ClientImpl::ProcessIncomingBinary(uint64_t curTimeMs,
                                       std::span<const uint8_t> data) {
  for (;;) {
    if (data.empty()) {
      break;
    }

    // decode message
    int id;
    Value value;
    std::string error;
    if (!WireDecodeBinary(&data, &id, &value, &error,
                          -m_outgoing.GetTimeOffset())) {
      ERR("binary decode error: {}", error);
      break;  // FIXME
    }
    DEBUG4("BinaryMessage({})", id);

    // handle RTT ping response (only use first one)
    if (id == -1) {
      if (!m_haveTimeOffset) {
        if (!value.IsInteger()) {
          WARN("RTT ping response with non-integer type {}",
               static_cast<int>(value.type()));
          continue;
        }
        DEBUG4("RTT ping response time {} value {}", value.time(),
               value.GetInteger());
        if (m_wire.GetVersion() < 0x0401) {
          m_pongTimeMs = curTimeMs;
        }
        int64_t now = wpi::Now();
        int64_t rtt2 = (now - value.GetInteger()) / 2;
        if (rtt2 < m_rtt2Us) {
          m_rtt2Us = rtt2;
          int64_t serverTimeOffsetUs = value.server_time() + rtt2 - now;
          DEBUG3("Time offset: {}", serverTimeOffsetUs);
          m_outgoing.SetTimeOffset(serverTimeOffsetUs);
          m_haveTimeOffset = true;
          m_timeSyncUpdated(serverTimeOffsetUs, m_rtt2Us, true);
        }
      }
      continue;
    }

    // otherwise it's a value message
    ServerSetValue(id, value);
  }
}

void ClientImpl::HandleLocal(std::span<ClientMessage> msgs) {
  DEBUG4("HandleLocal()");
  for (auto&& elem : msgs) {
    // common case is value
    if (auto msg = std::get_if<ClientValueMsg>(&elem.contents)) {
      SetValue(msg->pubuid, msg->value);
    } else if (auto msg = std::get_if<PublishMsg>(&elem.contents)) {
      Publish(msg->pubuid, msg->name, msg->typeStr, msg->properties,
              msg->options);
      m_outgoing.SendMessage(msg->pubuid, std::move(elem));
    } else if (auto msg = std::get_if<UnpublishMsg>(&elem.contents)) {
      Unpublish(msg->pubuid, std::move(elem));
    } else {
      m_outgoing.SendMessage(0, std::move(elem));
    }
  }
}

void ClientImpl::SendOutgoing(uint64_t curTimeMs, bool flush) {
  DEBUG4("SendOutgoing({}, {})", curTimeMs, flush);

  if (m_wire.GetVersion() >= 0x0401) {
    // Use WS pings
    if (!m_ping.Send(curTimeMs)) {
      return;
    }
  } else {
    // Use RTT pings; it's unsafe to use WS pings due to bugs in WS message
    // fragmentation in earlier NT4 implementations
    if (curTimeMs >= m_nextPingTimeMs) {
      // if we didn't receive a response to our last ping, disconnect
      if (m_nextPingTimeMs != 0 && m_pongTimeMs == 0) {
        m_wire.Disconnect("connection timed out");
        return;
      }

      auto now = wpi::Now();
      DEBUG4("Sending RTT ping {}", now);
      m_wire.SendBinary([&](auto& os) {
        WireEncodeBinary(os, -1, 0, Value::MakeInteger(now));
      });
      // drift isn't critical here, so just go from current time
      m_nextPingTimeMs = curTimeMs + kRttIntervalMs;
      m_pongTimeMs = 0;
    }
  }

  // wait until we have a RTT measurement before sending messages
  if (!m_haveTimeOffset) {
    return;
  }

  m_outgoing.SendOutgoing(curTimeMs, flush);
}

void ClientImpl::UpdatePeriodic() {
  if (m_periodMs < kMinPeriodMs) {
    m_periodMs = kMinPeriodMs;
  }
  if (m_periodMs > kMaxPeriodMs) {
    m_periodMs = kMaxPeriodMs;
  }
  m_setPeriodic(m_periodMs);
}

void ClientImpl::Publish(int32_t pubuid, std::string_view name,
                         std::string_view typeStr, const wpi::json& properties,
                         const PubSubOptionsImpl& options) {
  if (static_cast<uint32_t>(pubuid) >= m_publishers.size()) {
    m_publishers.resize(pubuid + 1);
  }
  auto& publisher = m_publishers[pubuid];
  if (!publisher) {
    publisher = std::make_unique<PublisherData>();
  }
  publisher->options = options;
  publisher->periodMs = std::lround(options.periodicMs / 10.0) * 10;
  if (publisher->periodMs < kMinPeriodMs) {
    publisher->periodMs = kMinPeriodMs;
  }
  m_outgoing.SetPeriod(pubuid, publisher->periodMs);

  // update period
  m_periodMs = UpdatePeriodCalc(m_periodMs, publisher->periodMs);
  UpdatePeriodic();
}

void ClientImpl::Unpublish(int32_t pubuid, ClientMessage&& msg) {
  if (static_cast<uint32_t>(pubuid) >= m_publishers.size()) {
    return;
  }
  m_publishers[pubuid].reset();

  // loop over all publishers to update period
  m_periodMs = kMaxPeriodMs;
  for (auto&& pub : m_publishers) {
    if (pub) {
      m_periodMs = std::gcd(m_periodMs, pub->periodMs);
    }
  }
  UpdatePeriodic();

  m_outgoing.SendMessage(pubuid, std::move(msg));

  // remove from outgoing handle map
  m_outgoing.EraseId(pubuid);
}

void ClientImpl::SetValue(int32_t pubuid, const Value& value) {
  DEBUG4("SetValue({}, time={}, server_time={})", pubuid, value.time(),
         value.server_time());
  if (static_cast<uint32_t>(pubuid) >= m_publishers.size() ||
      !m_publishers[pubuid]) {
    return;
  }
  auto& publisher = *m_publishers[pubuid];
  m_outgoing.SendValue(
      pubuid, value,
      publisher.options.sendAll ? ValueSendMode::kAll : ValueSendMode::kNormal);
}

int ClientImpl::ServerAnnounce(std::string_view name, int id,
                               std::string_view typeStr,
                               const wpi::json& properties,
                               std::optional<int> pubuid) {
  DEBUG4("ServerAnnounce({}, {}, {})", name, id, typeStr);
  assert(m_local);
  m_topicMap[id] =
      m_local->ServerAnnounce(name, 0, typeStr, properties, pubuid);
  return id;
}

void ClientImpl::ServerUnannounce(std::string_view name, int id) {
  DEBUG4("ServerUnannounce({}, {})", name, id);
  assert(m_local);
  m_local->ServerUnannounce(name, m_topicMap[id]);
  m_topicMap.erase(id);
}

void ClientImpl::ServerPropertiesUpdate(std::string_view name,
                                        const wpi::json& update, bool ack) {
  DEBUG4("ServerProperties({}, {}, {})", name, update.dump(), ack);
  assert(m_local);
  m_local->ServerPropertiesUpdate(name, update, ack);
}

void ClientImpl::ServerSetValue(int topicId, const Value& value) {
  // get the local topic handle for it
  auto topicIt = m_topicMap.find(topicId);
  if (topicIt == m_topicMap.end()) {
    WARN("received unknown id {}", topicId);
    return;
  }

  // pass along to local handler
  if (m_local) {
    m_local->ServerSetValue(topicIt->second, value);
  }
}

void ClientImpl::ProcessIncomingText(std::string_view data) {
  if (!m_local) {
    return;
  }
  WireDecodeText(data, *this, m_logger);
}

void ClientImpl::SendInitial() {}
