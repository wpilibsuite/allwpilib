// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerImpl.h"

#include <stdint.h>

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <fmt/ranges.h>
#include <wpi/Base64.h>
#include <wpi/MessagePack.h>
#include <wpi/SmallVector.h>
#include <wpi/SpanExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/json.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "IConnectionList.h"
#include "Log.h"
#include "NetworkInterface.h"
#include "Types_internal.h"
#include "net/Message.h"
#include "net/WireEncoder.h"
#include "net3/WireConnection3.h"
#include "net3/WireEncoder3.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"

using namespace nt;
using namespace nt::net;
using namespace mpack;

// maximum amount of time the wire can be not ready to send another
// transmission before we close the connection
static constexpr uint32_t kWireMaxNotReadyUs = 1000000;

namespace {
struct Writer : public mpack_writer_t {
  Writer() {
    mpack_writer_init(this, buf, sizeof(buf));
    mpack_writer_set_context(this, &os);
    mpack_writer_set_flush(
        this, [](mpack_writer_t* w, const char* buffer, size_t count) {
          static_cast<wpi::raw_ostream*>(w->context)->write(buffer, count);
        });
  }

  std::vector<uint8_t> bytes;
  wpi::raw_uvector_ostream os{bytes};
  char buf[128];
};
}  // namespace

static void WriteOptions(mpack_writer_t& w, const PubSubOptionsImpl& options) {
  int size =
      (options.sendAll ? 1 : 0) + (options.topicsOnly ? 1 : 0) +
      (options.periodicMs != PubSubOptionsImpl::kDefaultPeriodicMs ? 1 : 0) +
      (options.prefixMatch ? 1 : 0);
  mpack_start_map(&w, size);
  if (options.sendAll) {
    mpack_write_str(&w, "all");
    mpack_write_bool(&w, true);
  }
  if (options.topicsOnly) {
    mpack_write_str(&w, "topicsonly");
    mpack_write_bool(&w, true);
  }
  if (options.periodicMs != PubSubOptionsImpl::kDefaultPeriodicMs) {
    mpack_write_str(&w, "periodic");
    mpack_write_float(&w, options.periodicMs / 1000.0);
  }
  if (options.prefixMatch) {
    mpack_write_str(&w, "prefix");
    mpack_write_bool(&w, true);
  }
  mpack_finish_map(&w);
}

void ServerImpl::PublisherData::UpdateMeta() {
  {
    Writer w;
    mpack_start_map(&w, 2);
    mpack_write_str(&w, "uid");
    mpack_write_int(&w, pubuid);
    mpack_write_str(&w, "topic");
    mpack_write_str(&w, topic->name);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      metaClient = std::move(w.bytes);
    }
  }
  {
    Writer w;
    mpack_start_map(&w, 2);
    mpack_write_str(&w, "client");
    if (client) {
      mpack_write_str(&w, client->GetName());
    } else {
      mpack_write_str(&w, "");
    }
    mpack_write_str(&w, "pubuid");
    mpack_write_int(&w, pubuid);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      metaTopic = std::move(w.bytes);
    }
  }
}

void ServerImpl::SubscriberData::UpdateMeta() {
  {
    Writer w;
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "uid");
    mpack_write_int(&w, subuid);
    mpack_write_str(&w, "topics");
    mpack_start_array(&w, topicNames.size());
    for (auto&& name : topicNames) {
      mpack_write_str(&w, name);
    }
    mpack_finish_array(&w);
    mpack_write_str(&w, "options");
    WriteOptions(w, options);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      metaClient = std::move(w.bytes);
    }
  }
  {
    Writer w;
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "client");
    if (client) {
      mpack_write_str(&w, client->GetName());
    } else {
      mpack_write_str(&w, "");
    }
    mpack_write_str(&w, "subuid");
    mpack_write_int(&w, subuid);
    mpack_write_str(&w, "options");
    WriteOptions(w, options);
    mpack_finish_map(&w);
    if (mpack_writer_destroy(&w) == mpack_ok) {
      metaTopic = std::move(w.bytes);
    }
  }
}

