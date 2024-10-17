// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerClient4.h"

#include <string>

#include <wpi/timestamp.h>

#include "Log.h"
#include "net/WireDecoder.h"
#include "server/ServerStorage.h"
#include "server/ServerTopic.h"

using namespace nt::server;

ServerClient4::ServerClient4(std::string_view name, std::string_view connInfo,
                             bool local, net::WireConnection& wire,
                             SetPeriodicFunc setPeriodic,
                             ServerStorage& storage, int id,
                             wpi::Logger& logger)
    : ServerClient4Base{name,    connInfo, local, setPeriodic,
                        storage, id,       logger},
      m_wire{wire},
      m_ping{wire},
      m_incoming{logger},
      m_outgoing{wire, local} {
  // create client meta topics
  m_metaPub = storage.CreateMetaTopic(fmt::format("$clientpub${}", name));
  m_metaSub = storage.CreateMetaTopic(fmt::format("$clientsub${}", name));

  // update meta topics
  UpdateMetaClientPub();
  UpdateMetaClientSub();
}

bool ServerClient4::ProcessIncomingText(std::string_view data) {
  constexpr int kMaxImmProcessing = 10;
  bool queueWasEmpty = m_incoming.empty();
  // can't directly process, because we don't know how big it is
  WireDecodeText(data, m_incoming, m_logger);
  if (queueWasEmpty &&
      DoProcessIncomingMessages(m_incoming, kMaxImmProcessing)) {
    m_wire.StopRead();
    return true;
  }
  return false;
}

bool ServerClient4::ProcessIncomingBinary(std::span<const uint8_t> data) {
  constexpr int kMaxImmProcessing = 10;
  // if we've already queued, keep queuing
  int count = m_incoming.empty() ? 0 : kMaxImmProcessing;
  for (;;) {
    if (data.empty()) {
      break;
    }

    // decode message
    int pubuid;
    Value value;
    std::string error;
    if (!net::WireDecodeBinary(&data, &pubuid, &value, &error, 0)) {
      m_wire.Disconnect(fmt::format("binary decode error: {}", error));
      break;
    }

    // respond to RTT ping
    if (pubuid == -1) {
      auto now = wpi::Now();
      DEBUG4("RTT ping from {}, responding with time={}", m_id, now);
      m_wire.SendBinary(
          [&](auto& os) { net::WireEncodeBinary(os, -1, now, value); });
      continue;
    }

    // handle value set
    if (++count < kMaxImmProcessing) {
      ClientSetValue(pubuid, value);
    } else {
      m_incoming.ClientSetValue(pubuid, value);
    }
  }
  if (count >= kMaxImmProcessing) {
    m_wire.StopRead();
    return true;
  }
  return false;
}

void ServerClient4::SendValue(ServerTopic* topic, const Value& value,
                              net::ValueSendMode mode) {
  m_outgoing.SendValue(topic->id, value, mode);
}

void ServerClient4::SendAnnounce(ServerTopic* topic,
                                 std::optional<int> pubuid) {
  auto& sent = m_announceSent[topic];
  if (sent) {
    return;
  }
  sent = true;

  if (m_local) {
    int unsent = m_wire.WriteText([&](auto& os) {
      net::WireEncodeAnnounce(os, topic->name, topic->id, topic->typeStr,
                              topic->properties, pubuid);
    });
    if (unsent < 0) {
      return;  // error
    }
    if (unsent == 0 && m_wire.Flush() == 0) {
      return;
    }
  }
  m_outgoing.SendMessage(
      topic->id, net::AnnounceMsg{topic->name, static_cast<int>(topic->id),
                                  topic->typeStr, pubuid, topic->properties});
}

void ServerClient4::SendUnannounce(ServerTopic* topic) {
  auto& sent = m_announceSent[topic];
  if (!sent) {
    return;
  }
  sent = false;

  if (m_local) {
    int unsent = m_wire.WriteText([&](auto& os) {
      net::WireEncodeUnannounce(os, topic->name, topic->id);
    });
    if (unsent < 0) {
      return;  // error
    }
    if (unsent == 0 && m_wire.Flush() == 0) {
      return;
    }
  }
  m_outgoing.SendMessage(
      topic->id, net::UnannounceMsg{topic->name, static_cast<int>(topic->id)});
  m_outgoing.EraseId(topic->id);
}

void ServerClient4::SendPropertiesUpdate(ServerTopic* topic,
                                         const wpi::json& update, bool ack) {
  if (!m_announceSent.lookup(topic)) {
    return;
  }

  if (m_local) {
    int unsent = m_wire.WriteText([&](auto& os) {
      net::WireEncodePropertiesUpdate(os, topic->name, update, ack);
    });
    if (unsent < 0) {
      return;  // error
    }
    if (unsent == 0 && m_wire.Flush() == 0) {
      return;
    }
  }
  m_outgoing.SendMessage(topic->id,
                         net::PropertiesUpdateMsg{topic->name, update, ack});
}

void ServerClient4::SendOutgoing(uint64_t curTimeMs, bool flush) {
  if (m_wire.GetVersion() >= 0x0401) {
    if (!m_ping.Send(curTimeMs)) {
      return;
    }
  }
  m_outgoing.SendOutgoing(curTimeMs, flush);
}

void ServerClient4::UpdatePeriod(TopicClientData& tcd, ServerTopic* topic) {
  uint32_t period = net::CalculatePeriod(
      tcd.subscribers, [](auto& x) { return x->GetPeriodMs(); });
  DEBUG4("updating {} period to {} ms", topic->name, period);
  m_outgoing.SetPeriod(topic->id, period);
}
