// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerClient3.h"

#include <memory>
#include <string>

#include <wpi/timestamp.h>

#include "Log.h"
#include "Types_internal.h"
#include "net3/WireEncoder3.h"
#include "server/ServerImpl.h"
#include "server/ServerPublisher.h"
#include "server/ServerTopic.h"

using namespace nt::server;

// maximum amount of time the wire can be not ready to send another
// transmission before we close the connection
static constexpr uint32_t kWireMaxNotReadyUs = 1000000;

bool ServerClient3::TopicData3::UpdateFlags(ServerTopic* topic) {
  unsigned int newFlags = topic->persistent ? NT_PERSISTENT : 0;
  bool updated = flags != newFlags;
  flags = newFlags;
  return updated;
}

bool ServerClient3::ProcessIncomingBinary(std::span<const uint8_t> data) {
  if (!m_decoder.Execute(&data)) {
    m_wire.Disconnect(m_decoder.GetError());
  }
  return false;
}

void ServerClient3::SendValue(ServerTopic* topic, const Value& value,
                              net::ValueSendMode mode) {
  if (m_state != kStateRunning) {
    if (mode == net::ValueSendMode::kImm) {
      mode = net::ValueSendMode::kAll;
    }
  } else if (m_local) {
    mode = net::ValueSendMode::kImm;  // always send local immediately
  }
  TopicData3* topic3 = GetTopic3(topic);
  bool added = false;

  switch (mode) {
    case net::ValueSendMode::kDisabled:  // do nothing
      break;
    case net::ValueSendMode::kImm:  // send immediately
      ++topic3->seqNum;
      if (topic3->sentAssign) {
        net3::WireEncodeEntryUpdate(m_wire.Send().stream(), topic->id,
                                    topic3->seqNum.value(), value);
      } else {
        net3::WireEncodeEntryAssign(m_wire.Send().stream(), topic->name,
                                    topic->id, topic3->seqNum.value(), value,
                                    topic3->flags);
        topic3->sentAssign = true;
      }
      if (m_local) {
        Flush();
      }
      break;
    case net::ValueSendMode::kNormal: {
      // replace, or append if not present
      wpi::DenseMap<NT_Topic, size_t>::iterator it;
      std::tie(it, added) =
          m_outgoingValueMap.try_emplace(topic->id, m_outgoing.size());
      if (!added && it->second < m_outgoing.size()) {
        auto& msg = m_outgoing[it->second];
        if (msg.Is(net3::Message3::kEntryUpdate) ||
            msg.Is(net3::Message3::kEntryAssign)) {
          if (msg.id() == topic->id) {  // should always be true
            msg.SetValue(value);
            break;
          }
        }
      }
    }
      // fallthrough
    case net::ValueSendMode::kAll:  // append to outgoing
      if (!added) {
        m_outgoingValueMap[topic->id] = m_outgoing.size();
      }
      ++topic3->seqNum;
      if (topic3->sentAssign) {
        m_outgoing.emplace_back(net3::Message3::EntryUpdate(
            topic->id, topic3->seqNum.value(), value));
      } else {
        m_outgoing.emplace_back(net3::Message3::EntryAssign(
            topic->name, topic->id, topic3->seqNum.value(), value,
            topic3->flags));
        topic3->sentAssign = true;
      }
      break;
  }
}

void ServerClient3::SendAnnounce(ServerTopic* topic,
                                 std::optional<int> pubuid) {
  // ignore if we've not yet built the subscriber
  if (m_subscribers.empty()) {
    return;
  }

  // subscribe to all non-special topics
  if (!topic->special) {
    topic->clients[this].AddSubscriber(m_subscribers[0].get());
    m_storage.UpdateMetaTopicSub(topic);
  }

  // NT3 requires a value to send the assign message, so the assign message
  // will get sent when the first value is sent (by SendValue).
}