void ServerImpl::ClientData::UpdateMetaClientPub() {
  if (!m_metaPub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, m_publishers.size());
  for (auto&& pub : m_publishers) {
    mpack_write_object_bytes(
        &w, reinterpret_cast<const char*>(pub.second->metaClient.data()),
        pub.second->metaClient.size());
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_server.SetValue(nullptr, m_metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerImpl::ClientData::UpdateMetaClientSub() {
  if (!m_metaSub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, m_subscribers.size());
  for (auto&& sub : m_subscribers) {
    mpack_write_object_bytes(
        &w, reinterpret_cast<const char*>(sub.second->metaClient.data()),
        sub.second->metaClient.size());
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_server.SetValue(nullptr, m_metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

std::span<ServerImpl::SubscriberData*> ServerImpl::ClientData::GetSubscribers(
    std::string_view name, bool special,
    wpi::SmallVectorImpl<SubscriberData*>& buf) {
  buf.resize(0);
  for (auto&& subPair : m_subscribers) {
    SubscriberData* subscriber = subPair.getSecond().get();
    if (subscriber->Matches(name, special)) {
      buf.emplace_back(subscriber);
    }
  }
  return {buf.data(), buf.size()};
}

void ServerImpl::ClientData4Base::ClientPublish(
    int pubuid, std::string_view name, std::string_view typeStr,
    const wpi::json& properties, const PubSubOptionsImpl& options) {
  DEBUG3("ClientPublish({}, {}, {}, {})", m_id, name, pubuid, typeStr);
  auto topic = m_server.CreateTopic(this, name, typeStr, properties);

  // create publisher
  auto [publisherIt, isNew] = m_publishers.try_emplace(
      pubuid, std::make_unique<PublisherData>(this, topic, pubuid));
  if (!isNew) {
    WARN("client {} duplicate publish of pubuid {}", m_id, pubuid);
  } else {
    // add publisher to topic
    topic->AddPublisher(this, publisherIt->getSecond().get());

    // update meta data
    m_server.UpdateMetaTopicPub(topic);
  }

  // respond with announce with pubuid to client
  DEBUG4("client {}: announce {} pubuid {}", m_id, topic->name, pubuid);
  SendAnnounce(topic, pubuid);
}

void ServerImpl::ClientData4Base::ClientUnpublish(int pubuid) {
  DEBUG3("ClientUnpublish({}, {})", m_id, pubuid);
  auto publisherIt = m_publishers.find(pubuid);
  if (publisherIt == m_publishers.end()) {
    return;  // nothing to do
  }
  auto publisher = publisherIt->getSecond().get();
  auto topic = publisher->topic;

  // remove publisher from topic
  topic->RemovePublisher(this, publisher);

  // remove publisher from client
  m_publishers.erase(publisherIt);

  // update meta data
  m_server.UpdateMetaTopicPub(topic);

  // delete topic if no longer published
  if (!topic->IsPublished()) {
    m_server.DeleteTopic(topic);
  }
}

void ServerImpl::ClientData4Base::ClientSetProperties(std::string_view name,
                                                      const wpi::json& update) {
  DEBUG4("ClientSetProperties({}, {}, {})", m_id, name, update.dump());
  auto topicIt = m_server.m_nameTopics.find(name);
  if (topicIt == m_server.m_nameTopics.end() ||
      !topicIt->second->IsPublished()) {
    WARN(
        "server ignoring SetProperties({}) from client {} on unpublished topic "
        "'{}'; publish or set a value first",
        update.dump(), m_id, name);
    return;  // nothing to do
  }
  auto topic = topicIt->second;
  if (topic->special) {
    WARN("server ignoring SetProperties({}) from client {} on meta topic '{}'",
         update.dump(), m_id, name);
    return;  // nothing to do
  }
  m_server.SetProperties(nullptr, topic, update);
}

void ServerImpl::ClientData4Base::ClientSubscribe(
    int subuid, std::span<const std::string> topicNames,
    const PubSubOptionsImpl& options) {
  DEBUG4("ClientSubscribe({}, ({}), {})", m_id, fmt::join(topicNames, ","),
         subuid);
  auto& sub = m_subscribers[subuid];
  bool replace = false;
  if (sub) {
    // replace subscription
    sub->Update(topicNames, options);
    replace = true;
  } else {
    // create
    sub = std::make_unique<SubscriberData>(this, topicNames, subuid, options);
  }

  // limit subscriber min period
  if (sub->periodMs < kMinPeriodMs) {
    sub->periodMs = kMinPeriodMs;
  }

  // update periodic sender (if not local)
  if (!m_local) {
    m_periodMs = UpdatePeriodCalc(m_periodMs, sub->periodMs);
    m_setPeriodic(m_periodMs);
  }

  // see if this immediately subscribes to any topics
  // for transmit efficiency, we want to batch announcements and values, so
  // send announcements in first loop and remember what we want to send in
  // second loop.
  std::vector<TopicData*> dataToSend;
  dataToSend.reserve(m_server.m_topics.size());
  for (auto&& topic : m_server.m_topics) {
    auto tcdIt = topic->clients.find(this);
    bool removed = tcdIt != topic->clients.end() && replace &&
                   tcdIt->second.subscribers.erase(sub.get());

    // is client already subscribed?
    bool wasSubscribed =
        tcdIt != topic->clients.end() && !tcdIt->second.subscribers.empty();
    bool wasSubscribedValue =
        wasSubscribed ? tcdIt->second.sendMode != ValueSendMode::kDisabled
                      : false;

    bool added = false;
    if (sub->Matches(topic->name, topic->special)) {
      if (tcdIt == topic->clients.end()) {
        tcdIt = topic->clients.try_emplace(this).first;
      }
      tcdIt->second.AddSubscriber(sub.get());
      added = true;
    }

    if (added ^ removed) {
      UpdatePeriod(tcdIt->second, topic.get());
      m_server.UpdateMetaTopicSub(topic.get());
    }

    // announce topic to client if not previously announced
    if (added && !removed && !wasSubscribed) {
      DEBUG4("client {}: announce {}", m_id, topic->name);
      SendAnnounce(topic.get(), std::nullopt);
    }

    // send last value
    if (added && !sub->options.topicsOnly && !wasSubscribedValue &&
        topic->lastValue) {
      dataToSend.emplace_back(topic.get());
    }
  }

  for (auto topic : dataToSend) {
    DEBUG4("send last value for {} to client {}", topic->name, m_id);
    SendValue(topic, topic->lastValue, ValueSendMode::kAll);
  }
}

void ServerImpl::ClientData4Base::ClientUnsubscribe(int subuid) {
  DEBUG3("ClientUnsubscribe({}, {})", m_id, subuid);
  auto subIt = m_subscribers.find(subuid);
  if (subIt == m_subscribers.end() || !subIt->getSecond()) {
    return;  // nothing to do
  }
  auto sub = subIt->getSecond().get();

  // remove from topics
  for (auto&& topic : m_server.m_topics) {
    auto tcdIt = topic->clients.find(this);
    if (tcdIt != topic->clients.end()) {
      if (tcdIt->second.subscribers.erase(sub)) {
        UpdatePeriod(tcdIt->second, topic.get());
        m_server.UpdateMetaTopicSub(topic.get());
      }
    }
  }

  // delete it from client (future value sets will be ignored)
  m_subscribers.erase(subIt);

  // loop over all subscribers to update period
  if (!m_local) {
    m_periodMs = CalculatePeriod(
        m_subscribers, [](auto& x) { return x.getSecond()->periodMs; });
    m_setPeriodic(m_periodMs);
  }
}

void ServerImpl::ClientData4Base::ClientSetValue(int pubuid,
                                                 const Value& value) {
  DEBUG4("ClientSetValue({}, {})", m_id, pubuid);
  auto publisherIt = m_publishers.find(pubuid);
  if (publisherIt == m_publishers.end()) {
    WARN("unrecognized client {} pubuid {}, ignoring set", m_id, pubuid);
    return;  // ignore unrecognized pubuids
  }
  auto topic = publisherIt->getSecond().get()->topic;
  m_server.SetValue(this, topic, value);
}

void ServerImpl::ClientDataLocal::SendValue(TopicData* topic,
                                            const Value& value,
                                            ValueSendMode mode) {
  if (m_server.m_local) {
    m_server.m_local->ServerSetValue(topic->localTopic, value);
  }
}

void ServerImpl::ClientDataLocal::SendAnnounce(TopicData* topic,
                                               std::optional<int> pubuid) {
  if (m_server.m_local) {
    auto& sent = m_announceSent[topic];
    if (sent) {
      return;
    }
    sent = true;

    topic->localTopic = m_server.m_local->ServerAnnounce(
        topic->name, 0, topic->typeStr, topic->properties, pubuid);
  }
}

void ServerImpl::ClientDataLocal::SendUnannounce(TopicData* topic) {
  if (m_server.m_local) {
    auto& sent = m_announceSent[topic];
    if (!sent) {
      return;
    }
    sent = false;
    m_server.m_local->ServerUnannounce(topic->name, topic->localTopic);
  }
}

void ServerImpl::ClientDataLocal::SendPropertiesUpdate(TopicData* topic,
                                                       const wpi::json& update,
                                                       bool ack) {
  if (m_server.m_local) {
    if (!m_announceSent.lookup(topic)) {
      return;
    }
    m_server.m_local->ServerPropertiesUpdate(topic->name, update, ack);
  }
}

bool ServerImpl::ClientData4Base::DoProcessIncomingMessages(
    ClientMessageQueue& queue, size_t max) {
  DEBUG4("ProcessIncomingMessage()");
  max = (std::min)(m_msgsBuf.size(), max);
  std::span<ClientMessage> msgs =
      queue.ReadQueue(wpi::take_front(std::span{m_msgsBuf}, max));

  // just map as a normal client into client=0 calls
  bool updatepub = false;
  bool updatesub = false;
  for (const auto& elem : msgs) {  // NOLINT
    // common case is value, so check that first
    if (auto msg = std::get_if<ClientValueMsg>(&elem.contents)) {
      ClientSetValue(msg->pubuid, msg->value);
    } else if (auto msg = std::get_if<PublishMsg>(&elem.contents)) {
      ClientPublish(msg->pubuid, msg->name, msg->typeStr, msg->properties,
                    msg->options);
      updatepub = true;
    } else if (auto msg = std::get_if<UnpublishMsg>(&elem.contents)) {
      ClientUnpublish(msg->pubuid);
      updatepub = true;
    } else if (auto msg = std::get_if<SetPropertiesMsg>(&elem.contents)) {
      ClientSetProperties(msg->name, msg->update);
    } else if (auto msg = std::get_if<SubscribeMsg>(&elem.contents)) {
      ClientSubscribe(msg->subuid, msg->topicNames, msg->options);
      updatesub = true;
    } else if (auto msg = std::get_if<UnsubscribeMsg>(&elem.contents)) {
      ClientUnsubscribe(msg->subuid);
      updatesub = true;
    }
  }
  if (updatepub) {
    UpdateMetaClientPub();
  }
  if (updatesub) {
    UpdateMetaClientSub();
  }

  return msgs.size() == max;  // don't know for sure, but there might be more
}

bool ServerImpl::ClientData4::ProcessIncomingText(std::string_view data) {
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

bool ServerImpl::ClientData4::ProcessIncomingBinary(
    std::span<const uint8_t> data) {
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
    if (!WireDecodeBinary(&data, &pubuid, &value, &error, 0)) {
      m_wire.Disconnect(fmt::format("binary decode error: {}", error));
      break;
    }

    // respond to RTT ping
    if (pubuid == -1) {
      auto now = wpi::Now();
      DEBUG4("RTT ping from {}, responding with time={}", m_id, now);
      m_wire.SendBinary(
          [&](auto& os) { WireEncodeBinary(os, -1, now, value); });
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

void ServerImpl::ClientData4::SendValue(TopicData* topic, const Value& value,
                                        ValueSendMode mode) {
  m_outgoing.SendValue(topic->id, value, mode);
}

void ServerImpl::ClientData4::SendAnnounce(TopicData* topic,
                                           std::optional<int> pubuid) {
  auto& sent = m_announceSent[topic];
  if (sent) {
    return;
  }
  sent = true;

  if (m_local) {
    int unsent = m_wire.WriteText([&](auto& os) {
      WireEncodeAnnounce(os, topic->name, topic->id, topic->typeStr,
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
      topic->id, AnnounceMsg{topic->name, static_cast<int>(topic->id),
                             topic->typeStr, pubuid, topic->properties});
  m_server.m_controlReady = true;
}

void ServerImpl::ClientData4::SendUnannounce(TopicData* topic) {
  auto& sent = m_announceSent[topic];
  if (!sent) {
    return;
  }
  sent = false;

  if (m_local) {
    int unsent = m_wire.WriteText(
        [&](auto& os) { WireEncodeUnannounce(os, topic->name, topic->id); });
    if (unsent < 0) {
      return;  // error
    }
    if (unsent == 0 && m_wire.Flush() == 0) {
      return;
    }
  }
  m_outgoing.SendMessage(
      topic->id, UnannounceMsg{topic->name, static_cast<int>(topic->id)});
  m_outgoing.EraseId(topic->id);
  m_server.m_controlReady = true;
}

void ServerImpl::ClientData4::SendPropertiesUpdate(TopicData* topic,
                                                   const wpi::json& update,
                                                   bool ack) {
  if (!m_announceSent.lookup(topic)) {
    return;
  }

  if (m_local) {
    int unsent = m_wire.WriteText([&](auto& os) {
      WireEncodePropertiesUpdate(os, topic->name, update, ack);
    });
    if (unsent < 0) {
      return;  // error
    }
    if (unsent == 0 && m_wire.Flush() == 0) {
      return;
    }
  }
  m_outgoing.SendMessage(topic->id,
                         PropertiesUpdateMsg{topic->name, update, ack});
  m_server.m_controlReady = true;
}

void ServerImpl::ClientData4::SendOutgoing(uint64_t curTimeMs, bool flush) {
  if (m_wire.GetVersion() >= 0x0401) {
    if (!m_ping.Send(curTimeMs)) {
      return;
    }
  }
  m_outgoing.SendOutgoing(curTimeMs, flush);
}

void ServerImpl::ClientData4::UpdatePeriod(TopicData::TopicClientData& tcd,
                                           TopicData* topic) {
  uint32_t period =
      CalculatePeriod(tcd.subscribers, [](auto& x) { return x->periodMs; });
  DEBUG4("updating {} period to {} ms", topic->name, period);
  m_outgoing.SetPeriod(topic->id, period);
}

bool ServerImpl::ClientData3::TopicData3::UpdateFlags(TopicData* topic) {
  unsigned int newFlags = topic->persistent ? NT_PERSISTENT : 0;
  bool updated = flags != newFlags;
  flags = newFlags;
  return updated;
}

bool ServerImpl::ClientData3::ProcessIncomingBinary(
    std::span<const uint8_t> data) {
  if (!m_decoder.Execute(&data)) {
    m_wire.Disconnect(m_decoder.GetError());
  }
  return false;
}

void ServerImpl::ClientData3::SendValue(TopicData* topic, const Value& value,
                                        ValueSendMode mode) {
  if (m_state != kStateRunning) {
    if (mode == ValueSendMode::kImm) {
      mode = ValueSendMode::kAll;
    }
  } else if (m_local) {
    mode = ValueSendMode::kImm;  // always send local immediately
  }
  TopicData3* topic3 = GetTopic3(topic);
  bool added = false;

  switch (mode) {
    case ValueSendMode::kDisabled:  // do nothing
      break;
    case ValueSendMode::kImm:  // send immediately
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
    case ValueSendMode::kNormal: {
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
    case ValueSendMode::kAll:  // append to outgoing
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

void ServerImpl::ClientData3::SendAnnounce(TopicData* topic,
                                           std::optional<int> pubuid) {
  // ignore if we've not yet built the subscriber
  if (m_subscribers.empty()) {
    return;
  }

  // subscribe to all non-special topics
  if (!topic->special) {
    topic->clients[this].AddSubscriber(m_subscribers[0].get());
    m_server.UpdateMetaTopicSub(topic);
  }

  // NT3 requires a value to send the assign message, so the assign message
  // will get sent when the first value is sent (by SendValue).
}

void ServerImpl::ClientData3::SendUnannounce(TopicData* topic) {
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

void ServerImpl::ClientData3::SendPropertiesUpdate(TopicData* topic,
                                                   const wpi::json& update,
                                                   bool ack) {
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

void ServerImpl::ClientData3::SendOutgoing(uint64_t curTimeMs, bool flush) {
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

void ServerImpl::ClientData3::KeepAlive() {
  DEBUG4("KeepAlive({})", m_id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected KeepAlive message");
    return;
  }
  // ignore
}

void ServerImpl::ClientData3::ServerHelloDone() {
  DEBUG4("ServerHelloDone({})", m_id);
  m_decoder.SetError("received unexpected ServerHelloDone message");
}

void ServerImpl::ClientData3::ClientHelloDone() {
  DEBUG4("ClientHelloDone({})", m_id);
  if (m_state != kStateServerHelloComplete) {
    m_decoder.SetError("received unexpected ClientHelloDone message");
    return;
  }
  m_state = kStateRunning;
}

void ServerImpl::ClientData3::ClearEntries() {
  DEBUG4("ClearEntries({})", m_id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected ClearEntries message");
    return;
  }

  for (auto topic3it : m_topics3) {
    TopicData* topic = topic3it.first;

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
        m_server.UpdateMetaTopicPub(topic);
        UpdateMetaClientPub();
      }
    }

    // set retained=false
    m_server.SetProperties(this, topic, {{"retained", false}});
  }
}

void ServerImpl::ClientData3::ProtoUnsup(unsigned int proto_rev) {
  DEBUG4("ProtoUnsup({})", m_id);
  m_decoder.SetError("received unexpected ProtoUnsup message");
}

void ServerImpl::ClientData3::ClientHello(std::string_view self_id,
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
  // create a unique name (just ignore provided client id)
  m_name = fmt::format("NT3@{}", m_connInfo);
  m_connected(m_name, 0x0300);
  m_connected = nullptr;  // no longer required

  // create client meta topics
  m_metaPub = m_server.CreateMetaTopic(fmt::format("$clientpub${}", m_name));
  m_metaSub = m_server.CreateMetaTopic(fmt::format("$clientsub${}", m_name));

  // subscribe and send initial assignments
  auto& sub = m_subscribers[0];
  std::string prefix;
  PubSubOptions options;
  options.prefixMatch = true;
  sub = std::make_unique<SubscriberData>(
      this, std::span<const std::string>{{prefix}}, 0, options);
  m_periodMs = UpdatePeriodCalc(m_periodMs, sub->periodMs);
  m_setPeriodic(m_periodMs);

  {
    auto out = m_wire.Send();
    net3::WireEncodeServerHello(out.stream(), 0, "server");
    for (auto&& topic : m_server.m_topics) {
      if (topic && !topic->special && topic->IsPublished() &&
          topic->lastValue) {
        DEBUG4("client {}: initial announce of '{}' (id {})", m_id, topic->name,
               topic->id);
        topic->clients[this].AddSubscriber(sub.get());
        m_server.UpdateMetaTopicSub(topic.get());

        TopicData3* topic3 = GetTopic3(topic.get());
        ++topic3->seqNum;
        net3::WireEncodeEntryAssign(out.stream(), topic->name, topic->id,
                                    topic3->seqNum.value(), topic->lastValue,
                                    topic3->flags);
        topic3->sentAssign = true;
      }
    }
    net3::WireEncodeServerHelloDone(out.stream());
  }
  Flush();
  m_state = kStateServerHelloComplete;

  // update meta topics
  UpdateMetaClientPub();
  UpdateMetaClientSub();
}

void ServerImpl::ClientData3::ServerHello(unsigned int flags,
                                          std::string_view self_id) {
  DEBUG4("ServerHello({}, {}, {})", m_id, flags, self_id);
  m_decoder.SetError("received unexpected ServerHello message");
}

void ServerImpl::ClientData3::EntryAssign(std::string_view name,
                                          unsigned int id, unsigned int seq_num,
                                          const Value& value,
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
  auto topic = m_server.CreateTopic(this, name, typeStr, properties);
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
      std::make_unique<PublisherData>(this, topic, topic3->pubuid));
  if (!isNew) {
    return;  // shouldn't happen, but just in case...
  }

  // add publisher to topic
  topic->AddPublisher(this, publisherIt->getSecond().get());

  // update meta data
  m_server.UpdateMetaTopicPub(topic);
  UpdateMetaClientPub();

  // acts as an announce + data update
  SendAnnounce(topic, topic3->pubuid);
  m_server.SetValue(this, topic, value);

  // respond with assign message with assigned topic ID
  if (m_local && m_state == kStateRunning) {
    net3::WireEncodeEntryAssign(m_wire.Send().stream(), topic->name, topic->id,
                                topic3->seqNum.value(), value, topic3->flags);
  } else {
    m_outgoing.emplace_back(net3::Message3::EntryAssign(
        topic->name, topic->id, topic3->seqNum.value(), value, topic3->flags));
  }
}

void ServerImpl::ClientData3::EntryUpdate(unsigned int id, unsigned int seq_num,
                                          const Value& value) {
  DEBUG4("EntryUpdate({}, {}, {}, {})", m_id, id, seq_num,
         static_cast<int>(value.type()));
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected EntryUpdate message");
    return;
  }

  if (id >= m_server.m_topics.size()) {
    DEBUG3("ignored EntryUpdate from {} on non-existent topic {}", m_id, id);
    return;
  }
  TopicData* topic = m_server.m_topics[id].get();
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
        std::make_unique<PublisherData>(this, topic, topic3->pubuid));
    if (isNew) {
      // add publisher to topic
      topic->AddPublisher(this, publisherIt->getSecond().get());

      // update meta data
      m_server.UpdateMetaTopicPub(topic);
      UpdateMetaClientPub();
    }
  }
  topic3->seqNum = net3::SequenceNumber{seq_num};

  m_server.SetValue(this, topic, value);
}

void ServerImpl::ClientData3::FlagsUpdate(unsigned int id, unsigned int flags) {
  DEBUG4("FlagsUpdate({}, {}, {})", m_id, id, flags);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected FlagsUpdate message");
    return;
  }
  if (id >= m_server.m_topics.size()) {
    DEBUG3("ignored FlagsUpdate from {} on non-existent topic {}", m_id, id);
    return;
  }
  TopicData* topic = m_server.m_topics[id].get();
  if (!topic || !topic->IsPublished()) {
    DEBUG3("ignored FlagsUpdate from {} on non-existent topic {}", m_id, id);
    return;
  }
  if (topic->special) {
    DEBUG3("ignored FlagsUpdate from {} on special topic {}", m_id, id);
    return;
  }
  m_server.SetFlags(this, topic, flags);
}

void ServerImpl::ClientData3::EntryDelete(unsigned int id) {
  DEBUG4("EntryDelete({}, {})", m_id, id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected EntryDelete message");
    return;
  }
  if (id >= m_server.m_topics.size()) {
    DEBUG3("ignored EntryDelete from {} on non-existent topic {}", m_id, id);
    return;
  }
  TopicData* topic = m_server.m_topics[id].get();
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
        m_server.UpdateMetaTopicPub(topic);
        UpdateMetaClientPub();
      }
    }
  }

  // set retained=false
  m_server.SetProperties(this, topic, {{"retained", false}});
}

bool ServerImpl::TopicData::SetProperties(const wpi::json& update) {
  if (!update.is_object()) {
    return false;
  }
  bool updated = false;
  for (auto&& elem : update.items()) {
    if (elem.value().is_null()) {
      properties.erase(elem.key());
    } else {
      properties[elem.key()] = elem.value();
    }
    updated = true;
  }
  if (updated) {
    RefreshProperties();
  }
  return updated;
}

void ServerImpl::TopicData::RefreshProperties() {
  persistent = false;
  retained = false;
  cached = true;

  auto persistentIt = properties.find("persistent");
  if (persistentIt != properties.end()) {
    if (auto val = persistentIt->get_ptr<bool*>()) {
      persistent = *val;
    }
  }

  auto retainedIt = properties.find("retained");
  if (retainedIt != properties.end()) {
    if (auto val = retainedIt->get_ptr<bool*>()) {
      retained = *val;
    }
  }

  auto cachedIt = properties.find("cached");
  if (cachedIt != properties.end()) {
    if (auto val = cachedIt->get_ptr<bool*>()) {
      cached = *val;
    }
  }

  if (!cached) {
    lastValue = {};
    lastValueClient = nullptr;
  }

  if (!cached && persistent) {
    WARN("topic {}: disabling cached property disables persistent storage",
         name);
  }
}

bool ServerImpl::TopicData::SetFlags(unsigned int flags_) {
  bool updated;
  if ((flags_ & NT_PERSISTENT) != 0) {
    updated = !persistent;
    persistent = true;
    properties["persistent"] = true;
  } else {
    updated = persistent;
    persistent = false;
    properties.erase("persistent");
  }
  if ((flags_ & NT_RETAINED) != 0) {
    updated |= !retained;
    retained = true;
    properties["retained"] = true;
  } else {
    updated |= retained;
    retained = false;
    properties.erase("retained");
  }
  if ((flags_ & NT_UNCACHED) != 0) {
    updated |= cached;
    cached = false;
    properties["cached"] = false;
    lastValue = {};
    lastValueClient = nullptr;
  } else {
    updated |= !cached;
    cached = true;
    properties.erase("cached");
  }
  if (!cached && persistent) {
    WARN("topic {}: disabling cached property disables persistent storage",
         name);
  }
  return updated;
}

bool ServerImpl::SubscriberData::Matches(std::string_view name, bool special) {
  for (auto&& topicName : topicNames) {
    if ((!options.prefixMatch && name == topicName) ||
        (options.prefixMatch && (!special || !topicName.empty()) &&
         wpi::starts_with(name, topicName))) {
      return true;
    }
  }
  return false;
}

ServerImpl::ServerImpl(wpi::Logger& logger) : m_logger{logger} {
  // local is client 0
  m_clients.emplace_back(std::make_unique<ClientDataLocal>(*this, 0, logger));
  m_localClient = static_cast<ClientDataLocal*>(m_clients.back().get());
}

std::pair<std::string, int> ServerImpl::AddClient(
    std::string_view name, std::string_view connInfo, bool local,
    WireConnection& wire, ServerImpl::SetPeriodicFunc setPeriodic) {
  if (name.empty()) {
    name = "NT4";
  }
  size_t index = m_clients.size();
  // find an empty slot
  // just do a linear search as number of clients is typically small (<10)
  for (size_t i = 0, end = index; i < end; ++i) {
    if (!m_clients[i]) {
      index = i;
      break;
    }
  }
  if (index == m_clients.size()) {
    m_clients.emplace_back();
  }

  // ensure name is unique by suffixing index
  std::string dedupName = fmt::format("{}@{}", name, index);

  auto& clientData = m_clients[index];
  clientData = std::make_unique<ClientData4>(dedupName, connInfo, local, wire,
                                             std::move(setPeriodic), *this,
                                             index, m_logger);

  // create client meta topics
  clientData->m_metaPub =
      CreateMetaTopic(fmt::format("$clientpub${}", dedupName));
  clientData->m_metaSub =
      CreateMetaTopic(fmt::format("$clientsub${}", dedupName));

  // update meta topics
  clientData->UpdateMetaClientPub();
  clientData->UpdateMetaClientSub();

  DEBUG3("AddClient('{}', '{}') -> {}", name, connInfo, index);
  return {std::move(dedupName), index};
}

int ServerImpl::AddClient3(std::string_view connInfo, bool local,
                           net3::WireConnection3& wire,
                           ServerImpl::Connected3Func connected,
                           ServerImpl::SetPeriodicFunc setPeriodic) {
  size_t index = m_clients.size();
  // find an empty slot; we can't check for duplicates until we get a hello.
  // just do a linear search as number of clients is typically small (<10)
  for (size_t i = 0, end = index; i < end; ++i) {
    if (!m_clients[i]) {
      index = i;
      break;
    }
  }
  if (index == m_clients.size()) {
    m_clients.emplace_back();
  }

  m_clients[index] = std::make_unique<ClientData3>(
      connInfo, local, wire, std::move(connected), std::move(setPeriodic),
      *this, index, m_logger);

  DEBUG3("AddClient3('{}') -> {}", connInfo, index);
  return index;
}

std::shared_ptr<void> ServerImpl::RemoveClient(int clientId) {
  DEBUG3("RemoveClient({})", clientId);
  auto& client = m_clients[clientId];

  // remove all publishers and subscribers for this client
  wpi::SmallVector<TopicData*, 16> toDelete;
  for (auto&& topic : m_topics) {
    bool pubChanged = false;
    bool subChanged = false;
    auto tcdIt = topic->clients.find(client.get());
    if (tcdIt != topic->clients.end()) {
      pubChanged = !tcdIt->second.publishers.empty();
      subChanged = !tcdIt->second.subscribers.empty();
      topic->publisherCount -= tcdIt->second.publishers.size();
      topic->clients.erase(tcdIt);
    }

    if (!topic->IsPublished()) {
      toDelete.push_back(topic.get());
    } else {
      if (pubChanged) {
        UpdateMetaTopicPub(topic.get());
      }
      if (subChanged) {
        UpdateMetaTopicSub(topic.get());
      }
    }
  }

  // delete unpublished topics
  for (auto topic : toDelete) {
    DeleteTopic(topic);
  }
  DeleteTopic(client->m_metaPub);
  DeleteTopic(client->m_metaSub);

  return std::move(client);
}

bool ServerImpl::PersistentChanged() {
  bool rv = m_persistentChanged;
  m_persistentChanged = false;
  return rv;
}

static void DumpValue(wpi::raw_ostream& os, const Value& value,
                      wpi::json::serializer& s) {
  switch (value.type()) {
    case NT_BOOLEAN:
      if (value.GetBoolean()) {
        os << "true";
      } else {
        os << "false";
      }
      break;
    case NT_DOUBLE:
      s.dump_float(value.GetDouble());
      break;
    case NT_FLOAT:
      s.dump_float(value.GetFloat());
      break;
    case NT_INTEGER:
      s.dump_integer(value.GetInteger());
      break;
    case NT_STRING:
      os << '"';
      s.dump_escaped(value.GetString(), false);
      os << '"';
      break;
    case NT_RAW:
    case NT_RPC:
      os << '"';
      wpi::Base64Encode(os, value.GetRaw());
      os << '"';
      break;
    case NT_BOOLEAN_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetBooleanArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        if (v) {
          os << "true";
        } else {
          os << "false";
        }
      }
      os << ']';
      break;
    }
    case NT_DOUBLE_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetDoubleArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        s.dump_float(v);
      }
      os << ']';
      break;
    }
    case NT_FLOAT_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetFloatArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        s.dump_float(v);
      }
      os << ']';
      break;
    }
    case NT_INTEGER_ARRAY: {
      os << '[';
      bool first = true;
      for (auto v : value.GetIntegerArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        s.dump_integer(v);
      }
      os << ']';
      break;
    }
    case NT_STRING_ARRAY: {
      os << '[';
      bool first = true;
      for (auto&& v : value.GetStringArray()) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }
        os << '"';
        s.dump_escaped(v, false);
        os << '"';
      }
      os << ']';
      break;
    }
    default:
      os << "null";
      break;
  }
}

