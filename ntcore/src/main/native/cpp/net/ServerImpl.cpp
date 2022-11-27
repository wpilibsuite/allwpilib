// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerImpl.h"

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

#include <wpi/Base64.h>
#include <wpi/DenseMap.h>
#include <wpi/MessagePack.h>
#include <wpi/SmallVector.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/UidVector.h>
#include <wpi/json.h>
#include <wpi/json_serializer.h>
#include <wpi/raw_ostream.h>
#include <wpi/timestamp.h>

#include "IConnectionList.h"
#include "Log.h"
#include "Message.h"
#include "NetworkInterface.h"
#include "PubSubOptions.h"
#include "Types_internal.h"
#include "WireConnection.h"
#include "WireDecoder.h"
#include "WireEncoder.h"
#include "net3/Message3.h"
#include "net3/SequenceNumber.h"
#include "net3/WireConnection3.h"
#include "net3/WireDecoder3.h"
#include "net3/WireEncoder3.h"
#include "networktables/NetworkTableValue.h"
#include "ntcore_c.h"

using namespace nt;
using namespace nt::net;
using namespace mpack;

static constexpr uint32_t kMinPeriodMs = 5;

// maximum number of times the wire can be not ready to send another
// transmission before we close the connection
static constexpr int kWireMaxNotReady = 10;

namespace {

// Utility wrapper for making a set-like vector
template <typename T>
class VectorSet : public std::vector<T> {
 public:
  using iterator = typename std::vector<T>::iterator;
  void Add(T value) { this->push_back(value); }
  // returns true if element was present
  bool Remove(T value) {
    auto removeIt = std::remove(this->begin(), this->end(), value);
    if (removeIt == this->end()) {
      return false;
    }
    this->erase(removeIt, this->end());
    return true;
  }
};

struct PublisherData;
struct SubscriberData;
struct TopicData;
class SImpl;

class ClientData {
 public:
  ClientData(std::string_view originalName, std::string_view name,
             std::string_view connInfo, bool local,
             ServerImpl::SetPeriodicFunc setPeriodic, SImpl& server, int id,
             wpi::Logger& logger)
      : m_originalName{originalName},
        m_name{name},
        m_connInfo{connInfo},
        m_local{local},
        m_setPeriodic{std::move(setPeriodic)},
        m_server{server},
        m_id{id},
        m_logger{logger} {}
  virtual ~ClientData() = default;

  virtual void ProcessIncomingText(std::string_view data) = 0;
  virtual void ProcessIncomingBinary(std::span<const uint8_t> data) = 0;

  enum SendMode { kSendDisabled = 0, kSendAll, kSendNormal, kSendImmNoFlush };

  virtual void SendValue(TopicData* topic, const Value& value,
                         SendMode mode) = 0;
  virtual void SendAnnounce(TopicData* topic,
                            std::optional<int64_t> pubuid) = 0;
  virtual void SendUnannounce(TopicData* topic) = 0;
  virtual void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                                    bool ack) = 0;
  virtual void SendOutgoing(uint64_t curTimeMs) = 0;
  virtual void Flush() = 0;

  void UpdateMetaClientPub();
  void UpdateMetaClientSub();

  std::span<SubscriberData*> GetSubscribers(
      std::string_view name, bool special,
      wpi::SmallVectorImpl<SubscriberData*>& buf);

  std::string_view GetOriginalName() const { return m_originalName; }
  std::string_view GetName() const { return m_name; }
  int GetId() const { return m_id; }

 protected:
  std::string m_originalName;
  std::string m_name;
  std::string m_connInfo;
  bool m_local;  // local to machine
  ServerImpl::SetPeriodicFunc m_setPeriodic;
  // TODO: make this per-topic?
  uint32_t m_periodMs{UINT32_MAX};
  uint64_t m_lastSendMs{0};
  SImpl& m_server;
  int m_id;

  wpi::Logger& m_logger;

  wpi::DenseMap<int64_t, std::unique_ptr<PublisherData>> m_publishers;
  wpi::DenseMap<int64_t, std::unique_ptr<SubscriberData>> m_subscribers;

 public:
  // meta topics
  TopicData* m_metaPub = nullptr;
  TopicData* m_metaSub = nullptr;
};

class ClientData4Base : public ClientData, protected ClientMessageHandler {
 public:
  ClientData4Base(std::string_view originalName, std::string_view name,
                  std::string_view connInfo, bool local,
                  ServerImpl::SetPeriodicFunc setPeriodic, SImpl& server,
                  int id, wpi::Logger& logger)
      : ClientData{originalName, name,   connInfo, local,
                   setPeriodic,  server, id,       logger} {}

 protected:
  // ClientMessageHandler interface
  void ClientPublish(int64_t pubuid, std::string_view name,
                     std::string_view typeStr,
                     const wpi::json& properties) final;
  void ClientUnpublish(int64_t pubuid) final;
  void ClientSetProperties(std::string_view name,
                           const wpi::json& update) final;
  void ClientSubscribe(int64_t subuid, std::span<const std::string> topicNames,
                       const PubSubOptions& options) final;
  void ClientUnsubscribe(int64_t subuid) final;

  void ClientSetValue(int64_t pubuid, const Value& value);

  wpi::DenseMap<TopicData*, bool> m_announceSent;
};

class ClientDataLocal final : public ClientData4Base {
  friend class net::ServerStartup;

 public:
  ClientDataLocal(SImpl& server, int id, wpi::Logger& logger)
      : ClientData4Base{"", "", "", true, [](uint32_t) {}, server, id, logger} {
  }

  void ProcessIncomingText(std::string_view data) final {}
  void ProcessIncomingBinary(std::span<const uint8_t> data) final {}

  void SendValue(TopicData* topic, const Value& value, SendMode mode) final;
  void SendAnnounce(TopicData* topic, std::optional<int64_t> pubuid) final;
  void SendUnannounce(TopicData* topic) final;
  void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                            bool ack) final;
  void SendOutgoing(uint64_t curTimeMs) final {}
  void Flush() final {}

  void HandleLocal(std::span<const ClientMessage> msgs);
};

