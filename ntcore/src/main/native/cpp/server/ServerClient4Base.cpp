// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerClient4Base.h"

#include <memory>
#include <string>
#include <vector>

#include <fmt/ranges.h>
#include <wpi/SpanExtras.h>

#include "Log.h"
#include "server/ServerImpl.h"
#include "server/ServerPublisher.h"

using namespace nt::server;

void ServerClient4Base::ClientPublish(int pubuid, std::string_view name,
                                      std::string_view typeStr,
                                      const wpi::json& properties,
                                      const PubSubOptionsImpl& options) {
  DEBUG3("ClientPublish({}, {}, {}, {})", m_id, name, pubuid, typeStr);
  auto topic = m_storage.CreateTopic(this, name, typeStr, properties);

  // create publisher
  auto [publisherIt, isNew] = m_publishers.try_emplace(
      pubuid, std::make_unique<ServerPublisher>(GetName(), topic, pubuid));
  if (!isNew) {
    WARN("client {} duplicate publish of pubuid {}", m_id, pubuid);
  } else {
    // add publisher to topic
    topic->AddPublisher(this, publisherIt->getSecond().get());

    // update meta data
    m_storage.UpdateMetaTopicPub(topic);
  }

  // respond with announce with pubuid to client
  DEBUG4("client {}: announce {} pubuid {}", m_id, topic->name, pubuid);
  SendAnnounce(topic, pubuid);
}

void ServerClient4Base::ClientUnpublish(int pubuid) {
  DEBUG3("ClientUnpublish({}, {})", m_id, pubuid);
  auto publisherIt = m_publishers.find(pubuid);
  if (publisherIt == m_publishers.end()) {
    return;  // nothing to do
  }
  auto publisher = publisherIt->getSecond().get();
  auto topic = publisher->GetTopic();

  // remove publisher from topic
  topic->RemovePublisher(this, publisher);

  // remove publisher from client
  m_publishers.erase(publisherIt);

  // update meta data
  m_storage.UpdateMetaTopicPub(topic);

  // delete topic if no longer published
  if (!topic->IsPublished()) {
    m_storage.DeleteTopic(topic);
  }
}

void ServerClient4Base::ClientSetProperties(std::string_view name,
                                            const wpi::json& update) {
  DEBUG4("ClientSetProperties({}, {}, {})", m_id, name, update.dump());
  ServerTopic* topic = m_storage.GetTopic(name);
  if (!topic || !topic->IsPublished()) {
    WARN(
        "server ignoring SetProperties({}) from client {} on unpublished topic "
        "'{}'; publish or set a value first",
        update.dump(), m_id, name);
    return;  // nothing to do
  }
  if (topic->special) {
    WARN("server ignoring SetProperties({}) from client {} on meta topic '{}'",
         update.dump(), m_id, name);
    return;  // nothing to do
  }
  m_storage.SetProperties(nullptr, topic, update);
}

void ServerClient4Base::ClientSubscribe(int subuid,
                                        std::span<const std::string> topicNames,
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
    sub = std::make_unique<ServerSubscriber>(GetName(), topicNames, subuid,
                                             options);
  }

  // update periodic sender (if not local)
  if (!m_local) {
    m_periodMs = net::UpdatePeriodCalc(m_periodMs, sub->GetPeriodMs());
    m_setPeriodic(m_periodMs);
  }

  // see if this immediately subscribes to any topics
  // for transmit efficiency, we want to batch announcements and values, so
  // send announcements in first loop and remember what we want to send in
  // second loop.
  std::vector<ServerTopic*> dataToSend;
  dataToSend.reserve(m_storage.GetNumTopics());
  m_storage.ForEachTopic([&](ServerTopic* topic) {
    auto tcdIt = topic->clients.find(this);
    bool removed = tcdIt != topic->clients.end() && replace &&
                   tcdIt->second.subscribers.erase(sub.get());

    // is client already subscribed?
    bool wasSubscribed =
        tcdIt != topic->clients.end() && !tcdIt->second.subscribers.empty();
    bool wasSubscribedValue =
        wasSubscribed ? tcdIt->second.sendMode != net::ValueSendMode::kDisabled
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
      UpdatePeriod(tcdIt->second, topic);
      m_storage.UpdateMetaTopicSub(topic);
    }

    // announce topic to client if not previously announced
    if (added && !removed && !wasSubscribed) {
      DEBUG4("client {}: announce {}", m_id, topic->name);
      SendAnnounce(topic, std::nullopt);
    }

    // send last value
    if (added && !sub->GetOptions().topicsOnly && !wasSubscribedValue &&
        topic->lastValue) {
      dataToSend.emplace_back(topic);
    }
  });

  for (auto topic : dataToSend) {
    DEBUG4("send last value for {} to client {}", topic->name, m_id);
    SendValue(topic, topic->lastValue, net::ValueSendMode::kAll);
  }
}