void ServerImpl::DumpPersistent(wpi::raw_ostream& os) {
  wpi::json::serializer s{os, ' ', 16};
  os << "[\n";
  bool first = true;
  for (const auto& topic : m_topics) {
    if (!topic->persistent || !topic->lastValue) {
      continue;
    }
    if (first) {
      first = false;
    } else {
      os << ",\n";
    }
    os << "  {\n    \"name\": \"";
    s.dump_escaped(topic->name, false);
    os << "\",\n    \"type\": \"";
    s.dump_escaped(topic->typeStr, false);
    os << "\",\n    \"value\": ";
    DumpValue(os, topic->lastValue, s);
    os << ",\n    \"properties\": ";
    s.dump(topic->properties, true, false, 2, 4);
    os << "\n  }";
  }
  os << "\n]\n";
}

static std::string* ObjGetString(wpi::json::object_t& obj, std::string_view key,
                                 std::string* error) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    *error = fmt::format("no {} key", key);
    return nullptr;
  }
  auto val = it->second.get_ptr<std::string*>();
  if (!val) {
    *error = fmt::format("{} must be a string", key);
  }
  return val;
}

std::string ServerImpl::LoadPersistent(std::string_view in) {
  if (in.empty()) {
    return {};
  }

  wpi::json j;
  try {
    j = wpi::json::parse(in);
  } catch (wpi::json::parse_error& err) {
    return fmt::format("could not decode JSON: {}", err.what());
  }

  if (!j.is_array()) {
    return "expected JSON array at top level";
  }

  bool persistentChanged = m_persistentChanged;

  std::string allerrors;
  int i = -1;
  auto time = nt::Now();
  for (auto&& jitem : j) {
    ++i;
    std::string error;
    {
      auto obj = jitem.get_ptr<wpi::json::object_t*>();
      if (!obj) {
        error = "expected item to be an object";
        goto err;
      }

      // name
      auto name = ObjGetString(*obj, "name", &error);
      if (!name) {
        goto err;
      }

      // type
      auto typeStr = ObjGetString(*obj, "type", &error);
      if (!typeStr) {
        goto err;
      }

      // properties
      auto propsIt = obj->find("properties");
      if (propsIt == obj->end()) {
        error = "no properties key";
        goto err;
      }
      auto& props = propsIt->second;
      if (!props.is_object()) {
        error = "properties must be an object";
        goto err;
      }

      // check to make sure persistent property is set
      auto persistentIt = props.find("persistent");
      if (persistentIt == props.end()) {
        error = "no persistent property";
        goto err;
      }
      if (auto v = persistentIt->get_ptr<bool*>()) {
        if (!*v) {
          error = "persistent property is false";
          goto err;
        }
      } else {
        error = "persistent property is not boolean";
        goto err;
      }

      // value
      auto valueIt = obj->find("value");
      if (valueIt == obj->end()) {
        error = "no value key";
        goto err;
      }
      Value value;
      if (*typeStr == "boolean") {
        if (auto v = valueIt->second.get_ptr<bool*>()) {
          value = Value::MakeBoolean(*v, time);
        } else {
          error = "value type mismatch, expected boolean";
          goto err;
        }
      } else if (*typeStr == "int") {
        if (auto v = valueIt->second.get_ptr<int64_t*>()) {
          value = Value::MakeInteger(*v, time);
        } else if (auto v = valueIt->second.get_ptr<uint64_t*>()) {
          value = Value::MakeInteger(*v, time);
        } else {
          error = "value type mismatch, expected int";
          goto err;
        }
      } else if (*typeStr == "float") {
        if (auto v = valueIt->second.get_ptr<double*>()) {
          value = Value::MakeFloat(*v, time);
        } else {
          error = "value type mismatch, expected float";
          goto err;
        }
      } else if (*typeStr == "double") {
        if (auto v = valueIt->second.get_ptr<double*>()) {
          value = Value::MakeDouble(*v, time);
        } else {
          error = "value type mismatch, expected double";
          goto err;
        }
      } else if (*typeStr == "string" || *typeStr == "json") {
        if (auto v = valueIt->second.get_ptr<std::string*>()) {
          value = Value::MakeString(*v, time);
        } else {
          error = "value type mismatch, expected string";
          goto err;
        }
      } else if (*typeStr == "boolean[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<int> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<bool*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected boolean";
          }
        }
        value = Value::MakeBooleanArray(elems, time);
      } else if (*typeStr == "int[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<int64_t> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<int64_t*>()) {
            elems.push_back(*v);
          } else if (auto v = jelem.get_ptr<uint64_t*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected int";
          }
        }
        value = Value::MakeIntegerArray(elems, time);
      } else if (*typeStr == "double[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<double> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<double*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected double";
          }
        }
        value = Value::MakeDoubleArray(elems, time);
      } else if (*typeStr == "float[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<float> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<double*>()) {
            elems.push_back(*v);
          } else {
            error = "value type mismatch, expected float";
          }
        }
        value = Value::MakeFloatArray(elems, time);
      } else if (*typeStr == "string[]") {
        auto arr = valueIt->second.get_ptr<wpi::json::array_t*>();
        if (!arr) {
          error = "value type mismatch, expected array";
          goto err;
        }
        std::vector<std::string> elems;
        for (auto&& jelem : valueIt->second) {
          if (auto v = jelem.get_ptr<std::string*>()) {
            elems.emplace_back(*v);
          } else {
            error = "value type mismatch, expected string";
          }
        }
        value = Value::MakeStringArray(std::move(elems), time);
      } else {
        // raw
        if (auto v = valueIt->second.get_ptr<std::string*>()) {
          std::vector<uint8_t> data;
          wpi::Base64Decode(*v, &data);
          value = Value::MakeRaw(std::move(data), time);
        } else {
          error = "value type mismatch, expected string";
          goto err;
        }
      }

      // create persistent topic
      auto topic = CreateTopic(nullptr, *name, *typeStr, props);

      // set value
      SetValue(nullptr, topic, value);

      continue;
    }
  err:
    allerrors += fmt::format("{}: {}\n", i, error);
  }

  m_persistentChanged = persistentChanged;  // restore flag

  return allerrors;
}