void ServerClient3::SendUnannounce(ServerTopic* topic) {
  auto it = m_topics3.find(topic);
  if (it == m_topics3.end()) {
    return;  // never sent to client
  }
  bool sentAssign = it->second.sentAssign;
  m_topics3.erase(it);
  if (!sentAssign) {
    return;  // never sent to client
  }

  // map to NT3 delete message
  if (m_local && m_state == kStateRunning) {
    net3::WireEncodeEntryDelete(m_wire.Send().stream(), topic->id);
    Flush();
  } else {
    m_outgoing.emplace_back(net3::Message3::EntryDelete(topic->id));
  }
}

void ServerClient3::SendPropertiesUpdate(ServerTopic* topic,
                                         const wpi::json& update, bool ack) {
  if (ack) {
    return;  // we don't ack in NT3
  }
  auto it = m_topics3.find(topic);
  if (it == m_topics3.end()) {
    return;  // never sent to client
  }
  TopicData3* topic3 = &it->second;
  // Don't send flags update unless we've already sent an assign message.
  // The assign message will contain the updated flags when we eventually
  // send it.
  if (topic3->UpdateFlags(topic) && topic3->sentAssign) {
    if (m_local && m_state == kStateRunning) {
      net3::WireEncodeFlagsUpdate(m_wire.Send().stream(), topic->id,
                                  topic3->flags);
      Flush();
    } else {
      m_outgoing.emplace_back(
          net3::Message3::FlagsUpdate(topic->id, topic3->flags));
    }
  }
}

void ServerClient3::SendOutgoing(uint64_t curTimeMs, bool flush) {
  if (m_outgoing.empty() || m_state != kStateRunning) {
    return;  // nothing to do
  }

  // rate limit frequency of transmissions
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return;
  }

  if (!m_wire.Ready()) {
    uint64_t lastFlushTime = m_wire.GetLastFlushTime();
    uint64_t now = wpi::Now();
    if (lastFlushTime != 0 && now > (lastFlushTime + kWireMaxNotReadyUs)) {
      m_wire.Disconnect("transmit stalled");
    }
    return;
  }

  auto out = m_wire.Send();
  for (auto&& msg : m_outgoing) {
    net3::WireEncode(out.stream(), msg);
  }
  m_wire.Flush();
  m_outgoing.resize(0);
  m_outgoingValueMap.clear();
  m_lastSendMs = curTimeMs;
}

void ServerClient3::KeepAlive() {
  DEBUG4("KeepAlive({})", m_id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected KeepAlive message");
    return;
  }
  // ignore
}

void ServerClient3::ServerHelloDone() {
  DEBUG4("ServerHelloDone({})", m_id);
  m_decoder.SetError("received unexpected ServerHelloDone message");
}

void ServerClient3::ClientHelloDone() {
  DEBUG4("ClientHelloDone({})", m_id);
  if (m_state != kStateServerHelloComplete) {
    m_decoder.SetError("received unexpected ClientHelloDone message");
    return;
  }
  m_state = kStateRunning;
}

void ServerClient3::ClearEntries() {
  DEBUG4("ClearEntries({})", m_id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected ClearEntries message");
    return;
  }

  for (auto topic3it : m_topics3) {
    ServerTopic* topic = topic3it.first;

    // make sure we send assign the next time
    topic3it.second.sentAssign = false;

    // unpublish from this client (if it was previously published)
    if (topic3it.second.published) {
      topic3it.second.published = false;
      auto publisherIt = m_publishers.find(topic3it.second.pubuid);
      if (publisherIt != m_publishers.end()) {
        // remove publisher from topic
        topic->RemovePublisher(this, publisherIt->second.get());

        // remove publisher from client
        m_publishers.erase(publisherIt);

        // update meta data
        m_storage.UpdateMetaTopicPub(topic);
        UpdateMetaClientPub();
      }
    }

    // set retained=false
    m_storage.SetProperties(this, topic, {{"retained", false}});
  }
}

void ServerClient3::ProtoUnsup(unsigned int proto_rev) {
  DEBUG4("ProtoUnsup({})", m_id);
  m_decoder.SetError("received unexpected ProtoUnsup message");
}