void ServerClient4Base::ClientUnsubscribe(int subuid) {
  DEBUG3("ClientUnsubscribe({}, {})", m_id, subuid);
  auto subIt = m_subscribers.find(subuid);
  if (subIt == m_subscribers.end() || !subIt->getSecond()) {
    return;  // nothing to do
  }
  auto sub = subIt->getSecond().get();

  // remove from topics
  m_storage.ForEachTopic([&](ServerTopic* topic) {
    auto tcdIt = topic->clients.find(this);
    if (tcdIt != topic->clients.end()) {
      if (tcdIt->second.subscribers.erase(sub)) {
        UpdatePeriod(tcdIt->second, topic);
        m_storage.UpdateMetaTopicSub(topic);
      }
    }
  });

  // delete it from client (future value sets will be ignored)
  m_subscribers.erase(subIt);

  // loop over all subscribers to update period
  if (!m_local) {
    m_periodMs = net::CalculatePeriod(
        m_subscribers, [](auto& x) { return x.getSecond()->GetPeriodMs(); });
    m_setPeriodic(m_periodMs);
  }
}

void ServerClient4Base::ClientSetValue(int pubuid, const Value& value) {
  DEBUG4("ClientSetValue({}, {})", m_id, pubuid);
  auto publisherIt = m_publishers.find(pubuid);
  if (publisherIt == m_publishers.end()) {
    WARN("unrecognized client {} pubuid {}, ignoring set", m_id, pubuid);
    return;  // ignore unrecognized pubuids
  }
  auto topic = publisherIt->getSecond().get()->GetTopic();
  m_storage.SetValue(this, topic, value);
}

bool ServerClient4Base::DoProcessIncomingMessages(
    net::ClientMessageQueue& queue, size_t max) {
  DEBUG4("ProcessIncomingMessage()");
  max = (std::min)(m_msgsBuf.size(), max);
  std::span<net::ClientMessage> msgs =
      queue.ReadQueue(wpi::take_front(std::span{m_msgsBuf}, max));

  // just map as a normal client into client=0 calls
  bool updatepub = false;
  bool updatesub = false;
  for (const auto& elem : msgs) {  // NOLINT
    // common case is value, so check that first
    if (auto msg = std::get_if<net::ClientValueMsg>(&elem.contents)) {
      ClientSetValue(msg->pubuid, msg->value);
    } else if (auto msg = std::get_if<net::PublishMsg>(&elem.contents)) {
      ClientPublish(msg->pubuid, msg->name, msg->typeStr, msg->properties,
                    msg->options);
      updatepub = true;
    } else if (auto msg = std::get_if<net::UnpublishMsg>(&elem.contents)) {
      ClientUnpublish(msg->pubuid);
      updatepub = true;
    } else if (auto msg = std::get_if<net::SetPropertiesMsg>(&elem.contents)) {
      ClientSetProperties(msg->name, msg->update);
    } else if (auto msg = std::get_if<net::SubscribeMsg>(&elem.contents)) {
      ClientSubscribe(msg->subuid, msg->topicNames, msg->options);
      updatesub = true;
    } else if (auto msg = std::get_if<net::UnsubscribeMsg>(&elem.contents)) {
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