ServerImpl::TopicData* ServerImpl::CreateTopic(ClientData* client,
                                               std::string_view name,
                                               std::string_view typeStr,
                                               const wpi::json& properties,
                                               bool special) {
  auto& topic = m_nameTopics[name];
  if (topic) {
    if (typeStr != topic->typeStr) {
      if (client) {
        WARN("client {} publish '{}' conflicting type '{}' (currently '{}')",
             client->GetName(), name, typeStr, topic->typeStr);
      }
    }
  } else {
    // new topic
    unsigned int id = m_topics.emplace_back(
        std::make_unique<TopicData>(m_logger, name, typeStr, properties));
    topic = m_topics[id].get();
    topic->id = id;
    topic->special = special;

    for (auto&& aClient : m_clients) {
      if (!aClient) {
        continue;
      }

      // look for subscriber matching prefixes
      wpi::SmallVector<SubscriberData*, 16> subscribersBuf;
      auto subscribers =
          aClient->GetSubscribers(name, topic->special, subscribersBuf);

      // don't announce to this client if no subscribers
      if (subscribers.empty()) {
        continue;
      }

      auto& tcd = topic->clients[aClient.get()];
      bool added = false;
      for (auto subscriber : subscribers) {
        if (tcd.AddSubscriber(subscriber)) {
          added = true;
        }
      }
      if (added) {
        aClient->UpdatePeriod(tcd, topic);
      }

      if (aClient.get() == client) {
        continue;  // don't announce to requesting client again
      }

      DEBUG4("client {}: announce {}", aClient->GetId(), topic->name);
      aClient->SendAnnounce(topic, std::nullopt);
    }

    // create meta topics; don't create if topic is itself a meta topic
    if (!special) {
      topic->metaPub = CreateMetaTopic(fmt::format("$pub${}", name));
      topic->metaSub = CreateMetaTopic(fmt::format("$sub${}", name));
      UpdateMetaTopicPub(topic);
      UpdateMetaTopicSub(topic);
    }
  }

  return topic;
}

