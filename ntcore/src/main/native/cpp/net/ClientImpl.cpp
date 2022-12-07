// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ClientImpl.h"

#include <numeric>
#include <optional>
#include <string>
#include <variant>

#include <fmt/format.h>
#include <wpi/DenseMap.h>
#include <wpi/Logger.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "Handle.h"
#include "Log.h"
#include "Message.h"
#include "NetworkInterface.h"
#include "PubSubOptions.h"
#include "WireConnection.h"
#include "WireDecoder.h"
#include "WireEncoder.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;
using namespace nt::net;

static constexpr uint32_t kMinPeriodMs = 5;

// maximum number of times the wire can be not ready to send another
// transmission before we close the connection
static constexpr int kWireMaxNotReady = 10;

namespace {

struct PublisherData {
  NT_Publisher handle;
  PubSubOptions options;
  // in options as double, but copy here as integer; rounded to the nearest
  // 10 ms
  uint32_t periodMs;
  uint64_t nextSendMs{0};
  std::vector<Value> outValues;  // outgoing values
};

class CImpl : public ServerMessageHandler {
 public:
  CImpl(uint64_t curTimeMs, int inst, WireConnection& wire, wpi::Logger& logger,
        std::function<void(uint32_t repeatMs)> setPeriodic);

  void ProcessIncomingBinary(std::span<const uint8_t> data);
  void HandleLocal(std::vector<ClientMessage>&& msgs);
  bool SendControl(uint64_t curTimeMs);
  void SendValues(uint64_t curTimeMs);
  bool CheckNetworkReady();

  // ServerMessageHandler interface
  void ServerAnnounce(std::string_view name, int64_t id,
                      std::string_view typeStr, const wpi::json& properties,
                      std::optional<int64_t> pubuid) final;
  void ServerUnannounce(std::string_view name, int64_t id) final;
  void ServerPropertiesUpdate(std::string_view name, const wpi::json& update,
                              bool ack) final;

  void Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options);
  bool Unpublish(NT_Publisher pubHandle, NT_Topic topicHandle);
  void SetValue(NT_Publisher pubHandle, const Value& value);

  int m_inst;
  WireConnection& m_wire;
  wpi::Logger& m_logger;
  LocalInterface* m_local{nullptr};
  std::function<void(uint32_t repeatMs)> m_setPeriodic;

  // indexed by publisher index
  std::vector<std::unique_ptr<PublisherData>> m_publishers;

  // indexed by server-provided topic id
  wpi::DenseMap<int64_t, NT_Topic> m_topicMap;

  // timestamp handling
  static constexpr uint32_t kPingIntervalMs = 3000;
  uint64_t m_nextPingTimeMs{0};
  uint32_t m_rtt2Us{UINT32_MAX};
  bool m_haveTimeOffset{false};
  int64_t m_serverTimeOffsetUs{0};

  // periodic sweep handling
  uint32_t m_periodMs{kPingIntervalMs + 10};
  uint64_t m_lastSendMs{0};
  int m_notReadyCount{0};

  // outgoing queue
  std::vector<ClientMessage> m_outgoing;
};

}  // namespace