class ClientData4 final : public ClientData4Base {
 public:
  ClientData4(std::string_view originalName, std::string_view name,
              std::string_view connInfo, bool local, WireConnection& wire,
              ServerImpl::SetPeriodicFunc setPeriodic, SImpl& server, int id,
              wpi::Logger& logger)
      : ClientData4Base{originalName, name,   connInfo, local,
                        setPeriodic,  server, id,       logger},
        m_wire{wire} {}

  void ProcessIncomingText(std::string_view data) final;
  void ProcessIncomingBinary(std::span<const uint8_t> data) final;

  void SendValue(TopicData* topic, const Value& value, SendMode mode) final;
  void SendAnnounce(TopicData* topic, std::optional<int64_t> pubuid) final;
  void SendUnannounce(TopicData* topic) final;
  void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                            bool ack) final;
  void SendOutgoing(uint64_t curTimeMs) final;

  void Flush() final;

 public:
  WireConnection& m_wire;

 private:
  std::vector<ServerMessage> m_outgoing;
  int m_notReadyCount{0};

  bool WriteBinary(int64_t id, int64_t time, const Value& value) {
    return WireEncodeBinary(SendBinary().Add(), id, time, value);
  }

  TextWriter& SendText() {
    m_outBinary.reset();  // ensure proper interleaving of text and binary
    if (!m_outText) {
      m_outText = m_wire.SendText();
    }
    return *m_outText;
  }

  BinaryWriter& SendBinary() {
    m_outText.reset();  // ensure proper interleaving of text and binary
    if (!m_outBinary) {
      m_outBinary = m_wire.SendBinary();
    }
    return *m_outBinary;
  }

  // valid when we are actively writing to this client
  std::optional<TextWriter> m_outText;
  std::optional<BinaryWriter> m_outBinary;
};

class ClientData3 final : public ClientData, private net3::MessageHandler3 {
 public:
  ClientData3(std::string_view connInfo, bool local,
              net3::WireConnection3& wire, ServerImpl::Connected3Func connected,
              ServerImpl::SetPeriodicFunc setPeriodic, SImpl& server, int id,
              wpi::Logger& logger)
      : ClientData{"", "", connInfo, local, setPeriodic, server, id, logger},
        m_connected{std::move(connected)},
        m_wire{wire},
        m_decoder{*this} {}

  void ProcessIncomingText(std::string_view data) final {}
  void ProcessIncomingBinary(std::span<const uint8_t> data) final;

  void SendValue(TopicData* topic, const Value& value, SendMode mode) final;
  void SendAnnounce(TopicData* topic, std::optional<int64_t> pubuid) final;
  void SendUnannounce(TopicData* topic) final;
  void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                            bool ack) final;
  void SendOutgoing(uint64_t curTimeMs) final;

  void Flush() final { m_wire.Flush(); }

 private:
  // MessageHandler3 interface
  void KeepAlive() final;
  void ServerHelloDone() final;
  void ClientHelloDone() final;
  void ClearEntries() final;
  void ProtoUnsup(unsigned int proto_rev) final;
  void ClientHello(std::string_view self_id, unsigned int proto_rev) final;
  void ServerHello(unsigned int flags, std::string_view self_id) final;
  void EntryAssign(std::string_view name, unsigned int id, unsigned int seq_num,
                   const Value& value, unsigned int flags) final;
  void EntryUpdate(unsigned int id, unsigned int seq_num,
                   const Value& value) final;
  void FlagsUpdate(unsigned int id, unsigned int flags) final;
  void EntryDelete(unsigned int id) final;
  void ExecuteRpc(unsigned int id, unsigned int uid,
                  std::span<const uint8_t> params) final {}
  void RpcResponse(unsigned int id, unsigned int uid,
                   std::span<const uint8_t> result) final {}

  ServerImpl::Connected3Func m_connected;
  net3::WireConnection3& m_wire;

  enum State { kStateInitial, kStateServerHelloComplete, kStateRunning };
  State m_state{kStateInitial};
  net3::WireDecoder3 m_decoder;

  std::vector<net3::Message3> m_outgoing;
  int64_t m_nextPubUid{1};
  int m_notReadyCount{0};

  struct TopicData3 {
    explicit TopicData3(TopicData* topic) { UpdateFlags(topic); }

    unsigned int flags{0};
    net3::SequenceNumber seqNum;
    bool sentAssign{false};
    bool published{false};
    int64_t pubuid{0};

    bool UpdateFlags(TopicData* topic);
  };
  wpi::DenseMap<TopicData*, TopicData3> m_topics3;
  TopicData3* GetTopic3(TopicData* topic) {
    return &m_topics3.try_emplace(topic, topic).first->second;
  }
};

struct TopicData {
  TopicData(std::string_view name, std::string_view typeStr)
      : name{name}, typeStr{typeStr} {}
  TopicData(std::string_view name, std::string_view typeStr,
            wpi::json properties)
      : name{name}, typeStr{typeStr}, properties(std::move(properties)) {
    RefreshProperties();
  }

  bool IsPublished() const {
    return persistent || retained || !publishers.empty();
  }

  // returns true if properties changed
  bool SetProperties(const wpi::json& update);
  void RefreshProperties();
  bool SetFlags(unsigned int flags_);

  std::string name;
  unsigned int id;
  Value lastValue;
  ClientData* lastValueClient = nullptr;
  std::string typeStr;
  wpi::json properties = wpi::json::object();
  bool persistent{false};
  bool retained{false};
  bool special{false};
  NT_Topic localHandle{0};

  VectorSet<PublisherData*> publishers;
  VectorSet<SubscriberData*> subscribers;

  // meta topics
  TopicData* metaPub = nullptr;
  TopicData* metaSub = nullptr;
};

struct PublisherData {
  PublisherData(ClientData* client, TopicData* topic, int64_t pubuid)
      : client{client}, topic{topic}, pubuid{pubuid} {}

  ClientData* client;
  TopicData* topic;
  int64_t pubuid;
};