ServerImpl::TopicData* ServerImpl::CreateMetaTopic(std::string_view name) {
  return CreateTopic(nullptr, name, "msgpack", {{"retained", true}}, true);
}

void ServerImpl::DeleteTopic(TopicData* topic) {
  if (!topic) {
    return;
  }

  // delete meta topics
  if (topic->metaPub) {
    DeleteTopic(topic->metaPub);
  }
  if (topic->metaSub) {
    DeleteTopic(topic->metaSub);
  }

  // unannounce to all subscribers
  for (auto&& tcd : topic->clients) {
    if (!tcd.second.subscribers.empty()) {
      tcd.first->UpdatePeriod(tcd.second, topic);
      tcd.first->SendUnannounce(topic);
    }
  }

  // erase the topic
  m_nameTopics.erase(topic->name);
  m_topics.erase(topic->id);
}

void ServerImpl::SetProperties(ClientData* client, TopicData* topic,
                               const wpi::json& update) {
  DEBUG4("SetProperties({}, {}, {})", client ? client->GetId() : -1,
         topic->name, update.dump());
  bool wasPersistent = topic->persistent;
  if (topic->SetProperties(update)) {
    // update persistentChanged flag
    if (topic->persistent != wasPersistent) {
      m_persistentChanged = true;
    }
    PropertiesChanged(client, topic, update);
  }
}