void ServerClient3::ClientHello(std::string_view self_id,
                                unsigned int proto_rev) {
  DEBUG4("ClientHello({}, '{}', {:04x})", m_id, self_id, proto_rev);
  if (m_state != kStateInitial) {
    m_decoder.SetError("received unexpected ClientHello message");
    return;
  }
  if (proto_rev != 0x0300) {
    net3::WireEncodeProtoUnsup(m_wire.Send().stream(), 0x0300);
    Flush();
    m_decoder.SetError(
        fmt::format("unsupported protocol version {:04x}", proto_rev));
    return;
  }
  // create a unique name including client id
  m_name = fmt::format("{}-NT3@{}", self_id, m_connInfo);
  m_connected(m_name, 0x0300);
  m_connected = nullptr;  // no longer required

  // create client meta topics
  m_metaPub = m_storage.CreateMetaTopic(fmt::format("$clientpub${}", m_name));
  m_metaSub = m_storage.CreateMetaTopic(fmt::format("$clientsub${}", m_name));

  // subscribe and send initial assignments
  auto& sub = m_subscribers[0];
  std::string prefix;
  PubSubOptions options;
  options.prefixMatch = true;
  sub = std::make_unique<ServerSubscriber>(
      GetName(), std::span<const std::string>{{prefix}}, 0, options);
  m_periodMs = net::UpdatePeriodCalc(m_periodMs, sub->GetPeriodMs());
  m_setPeriodic(m_periodMs);

  {
    auto out = m_wire.Send();
    net3::WireEncodeServerHello(out.stream(), 0, "server");
    m_storage.ForEachTopic([&](ServerTopic* topic) {
      if (topic && !topic->special && topic->IsPublished() &&
          topic->lastValue) {
        DEBUG4("client {}: initial announce of '{}' (id {})", m_id, topic->name,
               topic->id);
        topic->clients[this].AddSubscriber(sub.get());
        m_storage.UpdateMetaTopicSub(topic);

        TopicData3* topic3 = GetTopic3(topic);
        ++topic3->seqNum;
        net3::WireEncodeEntryAssign(out.stream(), topic->name, topic->id,
                                    topic3->seqNum.value(), topic->lastValue,
                                    topic3->flags);
        topic3->sentAssign = true;
      }
    });
    net3::WireEncodeServerHelloDone(out.stream());
  }
  Flush();
  m_state = kStateServerHelloComplete;

  // update meta topics
  UpdateMetaClientPub();
  UpdateMetaClientSub();
}

void ServerClient3::ServerHello(unsigned int flags, std::string_view self_id) {
  DEBUG4("ServerHello({}, {}, {})", m_id, flags, self_id);
  m_decoder.SetError("received unexpected ServerHello message");
}

void ServerClient3::EntryAssign(std::string_view name, unsigned int id,
                                unsigned int seq_num, const Value& value,
                                unsigned int flags) {
  DEBUG4("EntryAssign({}, {}, {}, {}, {})", m_id, id, seq_num,
         static_cast<int>(value.type()), flags);
  if (id != 0xffff) {
    DEBUG3("ignored EntryAssign from {} with non-0xffff id {}", m_id, id);
    return;
  }

  // convert from NT3 info
  auto typeStr = TypeToString(value.type());
  wpi::json properties = wpi::json::object();
  properties["retained"] = true;  // treat all NT3 published topics as retained
  properties["cached"] = true;    // treat all NT3 published topics as cached
  if ((flags & NT_PERSISTENT) != 0) {
    properties["persistent"] = true;
  }

  // create topic
  auto topic = m_storage.CreateTopic(this, name, typeStr, properties);
  TopicData3* topic3 = GetTopic3(topic);
  if (topic3->published || topic3->sentAssign) {
    WARN("ignoring client {} duplicate publish of '{}'", m_id, name);
    return;
  }
  ++topic3->seqNum;
  topic3->published = true;
  topic3->pubuid = m_nextPubUid++;
  topic3->sentAssign = true;

  // create publisher
  auto [publisherIt, isNew] = m_publishers.try_emplace(
      topic3->pubuid,
      std::make_unique<ServerPublisher>(GetName(), topic, topic3->pubuid));
  if (!isNew) {
    return;  // shouldn't happen, but just in case...
  }

  // add publisher to topic
  topic->AddPublisher(this, publisherIt->getSecond().get());

  // update meta data
  m_storage.UpdateMetaTopicPub(topic);
  UpdateMetaClientPub();

  // acts as an announce + data update
  SendAnnounce(topic, topic3->pubuid);
  m_storage.SetValue(this, topic, value);

  // respond with assign message with assigned topic ID
  if (m_local && m_state == kStateRunning) {
    net3::WireEncodeEntryAssign(m_wire.Send().stream(), topic->name, topic->id,
                                topic3->seqNum.value(), value, topic3->flags);
  } else {
    m_outgoing.emplace_back(net3::Message3::EntryAssign(
        topic->name, topic->id, topic3->seqNum.value(), value, topic3->flags));
  }
}