struct SubscriberData {
  SubscriberData(ClientData* client, std::span<const std::string> topicNames,
                 int64_t subuid, const PubSubOptions& options)
      : client{client},
        topicNames{topicNames.begin(), topicNames.end()},
        subuid{subuid},
        options{options},
        periodMs(std::lround(options.periodic * 100) * 10) {
    if (periodMs < kMinPeriodMs) {
      periodMs = kMinPeriodMs;
    }
  }

  void Update(std::span<const std::string> topicNames_,
              const PubSubOptions& options_) {
    topicNames = {topicNames_.begin(), topicNames_.end()};
    options = options_;
    periodMs = std::lround(options_.periodic * 100) * 10;
    if (periodMs < kMinPeriodMs) {
      periodMs = kMinPeriodMs;
    }
  }

  bool Matches(std::string_view name, bool special);

  ClientData* client;
  std::vector<std::string> topicNames;
  int64_t subuid;
  PubSubOptions options;
  // in options as double, but copy here as integer; rounded to the nearest
  // 10 ms
  uint32_t periodMs;
};

class SImpl {
 public:
  explicit SImpl(wpi::Logger& logger);

  wpi::Logger& m_logger;
  LocalInterface* m_local{nullptr};
  bool m_controlReady{false};

  ClientDataLocal* m_localClient;
  std::vector<std::unique_ptr<ClientData>> m_clients;
  wpi::UidVector<std::unique_ptr<TopicData>, 16> m_topics;
  wpi::StringMap<TopicData*> m_nameTopics;
  bool m_persistentChanged{false};

  // global meta topics (other meta topics are linked to from the specific
  // client or topic)
  TopicData* m_metaClients;

  // ServerImpl interface
  std::pair<std::string, int> AddClient(
      std::string_view name, std::string_view connInfo, bool local,
      WireConnection& wire, ServerImpl::SetPeriodicFunc setPeriodic);
  int AddClient3(std::string_view connInfo, bool local,
                 net3::WireConnection3& wire,
                 ServerImpl::Connected3Func connected,
                 ServerImpl::SetPeriodicFunc setPeriodic);
  void RemoveClient(int clientId);

  bool PersistentChanged();
  void DumpPersistent(wpi::raw_ostream& os);
  std::string LoadPersistent(std::string_view in);

  // helper functions
  TopicData* CreateTopic(ClientData* client, std::string_view name,
                         std::string_view typeStr, const wpi::json& properties,
                         bool special = false);
  TopicData* CreateMetaTopic(std::string_view name);
  void DeleteTopic(TopicData* topic);
  void SetProperties(ClientData* client, TopicData* topic,
                     const wpi::json& update);
  void SetFlags(ClientData* client, TopicData* topic, unsigned int flags);
  void SetValue(ClientData* client, TopicData* topic, const Value& value);

  // update meta topic values from data structures
  void UpdateMetaClients(const std::vector<ConnectionInfo>& conns);
  void UpdateMetaTopicPub(TopicData* topic);
  void UpdateMetaTopicSub(TopicData* topic);

 private:
  void PropertiesChanged(ClientData* client, TopicData* topic,
                         const wpi::json& update);
};

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

static void WriteOptions(mpack_writer_t& w, const PubSubOptions& options) {
  int size = (options.sendAll ? 1 : 0) + (options.topicsOnly ? 1 : 0) +
             (options.periodic != 0.1 ? 1 : 0) + (options.prefixMatch ? 1 : 0);
  mpack_start_map(&w, size);
  if (options.sendAll) {
    mpack_write_str(&w, "all");
    mpack_write_bool(&w, true);
  }
  if (options.topicsOnly) {
    mpack_write_str(&w, "topicsonly");
    mpack_write_bool(&w, true);
  }
  if (options.periodic != 0.1) {
    mpack_write_str(&w, "periodic");
    mpack_write_float(&w, options.periodic);
  }
  if (options.prefixMatch) {
    mpack_write_str(&w, "prefix");
    mpack_write_bool(&w, true);
  }
  mpack_finish_map(&w);
}