void ServerImpl::SetFlags(ClientData* client, TopicData* topic,
                          unsigned int flags) {
  bool wasPersistent = topic->persistent;
  if (topic->SetFlags(flags)) {
    // update persistentChanged flag
    if (topic->persistent != wasPersistent) {
      m_persistentChanged = true;
      wpi::json update;
      if (topic->persistent) {
        update = {{"persistent", true}};
      } else {
        update = {{"persistent", wpi::json::object()}};
      }
      PropertiesChanged(client, topic, update);
    }
  }
}

void ServerImpl::SetValue(ClientData* client, TopicData* topic,
                          const Value& value) {
  // update retained value if from same client or timestamp newer
  if (topic->cached && (!topic->lastValue || topic->lastValueClient == client ||
                        topic->lastValue.time() == 0 ||
                        value.time() >= topic->lastValue.time())) {
    DEBUG4("updating '{}' last value (time was {} is {})", topic->name,
           topic->lastValue.time(), value.time());
    topic->lastValue = value;
    topic->lastValueClient = client;

    // if persistent, update flag
    if (topic->persistent) {
      m_persistentChanged = true;
    }
  }

  for (auto&& tcd : topic->clients) {
    if (tcd.first != client &&
        tcd.second.sendMode != ValueSendMode::kDisabled) {
      tcd.first->SendValue(topic, value, tcd.second.sendMode);
    }
  }
}