void ServerClient3::EntryUpdate(unsigned int id, unsigned int seq_num,
                                const Value& value) {
  DEBUG4("EntryUpdate({}, {}, {}, {})", m_id, id, seq_num,
         static_cast<int>(value.type()));
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected EntryUpdate message");
    return;
  }

  ServerTopic* topic = m_storage.GetTopic(id);
  if (!topic || !topic->IsPublished()) {
    DEBUG3("ignored EntryUpdate from {} on non-existent topic {}", m_id, id);
    return;
  }

  TopicData3* topic3 = GetTopic3(topic);
  if (!topic3->published) {
    topic3->published = true;
    topic3->pubuid = m_nextPubUid++;

    // create publisher
    auto [publisherIt, isNew] = m_publishers.try_emplace(
        topic3->pubuid,
        std::make_unique<ServerPublisher>(GetName(), topic, topic3->pubuid));
    if (isNew) {
      // add publisher to topic
      topic->AddPublisher(this, publisherIt->getSecond().get());

      // update meta data
      m_storage.UpdateMetaTopicPub(topic);
      UpdateMetaClientPub();
    }
  }
  topic3->seqNum = net3::SequenceNumber{seq_num};

  m_storage.SetValue(this, topic, value);
}

void ServerClient3::FlagsUpdate(unsigned int id, unsigned int flags) {
  DEBUG4("FlagsUpdate({}, {}, {})", m_id, id, flags);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected FlagsUpdate message");
    return;
  }
  ServerTopic* topic = m_storage.GetTopic(id);
  if (!topic || !topic->IsPublished()) {
    DEBUG3("ignored FlagsUpdate from {} on non-existent topic {}", m_id, id);
    return;
  }
  if (topic->special) {
    DEBUG3("ignored FlagsUpdate from {} on special topic {}", m_id, id);
    return;
  }
  m_storage.SetFlags(this, topic, flags);
}

void ServerClient3::EntryDelete(unsigned int id) {
  DEBUG4("EntryDelete({}, {})", m_id, id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected EntryDelete message");
    return;
  }
  ServerTopic* topic = m_storage.GetTopic(id);
  if (!topic || !topic->IsPublished()) {
    DEBUG3("ignored EntryDelete from {} on non-existent topic {}", m_id, id);
    return;
  }
  if (topic->special) {
    DEBUG3("ignored EntryDelete from {} on special topic {}", m_id, id);
    return;
  }

  auto topic3it = m_topics3.find(topic);
  if (topic3it != m_topics3.end()) {
    // make sure we send assign the next time
    topic3it->second.sentAssign = false;

    // unpublish from this client (if it was previously published)
    if (topic3it->second.published) {
      topic3it->second.published = false;
      auto publisherIt = m_publishers.find(topic3it->second.pubuid);
      if (publisherIt != m_publishers.end()) {
        // remove publisher from topic
        topic->RemovePublisher(this, publisherIt->second.get());

        // remove publisher from client
        m_publishers.erase(publisherIt);

        // update meta data
        m_storage.UpdateMetaTopicPub(topic);
        UpdateMetaClientPub();
      }
    }
  }

  // set retained=false
  m_storage.SetProperties(this, topic, {{"retained", false}});
}