void ClientData::UpdateMetaClientPub() {
  if (!m_metaPub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, m_publishers.size());
  for (auto&& pub : m_publishers) {
    mpack_start_map(&w, 2);
    mpack_write_str(&w, "uid");
    mpack_write_int(&w, pub.first);
    mpack_write_str(&w, "topic");
    mpack_write_str(&w, pub.second->topic->name);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_server.SetValue(nullptr, m_metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void ClientData::UpdateMetaClientSub() {
  if (!m_metaSub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, m_subscribers.size());
  for (auto&& sub : m_subscribers) {
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "uid");
    mpack_write_int(&w, sub.first);
    mpack_write_str(&w, "topics");
    mpack_start_array(&w, sub.second->topicNames.size());
    for (auto&& name : sub.second->topicNames) {
      mpack_write_str(&w, name);
    }
    mpack_finish_array(&w);
    mpack_write_str(&w, "options");
    WriteOptions(w, sub.second->options);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_server.SetValue(nullptr, m_metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

std::span<SubscriberData*> ClientData::GetSubscribers(
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

void ClientData4Base::ClientPublish(int64_t pubuid, std::string_view name,
                                    std::string_view typeStr,
                                    const wpi::json& properties) {
  DEBUG3("ClientPublish({}, {}, {}, {})", m_id, name, pubuid, typeStr);
  auto topic = m_server.CreateTopic(this, name, typeStr, properties);

  // create publisher
  auto [publisherIt, isNew] = m_publishers.try_emplace(
      pubuid, std::make_unique<PublisherData>(this, topic, pubuid));
  if (!isNew) {
    WARNING("client {} duplicate publish of pubuid {}", m_id, pubuid);
  }

  // add publisher to topic
  topic->publishers.Add(publisherIt->getSecond().get());

  // update meta data
  m_server.UpdateMetaTopicPub(topic);
  UpdateMetaClientPub();

  // respond with announce with pubuid to client
  DEBUG4("client {}: announce {} pubuid {}", m_id, topic->name, pubuid);
  SendAnnounce(topic, pubuid);
}

void ClientData4Base::ClientUnpublish(int64_t pubuid) {
  DEBUG3("ClientUnpublish({}, {})", m_id, pubuid);
  auto publisherIt = m_publishers.find(pubuid);
  if (publisherIt == m_publishers.end()) {
    return;  // nothing to do
  }
  auto publisher = publisherIt->getSecond().get();
  auto topic = publisher->topic;

  // remove publisher from topic
  topic->publishers.Remove(publisher);

  // remove publisher from client
  m_publishers.erase(publisherIt);

  // update meta data
  m_server.UpdateMetaTopicPub(topic);
  UpdateMetaClientPub();

  // delete topic if no longer published
  if (!topic->IsPublished()) {
    m_server.DeleteTopic(topic);
  }
}

void ClientData4Base::ClientSetProperties(std::string_view name,
                                          const wpi::json& update) {
  DEBUG4("ClientSetProperties({}, {}, {})", m_id, name, update.dump());
  auto topicIt = m_server.m_nameTopics.find(name);
  if (topicIt == m_server.m_nameTopics.end() ||
      !topicIt->second->IsPublished()) {
    DEBUG3("ignored SetProperties from {} on non-existent topic '{}'", m_id,
           name);
    return;  // nothing to do
  }
  auto topic = topicIt->second;
  if (topic->special) {
    DEBUG3("ignored SetProperties from {} on meta topic '{}'", m_id, name);
    return;  // nothing to do
  }
  m_server.SetProperties(nullptr, topic, update);
}

void ClientData4Base::ClientSubscribe(int64_t subuid,
                                      std::span<const std::string> topicNames,
                                      const PubSubOptions& options) {
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
    if (m_periodMs == UINT32_MAX) {
      m_periodMs = sub->periodMs;
    } else {
      m_periodMs = std::gcd(m_periodMs, sub->periodMs);
    }
    if (m_periodMs < kMinPeriodMs) {
      m_periodMs = kMinPeriodMs;
    }
    m_setPeriodic(m_periodMs);
  }

  // see if this immediately subscribes to any topics
  for (auto&& topic : m_server.m_topics) {
    bool removed = false;
    if (replace) {
      removed = topic->subscribers.Remove(sub.get());
    }

    // is client already subscribed?
    bool wasSubscribed = false;
    for (auto subscriber : topic->subscribers) {
      if (subscriber->client == this) {
        wasSubscribed = true;
        break;
      }
    }

    bool added = false;
    if (sub->Matches(topic->name, topic->special)) {
      topic->subscribers.Add(sub.get());
      added = true;
    }

    if (added ^ removed) {
      m_server.UpdateMetaTopicSub(topic.get());
    }

    if (!wasSubscribed && added && !removed) {
      // announce topic to client
      DEBUG4("client {}: announce {}", m_id, topic->name);
      SendAnnounce(topic.get(), std::nullopt);

      // send last value
      if (!sub->options.topicsOnly && topic->lastValue) {
        DEBUG4("send last value for {} to client {}", topic->name, m_id);
        SendValue(topic.get(), topic->lastValue, kSendAll);
      }
    }
  }

  // update meta data
  UpdateMetaClientSub();

  Flush();
}

void ClientData4Base::ClientUnsubscribe(int64_t subuid) {
  DEBUG3("ClientUnsubscribe({}, {})", m_id, subuid);
  auto subIt = m_subscribers.find(subuid);
  if (subIt == m_subscribers.end() || !subIt->getSecond()) {
    return;  // nothing to do
  }
  auto sub = subIt->getSecond().get();

  // remove from topics
  for (auto&& topic : m_server.m_topics) {
    if (topic->subscribers.Remove(sub)) {
      m_server.UpdateMetaTopicSub(topic.get());
    }
  }

  // delete it from client (future value sets will be ignored)
  m_subscribers.erase(subIt);
  UpdateMetaClientSub();

  // loop over all publishers to update period
  m_periodMs = UINT32_MAX;
  for (auto&& sub : m_subscribers) {
    if (m_periodMs == UINT32_MAX) {
      m_periodMs = sub.getSecond()->periodMs;
    } else {
      m_periodMs = std::gcd(m_periodMs, sub.getSecond()->periodMs);
    }
  }
  if (m_periodMs < kMinPeriodMs) {
    m_periodMs = kMinPeriodMs;
  }
  m_setPeriodic(m_periodMs);
}

void ClientData4Base::ClientSetValue(int64_t pubuid, const Value& value) {
  DEBUG4("ClientSetValue({}, {})", m_id, pubuid);
  auto publisherIt = m_publishers.find(pubuid);
  if (publisherIt == m_publishers.end()) {
    WARNING("unrecognized client {} pubuid {}, ignoring set", m_id, pubuid);
    return;  // ignore unrecognized pubuids
  }
  auto topic = publisherIt->getSecond().get()->topic;
  m_server.SetValue(this, topic, value);
}

void ClientDataLocal::SendValue(TopicData* topic, const Value& value,
                                SendMode mode) {
  if (m_server.m_local) {
    m_server.m_local->NetworkSetValue(topic->localHandle, value);
  }
}

void ClientDataLocal::SendAnnounce(TopicData* topic,
                                   std::optional<int64_t> pubuid) {
  if (m_server.m_local) {
    auto& sent = m_announceSent[topic];
    if (sent) {
      return;
    }
    sent = true;

    topic->localHandle = m_server.m_local->NetworkAnnounce(
        topic->name, topic->typeStr, topic->properties, pubuid.value_or(0));
  }
}

void ClientDataLocal::SendUnannounce(TopicData* topic) {
  if (m_server.m_local) {
    auto& sent = m_announceSent[topic];
    if (!sent) {
      return;
    }
    sent = false;
    m_server.m_local->NetworkUnannounce(topic->name);
  }
}

void ClientDataLocal::SendPropertiesUpdate(TopicData* topic,
                                           const wpi::json& update, bool ack) {
  if (m_server.m_local) {
    if (!m_announceSent.lookup(topic)) {
      return;
    }
    m_server.m_local->NetworkPropertiesUpdate(topic->name, update, ack);
  }
}

void ClientDataLocal::HandleLocal(std::span<const ClientMessage> msgs) {
  DEBUG4("HandleLocal()");
  // just map as a normal client into client=0 calls
  for (const auto& elem : msgs) {  // NOLINT
    // common case is value, so check that first
    if (auto msg = std::get_if<ClientValueMsg>(&elem.contents)) {
      ClientSetValue(msg->pubHandle, msg->value);
    } else if (auto msg = std::get_if<PublishMsg>(&elem.contents)) {
      ClientPublish(msg->pubHandle, msg->name, msg->typeStr, msg->properties);
    } else if (auto msg = std::get_if<UnpublishMsg>(&elem.contents)) {
      ClientUnpublish(msg->pubHandle);
    } else if (auto msg = std::get_if<SetPropertiesMsg>(&elem.contents)) {
      ClientSetProperties(msg->name, msg->update);
    } else if (auto msg = std::get_if<SubscribeMsg>(&elem.contents)) {
      ClientSubscribe(msg->subHandle, msg->topicNames, msg->options);
    } else if (auto msg = std::get_if<UnsubscribeMsg>(&elem.contents)) {
      ClientUnsubscribe(msg->subHandle);
    }
  }
}

void ClientData4::ProcessIncomingText(std::string_view data) {
  WireDecodeText(data, *this, m_logger);
}

void ClientData4::ProcessIncomingBinary(std::span<const uint8_t> data) {
  for (;;) {
    if (data.empty()) {
      break;
    }

    // decode message
    int64_t pubuid;
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
      {
        auto out = m_wire.SendBinary();
        WireEncodeBinary(out.Add(), -1, now, value);
      }
      m_wire.Flush();
      continue;
    }

    // handle value set
    ClientSetValue(pubuid, value);
  }
}

void ClientData4::SendValue(TopicData* topic, const Value& value,
                            SendMode mode) {
  if (m_local) {
    mode = ClientData::kSendImmNoFlush;  // always send local immediately
  }
  switch (mode) {
    case ClientData::kSendDisabled:  // do nothing
      break;
    case ClientData::kSendImmNoFlush:  // send immediately
      WriteBinary(topic->id, value.time(), value);
      if (m_local) {
        Flush();
      }
      break;
    case ClientData::kSendAll:  // append to outgoing
      m_outgoing.emplace_back(ServerMessage{ServerValueMsg{topic->id, value}});
      break;
    case ClientData::kSendNormal: {
      // scan outgoing and replace, or append if not present
      bool found = false;
      for (auto&& msg : m_outgoing) {
        if (auto m = std::get_if<ServerValueMsg>(&msg.contents)) {
          if (m->topic == topic->id) {
            m->value = value;
            found = true;
            break;
          }
        }
      }
      if (!found) {
        m_outgoing.emplace_back(
            ServerMessage{ServerValueMsg{topic->id, value}});
      }
      break;
    }
  }
}

void ClientData4::SendAnnounce(TopicData* topic,
                               std::optional<int64_t> pubuid) {
  auto& sent = m_announceSent[topic];
  if (sent) {
    return;
  }
  sent = true;

  if (m_local) {
    WireEncodeAnnounce(SendText().Add(), topic->name, topic->id, topic->typeStr,
                       topic->properties, pubuid);
    Flush();
  } else {
    m_outgoing.emplace_back(ServerMessage{AnnounceMsg{
        topic->name, topic->id, topic->typeStr, pubuid, topic->properties}});
    m_server.m_controlReady = true;
  }
}

void ClientData4::SendUnannounce(TopicData* topic) {
  auto& sent = m_announceSent[topic];
  if (!sent) {
    return;
  }
  sent = false;

  if (m_local) {
    WireEncodeUnannounce(SendText().Add(), topic->name, topic->id);
    Flush();
  } else {
    m_outgoing.emplace_back(
        ServerMessage{UnannounceMsg{topic->name, topic->id}});
    m_server.m_controlReady = true;
  }
}

void ClientData4::SendPropertiesUpdate(TopicData* topic,
                                       const wpi::json& update, bool ack) {
  if (!m_announceSent.lookup(topic)) {
    return;
  }

  if (m_local) {
    WireEncodePropertiesUpdate(SendText().Add(), topic->name, update, ack);
    Flush();
  } else {
    m_outgoing.emplace_back(
        ServerMessage{PropertiesUpdateMsg{topic->name, update, ack}});
    m_server.m_controlReady = true;
  }
}

void ClientData4::SendOutgoing(uint64_t curTimeMs) {
  if (m_outgoing.empty()) {
    return;  // nothing to do
  }

  // rate limit frequency of transmissions
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return;
  }

  if (!m_wire.Ready()) {
    ++m_notReadyCount;
    if (m_notReadyCount > kWireMaxNotReady) {
      m_wire.Disconnect("transmit stalled");
    }
    return;
  }
  m_notReadyCount = 0;

  for (auto&& msg : m_outgoing) {
    if (auto m = std::get_if<ServerValueMsg>(&msg.contents)) {
      WriteBinary(m->topic, m->value.time(), m->value);
    } else {
      WireEncodeText(SendText().Add(), msg);
    }
  }
  m_outgoing.resize(0);
  m_lastSendMs = curTimeMs;
}

void ClientData4::Flush() {
  m_outText.reset();
  m_outBinary.reset();
  m_wire.Flush();
}

bool ClientData3::TopicData3::UpdateFlags(TopicData* topic) {
  unsigned int newFlags = topic->persistent ? NT_PERSISTENT : 0;
  bool updated = flags != newFlags;
  flags = newFlags;
  return updated;
}

void ClientData3::ProcessIncomingBinary(std::span<const uint8_t> data) {
  if (!m_decoder.Execute(&data)) {
    m_wire.Disconnect(m_decoder.GetError());
  }
}

void ClientData3::SendValue(TopicData* topic, const Value& value,
                            SendMode mode) {
  if (m_state != kStateRunning) {
    if (mode == kSendImmNoFlush) {
      mode = kSendAll;
    }
  } else if (m_local) {
    mode = ClientData::kSendImmNoFlush;  // always send local immediately
  }
  TopicData3* topic3 = GetTopic3(topic);

  switch (mode) {
    case ClientData::kSendDisabled:  // do nothing
      break;
    case ClientData::kSendImmNoFlush:  // send immediately and flush
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
    case ClientData::kSendNormal: {
      // scan outgoing and replace, or append if not present
      bool found = false;
      for (auto&& msg : m_outgoing) {
        if (msg.Is(net3::Message3::kEntryUpdate) ||
            msg.Is(net3::Message3::kEntryAssign)) {
          if (msg.id() == topic->id) {
            msg.SetValue(value);
            found = true;
            break;
          }
        }
      }
      if (found) {
        break;
      }
    }
      // fallthrough
    case ClientData::kSendAll:  // append to outgoing
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

void ClientData3::SendAnnounce(TopicData* topic,
                               std::optional<int64_t> pubuid) {
  // ignore if we've not yet built the subscriber
  if (m_subscribers.empty()) {
    return;
  }

  // subscribe to all non-special topics
  if (!topic->special) {
    topic->subscribers.Add(m_subscribers[0].get());
    m_server.UpdateMetaTopicSub(topic);
  }

  // NT3 requires a value to send the assign message, so the assign message
  // will get sent when the first value is sent (by SendValue).
}

void ClientData3::SendUnannounce(TopicData* topic) {
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

void ClientData3::SendPropertiesUpdate(TopicData* topic,
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

void ClientData3::SendOutgoing(uint64_t curTimeMs) {
  if (m_outgoing.empty() || m_state != kStateRunning) {
    return;  // nothing to do
  }

  // rate limit frequency of transmissions
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return;
  }

  if (!m_wire.Ready()) {
    ++m_notReadyCount;
    if (m_notReadyCount > kWireMaxNotReady) {
      m_wire.Disconnect("transmit stalled");
    }
    return;
  }
  m_notReadyCount = 0;

  auto out = m_wire.Send();
  for (auto&& msg : m_outgoing) {
    net3::WireEncode(out.stream(), msg);
  }
  m_outgoing.resize(0);
  m_lastSendMs = curTimeMs;
}

void ClientData3::KeepAlive() {
  DEBUG4("KeepAlive({})", m_id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received unexpected KeepAlive message");
    return;
  }
  // ignore
}

void ClientData3::ServerHelloDone() {
  DEBUG4("ServerHelloDone({})", m_id);
  m_decoder.SetError("received unexpected ServerHelloDone message");
}

void ClientData3::ClientHelloDone() {
  DEBUG4("ClientHelloDone({})", m_id);
  if (m_state != kStateServerHelloComplete) {
    m_decoder.SetError("received unexpected ClientHelloDone message");
    return;
  }
  m_state = kStateRunning;
}

void ClientData3::ClearEntries() {
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
        topic->publishers.Remove(publisherIt->second.get());

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

void ClientData3::ProtoUnsup(unsigned int proto_rev) {
  DEBUG4("ProtoUnsup({})", m_id);
  m_decoder.SetError("received unexpected ProtoUnsup message");
}

void ClientData3::ClientHello(std::string_view self_id,
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
  m_periodMs = std::gcd(m_periodMs, sub->periodMs);
  if (m_periodMs < kMinPeriodMs) {
    m_periodMs = kMinPeriodMs;
  }
  m_setPeriodic(m_periodMs);

  {
    auto out = m_wire.Send();
    net3::WireEncodeServerHello(out.stream(), 0, "server");
    for (auto&& topic : m_server.m_topics) {
      if (topic && !topic->special && topic->IsPublished() &&
          topic->lastValue) {
        DEBUG4("client {}: initial announce of '{}' (id {})", m_id, topic->name,
               topic->id);
        topic->subscribers.Add(sub.get());
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

void ClientData3::ServerHello(unsigned int flags, std::string_view self_id) {
  DEBUG4("ServerHello({}, {}, {})", m_id, flags, self_id);
  m_decoder.SetError("received unexpected ServerHello message");
}

void ClientData3::EntryAssign(std::string_view name, unsigned int id,
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
  if ((flags & NT_PERSISTENT) != 0) {
    properties["persistent"] = true;
  }

  // create topic
  auto topic = m_server.CreateTopic(this, name, typeStr, properties);
  TopicData3* topic3 = GetTopic3(topic);
  if (topic3->published || topic3->sentAssign) {
    WARNING("ignorning client {} duplicate publish of '{}'", m_id, name);
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
  topic->publishers.Add(publisherIt->getSecond().get());

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

void ClientData3::EntryUpdate(unsigned int id, unsigned int seq_num,
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
      topic->publishers.Add(publisherIt->getSecond().get());

      // update meta data
      m_server.UpdateMetaTopicPub(topic);
      UpdateMetaClientPub();
    }
  }
  topic3->seqNum = net3::SequenceNumber{seq_num};

  m_server.SetValue(this, topic, value);
}

void ClientData3::FlagsUpdate(unsigned int id, unsigned int flags) {
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

void ClientData3::EntryDelete(unsigned int id) {
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
        topic->publishers.Remove(publisherIt->second.get());

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

bool TopicData::SetProperties(const wpi::json& update) {
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

void TopicData::RefreshProperties() {
  persistent = false;
  retained = false;

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
}

bool TopicData::SetFlags(unsigned int flags_) {
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
  return updated;
}

bool SubscriberData::Matches(std::string_view name, bool special) {
  for (auto&& topicName : topicNames) {
    if ((!options.prefixMatch && name == topicName) ||
        (options.prefixMatch && (!special || !topicName.empty()) &&
         wpi::starts_with(name, topicName))) {
      return true;
    }
  }
  return false;
}

SImpl::SImpl(wpi::Logger& logger) : m_logger{logger} {
  // local is client 0
  m_clients.emplace_back(std::make_unique<ClientDataLocal>(*this, 0, logger));
  m_localClient = static_cast<ClientDataLocal*>(m_clients.back().get());
}

std::pair<std::string, int> SImpl::AddClient(
    std::string_view name, std::string_view connInfo, bool local,
    WireConnection& wire, ServerImpl::SetPeriodicFunc setPeriodic) {
  // strip anything after @ in the name
  name = wpi::split(name, '@').first;
  if (name.empty()) {
    name = "NT4";
  }
  size_t index = m_clients.size();
  // find an empty slot and check for duplicates
  // just do a linear search as number of clients is typically small (<10)
  int duplicateName = 0;
  for (size_t i = 0, end = index; i < end; ++i) {
    auto& clientData = m_clients[i];
    if (clientData && clientData->GetOriginalName() == name) {
      ++duplicateName;
    } else if (!clientData && index == end) {
      index = i;
    }
  }
  if (index == m_clients.size()) {
    m_clients.emplace_back();
  }

  // if duplicate name, de-duplicate
  std::string dedupName;
  if (duplicateName > 0) {
    dedupName = fmt::format("{}@{}", name, duplicateName);
  } else {
    dedupName = name;
  }

  auto& clientData = m_clients[index];
  clientData = std::make_unique<ClientData4>(name, dedupName, connInfo, local,
                                             wire, std::move(setPeriodic),
                                             *this, index, m_logger);

  // create client meta topics
  clientData->m_metaPub =
      CreateMetaTopic(fmt::format("$clientpub${}", dedupName));
  clientData->m_metaSub =
      CreateMetaTopic(fmt::format("$clientsub${}", dedupName));

  // update meta topics
  clientData->UpdateMetaClientPub();
  clientData->UpdateMetaClientSub();

  wire.Flush();

  DEBUG3("AddClient('{}', '{}') -> {}", name, connInfo, index);
  return {std::move(dedupName), index};
}

int SImpl::AddClient3(std::string_view connInfo, bool local,
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

void SImpl::RemoveClient(int clientId) {
  DEBUG3("RemoveClient({})", clientId);
  auto& client = m_clients[clientId];

  // remove all publishers and subscribers for this client
  wpi::SmallVector<TopicData*, 16> toDelete;
  for (auto&& topic : m_topics) {
    auto pubRemove =
        std::remove_if(topic->publishers.begin(), topic->publishers.end(),
                       [&](auto&& e) { return e->client == client.get(); });
    bool pubChanged = pubRemove != topic->publishers.end();
    topic->publishers.erase(pubRemove, topic->publishers.end());

    auto subRemove =
        std::remove_if(topic->subscribers.begin(), topic->subscribers.end(),
                       [&](auto&& e) { return e->client == client.get(); });
    bool subChanged = subRemove != topic->subscribers.end();
    topic->subscribers.erase(subRemove, topic->subscribers.end());

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

  // delete the client
  client.reset();
}

bool SImpl::PersistentChanged() {
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

void SImpl::DumpPersistent(wpi::raw_ostream& os) {
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

std::string SImpl::LoadPersistent(std::string_view in) {
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

TopicData* SImpl::CreateTopic(ClientData* client, std::string_view name,
                              std::string_view typeStr,
                              const wpi::json& properties, bool special) {
  auto& topic = m_nameTopics[name];
  if (topic) {
    if (typeStr != topic->typeStr) {
      if (client) {
        WARNING("client {} publish '{}' conflicting type '{}' (currently '{}')",
                client->GetName(), name, typeStr, topic->typeStr);
      }
    }
  } else {
    // new topic
    unsigned int id = m_topics.emplace_back(
        std::make_unique<TopicData>(name, typeStr, properties));
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
      for (auto subscriber : subscribers) {
        topic->subscribers.Add(subscriber);
      }

      // don't announce to this client if no subscribers
      if (subscribers.empty()) {
        continue;
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

TopicData* SImpl::CreateMetaTopic(std::string_view name) {
  return CreateTopic(nullptr, name, "msgpack", {{"retained", true}}, true);
}

void SImpl::DeleteTopic(TopicData* topic) {
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
  wpi::SmallVector<bool, 16> clients;
  clients.resize(m_clients.size());
  for (auto&& sub : topic->subscribers) {
    clients[sub->client->GetId()] = true;
  }
  for (size_t i = 0, iend = clients.size(); i < iend; ++i) {
    if (!clients[i]) {
      continue;
    }
    if (auto aClient = m_clients[i].get()) {
      aClient->SendUnannounce(topic);
    }
  }

  // erase the topic
  m_nameTopics.erase(topic->name);
  m_topics.erase(topic->id);
}

void SImpl::SetProperties(ClientData* client, TopicData* topic,
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

void SImpl::SetFlags(ClientData* client, TopicData* topic, unsigned int flags) {
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

void SImpl::SetValue(ClientData* client, TopicData* topic, const Value& value) {
  // update retained value if from same client or timestamp newer
  if (!topic->lastValue || topic->lastValueClient == client ||
      value.time() >= topic->lastValue.time()) {
    DEBUG4("updating '{}' last value (time was {} is {})", topic->name,
           topic->lastValue.time(), value.time());
    topic->lastValue = value;
    topic->lastValueClient = client;

    // if persistent, update flag
    if (topic->persistent) {
      m_persistentChanged = true;
    }
  }

  // propagate to subscribers; as each client may have multiple subscribers,
  // but we only want to send the value once, first map to clients and then
  // take action based on union of subscriptions

  // indexed by clientId
  wpi::SmallVector<ClientData::SendMode, 16> toSend;
  toSend.resize(m_clients.size());

  for (auto&& subscriber : topic->subscribers) {
    int id = subscriber->client->GetId();
    if (subscriber->options.topicsOnly) {
      continue;
    } else if (subscriber->options.sendAll) {
      toSend[id] = ClientData::kSendAll;
    } else if (toSend[id] != ClientData::kSendAll) {
      toSend[id] = ClientData::kSendNormal;
    }
  }

  for (size_t i = 0, iend = toSend.size(); i < iend; ++i) {
    auto aClient = m_clients[i].get();
    if (!aClient || client == aClient) {
      continue;  // don't echo back
    }
    if (toSend[i] != ClientData::kSendDisabled) {
      aClient->SendValue(topic, value, toSend[i]);
    }
  }
}

void SImpl::UpdateMetaClients(const std::vector<ConnectionInfo>& conns) {
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

void SImpl::UpdateMetaTopicPub(TopicData* topic) {
  if (!topic->metaPub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, topic->publishers.size());
  for (auto&& pub : topic->publishers) {
    mpack_start_map(&w, 2);
    mpack_write_str(&w, "client");
    if (pub->client) {
      mpack_write_str(&w, pub->client->GetName());
    } else {
      mpack_write_str(&w, "");
    }
    mpack_write_str(&w, "pubuid");
    mpack_write_int(&w, pub->pubuid);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaPub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void SImpl::UpdateMetaTopicSub(TopicData* topic) {
  if (!topic->metaSub) {
    return;
  }
  Writer w;
  mpack_start_array(&w, topic->subscribers.size());
  for (auto&& sub : topic->subscribers) {
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "client");
    if (sub->client) {
      mpack_write_str(&w, sub->client->GetName());
    } else {
      mpack_write_str(&w, "");
    }
    mpack_write_str(&w, "subuid");
    mpack_write_int(&w, sub->subuid);
    mpack_write_str(&w, "options");
    WriteOptions(w, sub->options);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    SetValue(nullptr, topic->metaSub, Value::MakeRaw(std::move(w.bytes)));
  }
}

void SImpl::PropertiesChanged(ClientData* client, TopicData* topic,
                              const wpi::json& update) {
  // removing some properties can result in the topic being unpublished
  if (!topic->IsPublished()) {
    DeleteTopic(topic);
  } else {
    // send updated announcement to all subscribers
    wpi::SmallVector<bool, 16> clients;
    clients.resize(m_clients.size());
    for (auto&& sub : topic->subscribers) {
      clients[sub->client->GetId()] = true;
    }
    for (size_t i = 0, iend = clients.size(); i < iend; ++i) {
      if (!clients[i]) {
        continue;
      }
      if (auto aClient = m_clients[i].get()) {
        aClient->SendPropertiesUpdate(topic, update, aClient == client);
      }
    }
  }
}

class ServerImpl::Impl final : public SImpl {
 public:
  explicit Impl(wpi::Logger& logger) : SImpl{logger} {}
};

ServerImpl::ServerImpl(wpi::Logger& logger)
    : m_impl{std::make_unique<Impl>(logger)} {}

ServerImpl::~ServerImpl() = default;

void ServerImpl::SendControl(uint64_t curTimeMs) {
  if (!m_impl->m_controlReady) {
    return;
  }
  m_impl->m_controlReady = false;

  for (auto&& client : m_impl->m_clients) {
    if (client) {
      // to ensure ordering, just send everything
      client->SendOutgoing(curTimeMs);
      client->Flush();
    }
  }
}

void ServerImpl::SendValues(int clientId, uint64_t curTimeMs) {
  auto client = m_impl->m_clients[clientId].get();
  client->SendOutgoing(curTimeMs);
  client->Flush();
}

void ServerImpl::HandleLocal(std::span<const ClientMessage> msgs) {
  // just map as a normal client into client=0 calls
  m_impl->m_localClient->HandleLocal(msgs);
}

void ServerImpl::SetLocal(LocalInterface* local) {
  m_impl->m_local = local;

  // create server meta topics
  m_impl->m_metaClients = m_impl->CreateMetaTopic("$clients");

  // create local client meta topics
  m_impl->m_localClient->m_metaPub = m_impl->CreateMetaTopic("$serverpub");
  m_impl->m_localClient->m_metaSub = m_impl->CreateMetaTopic("$serversub");

  // update meta topics
  m_impl->m_localClient->UpdateMetaClientPub();
  m_impl->m_localClient->UpdateMetaClientSub();
}

void ServerImpl::ProcessIncomingText(int clientId, std::string_view data) {
  m_impl->m_clients[clientId]->ProcessIncomingText(data);
}

void ServerImpl::ProcessIncomingBinary(int clientId,
                                       std::span<const uint8_t> data) {
  m_impl->m_clients[clientId]->ProcessIncomingBinary(data);
}

std::pair<std::string, int> ServerImpl::AddClient(std::string_view name,
                                                  std::string_view connInfo,
                                                  bool local,
                                                  WireConnection& wire,
                                                  SetPeriodicFunc setPeriodic) {
  return m_impl->AddClient(name, connInfo, local, wire, std::move(setPeriodic));
}

int ServerImpl::AddClient3(std::string_view connInfo, bool local,
                           net3::WireConnection3& wire,
                           Connected3Func connected,
                           SetPeriodicFunc setPeriodic) {
  return m_impl->AddClient3(connInfo, local, wire, std::move(connected),
                            std::move(setPeriodic));
}

void ServerImpl::RemoveClient(int clientId) {
  m_impl->RemoveClient(clientId);
}

void ServerImpl::ConnectionsChanged(const std::vector<ConnectionInfo>& conns) {
  m_impl->UpdateMetaClients(conns);
}

bool ServerImpl::PersistentChanged() {
  return m_impl->PersistentChanged();
}

std::string ServerImpl::DumpPersistent() {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  m_impl->DumpPersistent(os);
  os.flush();
  return rv;
}

std::string ServerImpl::LoadPersistent(std::string_view in) {
  return m_impl->LoadPersistent(in);
}

void ServerStartup::Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                            std::string_view name, std::string_view typeStr,
                            const wpi::json& properties,
                            const PubSubOptions& options) {
  m_server.m_impl->m_localClient->ClientPublish(pubHandle, name, typeStr,
                                                properties);
}

void ServerStartup::Subscribe(NT_Subscriber subHandle,
                              std::span<const std::string> topicNames,
                              const PubSubOptions& options) {
  m_server.m_impl->m_localClient->ClientSubscribe(subHandle, topicNames,
                                                  options);
}

void ServerStartup::SetValue(NT_Publisher pubHandle, const Value& value) {
  m_server.m_impl->m_localClient->ClientSetValue(pubHandle, value);
}