CImpl::CImpl(uint64_t curTimeMs, int inst, WireConnection& wire,
             wpi::Logger& logger,
             std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_inst{inst},
      m_wire{wire},
      m_logger{logger},
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

void CImpl::ProcessIncomingBinary(std::span<const uint8_t> data) {
  for (;;) {
    if (data.empty()) {
      break;
    }

    // decode message
    int64_t id;
    Value value;
    std::string error;
    if (!WireDecodeBinary(&data, &id, &value, &error, -m_serverTimeOffsetUs)) {
      ERROR("binary decode error: {}", error);
      break;  // FIXME
    }
    DEBUG4("BinaryMessage({})", id);

    // handle RTT ping response
    if (id == -1) {
      if (!value.IsInteger()) {
        WARNING("RTT ping response with non-integer type {}",
                static_cast<int>(value.type()));
        continue;
      }
      DEBUG4("RTT ping response time {} value {}", value.time(),
             value.GetInteger());
      int64_t now = wpi::Now();
      int64_t rtt2 = (now - value.GetInteger()) / 2;
      if (rtt2 < m_rtt2Us) {
        m_rtt2Us = rtt2;
        m_serverTimeOffsetUs = value.server_time() + rtt2 - now;
        DEBUG3("Time offset: {}", m_serverTimeOffsetUs);
        m_haveTimeOffset = true;
      }
      continue;
    }

    // otherwise it's a value message, get the local topic handle for it
    auto topicIt = m_topicMap.find(id);
    if (topicIt == m_topicMap.end()) {
      WARNING("received unknown id {}", id);
      continue;
    }

    // pass along to local handler
    if (m_local) {
      m_local->NetworkSetValue(topicIt->second, value);
    }
  }
}

void CImpl::HandleLocal(std::vector<ClientMessage>&& msgs) {
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

bool CImpl::SendControl(uint64_t curTimeMs) {
  DEBUG4("SendControl({})", curTimeMs);

  // rate limit sends
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return false;
  }

  // start a timestamp RTT ping if it's time to do one
  if (curTimeMs >= m_nextPingTimeMs) {
    if (!CheckNetworkReady()) {
      return false;
    }
    auto now = wpi::Now();
    DEBUG4("Sending RTT ping {}", now);
    WireEncodeBinary(m_wire.SendBinary().Add(), -1, 0, Value::MakeInteger(now));
    // drift isn't critical here, so just go from current time
    m_nextPingTimeMs = curTimeMs + kPingIntervalMs;
  }

  if (!m_outgoing.empty()) {
    if (!CheckNetworkReady()) {
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

void CImpl::SendValues(uint64_t curTimeMs) {
  DEBUG4("SendPeriodic({})", curTimeMs);

  // can't send value updates until we have a RTT
  if (!m_haveTimeOffset) {
    return;
  }

  // ensure all control messages are sent ahead of value updates
  if (!SendControl(curTimeMs)) {
    return;
  }

  // send any pending updates due to be sent
  bool checkedNetwork = false;
  auto writer = m_wire.SendBinary();
  for (auto&& pub : m_publishers) {
    if (pub && !pub->outValues.empty() && curTimeMs >= pub->nextSendMs) {
      for (auto&& val : pub->outValues) {
        if (!checkedNetwork) {
          if (!CheckNetworkReady()) {
            return;
          }
          checkedNetwork = true;
        }
        DEBUG4("Sending {} value time={} server_time={} st_off={}", pub->handle,
               val.time(), val.server_time(), m_serverTimeOffsetUs);
        int64_t time = val.time();
        if (time != 0) {
          time += m_serverTimeOffsetUs;
        }
        WireEncodeBinary(writer.Add(), Handle{pub->handle}.GetIndex(), time,
                         val);
      }
      pub->outValues.resize(0);
      pub->nextSendMs = curTimeMs + pub->periodMs;
    }
  }
}

bool CImpl::CheckNetworkReady() {
  if (!m_wire.Ready()) {
    ++m_notReadyCount;
    if (m_notReadyCount > kWireMaxNotReady) {
      m_wire.Disconnect("transmit stalled");
    }
    return false;
  }
  m_notReadyCount = 0;
  return true;
}

void CImpl::Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                    std::string_view name, std::string_view typeStr,
                    const wpi::json& properties, const PubSubOptions& options) {
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
  publisher->periodMs = std::lround(options.periodic * 100) * 10;
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

bool CImpl::Unpublish(NT_Publisher pubHandle, NT_Topic topicHandle) {
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

void CImpl::SetValue(NT_Publisher pubHandle, const Value& value) {
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

void CImpl::ServerAnnounce(std::string_view name, int64_t id,
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

void CImpl::ServerUnannounce(std::string_view name, int64_t id) {
  DEBUG4("ServerUnannounce({}, {})", name, id);
  assert(m_local);
  m_local->NetworkUnannounce(name);
  m_topicMap.erase(id);
}

void CImpl::ServerPropertiesUpdate(std::string_view name,
                                   const wpi::json& update, bool ack) {
  DEBUG4("ServerProperties({}, {}, {})", name, update.dump(), ack);
  assert(m_local);
  m_local->NetworkPropertiesUpdate(name, update, ack);
}

class ClientImpl::Impl final : public CImpl {
 public:
  Impl(uint64_t curTimeMs, int inst, WireConnection& wire, wpi::Logger& logger,
       std::function<void(uint32_t repeatMs)> setPeriodic)
      : CImpl{curTimeMs, inst, wire, logger, std::move(setPeriodic)} {}
};

ClientImpl::ClientImpl(uint64_t curTimeMs, int inst, WireConnection& wire,
                       wpi::Logger& logger,
                       std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_impl{std::make_unique<Impl>(curTimeMs, inst, wire, logger,
                                    std::move(setPeriodic))} {}

ClientImpl::~ClientImpl() = default;

void ClientImpl::ProcessIncomingText(std::string_view data) {
  if (!m_impl->m_local) {
    return;
  }
  WireDecodeText(data, *m_impl, m_impl->m_logger);
}

void ClientImpl::ProcessIncomingBinary(std::span<const uint8_t> data) {
  m_impl->ProcessIncomingBinary(data);
}

void ClientImpl::HandleLocal(std::vector<ClientMessage>&& msgs) {
  m_impl->HandleLocal(std::move(msgs));
}

void ClientImpl::SendControl(uint64_t curTimeMs) {
  m_impl->SendControl(curTimeMs);
  m_impl->m_wire.Flush();
}

void ClientImpl::SendValues(uint64_t curTimeMs) {
  m_impl->SendValues(curTimeMs);
  m_impl->m_wire.Flush();
}

void ClientImpl::SetLocal(LocalInterface* local) {
  m_impl->m_local = local;
}

ClientStartup::ClientStartup(ClientImpl& client)
    : m_client{client},
      m_binaryWriter{client.m_impl->m_wire.SendBinary()},
      m_textWriter{client.m_impl->m_wire.SendText()} {}

ClientStartup::~ClientStartup() {
  m_client.m_impl->m_wire.Flush();
}

void ClientStartup::Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                            std::string_view name, std::string_view typeStr,
                            const wpi::json& properties,
                            const PubSubOptions& options) {
  WPI_DEBUG4(m_client.m_impl->m_logger, "StartupPublish({}, {}, {}, {})",
             pubHandle, topicHandle, name, typeStr);
  m_client.m_impl->Publish(pubHandle, topicHandle, name, typeStr, properties,
                           options);
  WireEncodePublish(m_textWriter.Add(), Handle{pubHandle}.GetIndex(), name,
                    typeStr, properties);
}

void ClientStartup::Subscribe(NT_Subscriber subHandle,
                              std::span<const std::string> prefixes,
                              const PubSubOptions& options) {
  WPI_DEBUG4(m_client.m_impl->m_logger, "StartupSubscribe({})", subHandle);
  WireEncodeSubscribe(m_textWriter.Add(), Handle{subHandle}.GetIndex(),
                      prefixes, options);
}

void ClientStartup::SetValue(NT_Publisher pubHandle, const Value& value) {
  WPI_DEBUG4(m_client.m_impl->m_logger, "StartupSetValue({})", pubHandle);
  // Similar to Client::SetValue(), except always set lastValue and send
  unsigned int index = Handle{pubHandle}.GetIndex();
  assert(index < m_client.m_impl->m_publishers.size() &&
         m_client.m_impl->m_publishers[index]);
  auto& publisher = *m_client.m_impl->m_publishers[index];
  // only send time 0 values until we have a RTT
  if (value.server_time() == 0) {
    WireEncodeBinary(m_binaryWriter.Add(), index, 0, value);
  } else {
    publisher.outValues.emplace_back(value);
  }
}