void ServerImpl::UpdateMetaClients(const std::vector<ConnectionInfo>& conns) {
  Writer w;
  mpack_start_array(&w, conns.size());
  for (auto&& conn : conns) {
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "id");
    mpack_write_str(&w, conn.remote_id);
    mpack_write_str(&w, "conn");
    mpack_write_str(&w, fmt::format("{}:{}", conn.remote_ip, conn.remote_port));
    mpack_write_str(&w, "ver");
    mpack_write_u16(&w, conn.protocol_version);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, m_metaClients, Value::MakeRaw(std::move(w.bytes)));
  } else {
    DEBUG4("failed to encode $clients");
  }
}

void ServerImpl::UpdateMetaTopicPub(TopicData* topic) {
  if (!topic->metaPub) {
    return;
  }
  Writer w;
  uint32_t count = 0;
  for (auto&& tcd : topic->clients) {
    count += tcd.second.publishers.size();
  }
  mpack_start_array(&w, count);
  for (auto&& tcd : topic->clients) {
    for (auto&& pub : tcd.second.publishers) {
      mpack_write_object_bytes(
          &w, reinterpret_cast<const char*>(pub->metaTopic.data()),
          pub->metaTopic.size());
    }
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerImpl::UpdateMetaTopicSub(TopicData* topic) {
  if (!topic->metaSub) {
    return;
  }
  Writer w;
  uint32_t count = 0;
  for (auto&& tcd : topic->clients) {
    count += tcd.second.subscribers.size();
  }
  mpack_start_array(&w, count);
  for (auto&& tcd : topic->clients) {
    for (auto&& sub : tcd.second.subscribers) {
      mpack_write_object_bytes(
          &w, reinterpret_cast<const char*>(sub->metaTopic.data()),
          sub->metaTopic.size());
    }
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ServerImpl::PropertiesChanged(ClientData* client, TopicData* topic,
                                   const wpi::json& update) {
  // removing some properties can result in the topic being unpublished
  if (!topic->IsPublished()) {
    DeleteTopic(topic);
  } else {
    // send updated announcement to all subscribers
    for (auto&& tcd : topic->clients) {
      tcd.first->SendPropertiesUpdate(topic, update, tcd.first == client);
    }
  }
}

void ServerImpl::SendAllOutgoing(uint64_t curTimeMs, bool flush) {
  for (auto&& client : m_clients) {
    if (client) {
      client->SendOutgoing(curTimeMs, flush);
    }
  }
}

void ServerImpl::SendOutgoing(int clientId, uint64_t curTimeMs) {
  if (auto client = m_clients[clientId].get()) {
    client->SendOutgoing(curTimeMs, false);
  }
}

void ServerImpl::SetLocal(ServerMessageHandler* local,
                          ClientMessageQueue* queue) {
  DEBUG4("SetLocal()");
  m_local = local;
  m_localClient->SetQueue(queue);

  // create server meta topics
  m_metaClients = CreateMetaTopic("$clients");

  // create local client meta topics
  m_localClient->m_metaPub = CreateMetaTopic("$serverpub");
  m_localClient->m_metaSub = CreateMetaTopic("$serversub");

  // update meta topics
  m_localClient->UpdateMetaClientPub();
  m_localClient->UpdateMetaClientSub();
}

bool ServerImpl::ProcessIncomingText(int clientId, std::string_view data) {
  if (auto client = m_clients[clientId].get()) {
    return client->ProcessIncomingText(data);
  } else {
    return false;
  }
}

bool ServerImpl::ProcessIncomingBinary(int clientId,
                                       std::span<const uint8_t> data) {
  if (auto client = m_clients[clientId].get()) {
    return client->ProcessIncomingBinary(data);
  } else {
    return false;
  }
}

bool ServerImpl::ProcessIncomingMessages(size_t max) {
  DEBUG4("ProcessIncomingMessages({})", max);
  bool rv = false;
  for (auto&& client : m_clients) {
    if (client && client->ProcessIncomingMessages(max)) {
      rv = true;
    }
  }
  return rv;
}

bool ServerImpl::ProcessLocalMessages(size_t max) {
  DEBUG4("ProcessLocalMessages({})", max);
  return m_localClient->ProcessIncomingMessages(max);
}

void ServerImpl::ConnectionsChanged(const std::vector<ConnectionInfo>& conns) {
  UpdateMetaClients(conns);
}

std::string ServerImpl::DumpPersistent() {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  DumpPersistent(os);
  os.flush();
  return rv;
}
