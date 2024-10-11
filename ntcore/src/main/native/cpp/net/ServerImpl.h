// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cmath>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/DenseMap.h>
#include <wpi/SmallPtrSet.h>
#include <wpi/StringMap.h>
#include <wpi/UidVector.h>
#include <wpi/json.h>

#include "ClientMessageQueue.h"
#include "Message.h"
#include "NetworkOutgoingQueue.h"
#include "NetworkPing.h"
#include "PubSubOptions.h"
#include "WireConnection.h"
#include "WireDecoder.h"
#include "WireEncoder.h"
#include "net3/Message3.h"
#include "net3/SequenceNumber.h"
#include "net3/WireConnection3.h"
#include "net3/WireDecoder3.h"

namespace wpi {
class Logger;
template <typename T>
class SmallVectorImpl;
class raw_ostream;
}  // namespace wpi

namespace nt::net3 {
class WireConnection3;
}  // namespace nt::net3

namespace nt::net {

struct ClientMessage;
class LocalInterface;
class WireConnection;

class ServerImpl final {
 public:
  using SetPeriodicFunc = std::function<void(uint32_t repeatMs)>;
  using Connected3Func =
      std::function<void(std::string_view name, uint16_t proto)>;

  explicit ServerImpl(wpi::Logger& logger);

  void SendAllOutgoing(uint64_t curTimeMs, bool flush);
  void SendOutgoing(int clientId, uint64_t curTimeMs);

  void SetLocal(ServerMessageHandler* local, ClientMessageQueue* queue);

  // these return true if any messages have been queued for later processing
  bool ProcessIncomingText(int clientId, std::string_view data);
  bool ProcessIncomingBinary(int clientId, std::span<const uint8_t> data);

  // later processing -- returns true if more to process
  bool ProcessIncomingMessages(size_t max);
  bool ProcessLocalMessages(size_t max);

  // Returns -1 if cannot add client (e.g. due to duplicate name).
  // Caller must ensure WireConnection lifetime lasts until RemoveClient() call.
  std::pair<std::string, int> AddClient(std::string_view name,
                                        std::string_view connInfo, bool local,
                                        WireConnection& wire,
                                        SetPeriodicFunc setPeriodic);
  int AddClient3(std::string_view connInfo, bool local,
                 net3::WireConnection3& wire, Connected3Func connected,
                 SetPeriodicFunc setPeriodic);
  std::shared_ptr<void> RemoveClient(int clientId);

  void ConnectionsChanged(const std::vector<ConnectionInfo>& conns);

  // if any persistent values changed since the last call to this function
  bool PersistentChanged();
  std::string DumpPersistent();
  // returns newline-separated errors
  std::string LoadPersistent(std::string_view in);

 private:
  static constexpr uint32_t kMinPeriodMs = 5;

  class ClientData;
  struct PublisherData;
  struct SubscriberData;

  struct TopicData {
    TopicData(wpi::Logger& logger, std::string_view name,
              std::string_view typeStr)
        : m_logger{logger}, name{name}, typeStr{typeStr} {}
    TopicData(wpi::Logger& logger, std::string_view name,
              std::string_view typeStr, wpi::json properties)
        : m_logger{logger},
          name{name},
          typeStr{typeStr},
          properties(std::move(properties)) {
      RefreshProperties();
    }

    bool IsPublished() const {
      return persistent || retained || publisherCount != 0;
    }

    // returns true if properties changed
    bool SetProperties(const wpi::json& update);
    void RefreshProperties();
    bool SetFlags(unsigned int flags_);

    wpi::Logger& m_logger;  // Must be m_logger for WARN macro to work
    std::string name;
    unsigned int id;
    Value lastValue;
    ClientData* lastValueClient = nullptr;
    std::string typeStr;
    wpi::json properties = wpi::json::object();
    unsigned int publisherCount{0};
    bool persistent{false};
    bool retained{false};
    bool cached{true};
    bool special{false};
    int localTopic{0};

    void AddPublisher(ClientData* client, PublisherData* pub) {
      if (clients[client].publishers.insert(pub).second) {
        ++publisherCount;
      }
    }

    void RemovePublisher(ClientData* client, PublisherData* pub) {
      if (clients[client].publishers.erase(pub)) {
        --publisherCount;
      }
    }

    struct TopicClientData {
      wpi::SmallPtrSet<PublisherData*, 2> publishers;
      wpi::SmallPtrSet<SubscriberData*, 2> subscribers;
      ValueSendMode sendMode = ValueSendMode::kDisabled;

      bool AddSubscriber(SubscriberData* sub) {
        bool added = subscribers.insert(sub).second;
        if (!sub->options.topicsOnly) {
          if (sub->options.sendAll) {
            sendMode = ValueSendMode::kAll;
          } else if (sendMode == ValueSendMode::kDisabled) {
            sendMode = ValueSendMode::kNormal;
          }
        }
        return added;
      }
    };
    wpi::SmallDenseMap<ClientData*, TopicClientData, 4> clients;

    // meta topics
    TopicData* metaPub = nullptr;
    TopicData* metaSub = nullptr;
  };

  class ClientData {
   public:
    ClientData(std::string_view name, std::string_view connInfo, bool local,
               ServerImpl::SetPeriodicFunc setPeriodic, ServerImpl& server,
               int id, wpi::Logger& logger)
        : m_name{name},
          m_connInfo{connInfo},
          m_local{local},
          m_setPeriodic{std::move(setPeriodic)},
          m_server{server},
          m_id{id},
          m_logger{logger} {}
    virtual ~ClientData() = default;

    // these return true if any messages have been queued for later processing
    virtual bool ProcessIncomingText(std::string_view data) = 0;
    virtual bool ProcessIncomingBinary(std::span<const uint8_t> data) = 0;

    virtual void SendValue(TopicData* topic, const Value& value,
                           ValueSendMode mode) = 0;
    virtual void SendAnnounce(TopicData* topic, std::optional<int> pubuid) = 0;
    virtual void SendUnannounce(TopicData* topic) = 0;
    virtual void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                                      bool ack) = 0;
    virtual void SendOutgoing(uint64_t curTimeMs, bool flush) = 0;
    virtual void Flush() = 0;

    // later processing -- returns true if more to process
    virtual bool ProcessIncomingMessages(size_t max) = 0;

    void UpdateMetaClientPub();
    void UpdateMetaClientSub();

    std::span<SubscriberData*> GetSubscribers(
        std::string_view name, bool special,
        wpi::SmallVectorImpl<SubscriberData*>& buf);

    std::string_view GetName() const { return m_name; }
    int GetId() const { return m_id; }

    virtual void UpdatePeriod(TopicData::TopicClientData& tcd,
                              TopicData* topic) {}

   protected:
    std::string m_name;
    std::string m_connInfo;
    bool m_local;  // local to machine
    ServerImpl::SetPeriodicFunc m_setPeriodic;
    // TODO: make this per-topic?
    uint32_t m_periodMs{UINT32_MAX};
    ServerImpl& m_server;
    int m_id;

    wpi::Logger& m_logger;

    wpi::DenseMap<int, std::unique_ptr<PublisherData>> m_publishers;
    wpi::DenseMap<int, std::unique_ptr<SubscriberData>> m_subscribers;

   public:
    // meta topics
    TopicData* m_metaPub = nullptr;
    TopicData* m_metaSub = nullptr;
  };

  class ClientData4Base : public ClientData, protected ClientMessageHandler {
   public:
    ClientData4Base(std::string_view name, std::string_view connInfo,
                    bool local, ServerImpl::SetPeriodicFunc setPeriodic,
                    ServerImpl& server, int id, wpi::Logger& logger)
        : ClientData{name, connInfo, local, setPeriodic, server, id, logger} {}

   protected:
    // ClientMessageHandler interface
    void ClientPublish(int pubuid, std::string_view name,
                       std::string_view typeStr, const wpi::json& properties,
                       const PubSubOptionsImpl& options) final;
    void ClientUnpublish(int pubuid) final;
    void ClientSetProperties(std::string_view name,
                             const wpi::json& update) final;
    void ClientSubscribe(int subuid, std::span<const std::string> topicNames,
                         const PubSubOptionsImpl& options) final;
    void ClientUnsubscribe(int subuid) final;

    void ClientSetValue(int pubuid, const Value& value) final;

    bool DoProcessIncomingMessages(ClientMessageQueue& queue, size_t max);

    wpi::DenseMap<TopicData*, bool> m_announceSent;

   private:
    std::array<ClientMessage, 16> m_msgsBuf;
  };

  class ClientDataLocal final : public ClientData4Base {
   public:
    ClientDataLocal(ServerImpl& server, int id, wpi::Logger& logger)
        : ClientData4Base{"", "", true, [](uint32_t) {}, server, id, logger} {}

    bool ProcessIncomingText(std::string_view data) final { return false; }
    bool ProcessIncomingBinary(std::span<const uint8_t> data) final {
      return false;
    }

    bool ProcessIncomingMessages(size_t max) final {
      if (!m_queue) {
        return false;
      }
      return DoProcessIncomingMessages(*m_queue, max);
    }

    void SendValue(TopicData* topic, const Value& value,
                   ValueSendMode mode) final;
    void SendAnnounce(TopicData* topic, std::optional<int> pubuid) final;
    void SendUnannounce(TopicData* topic) final;
    void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                              bool ack) final;
    void SendOutgoing(uint64_t curTimeMs, bool flush) final {}
    void Flush() final {}

    void SetQueue(ClientMessageQueue* queue) { m_queue = queue; }

   private:
    ClientMessageQueue* m_queue = nullptr;
  };

  class ClientData4 final : public ClientData4Base {
   public:
    ClientData4(std::string_view name, std::string_view connInfo, bool local,
                WireConnection& wire, ServerImpl::SetPeriodicFunc setPeriodic,
                ServerImpl& server, int id, wpi::Logger& logger)
        : ClientData4Base{name,   connInfo, local, setPeriodic,
                          server, id,       logger},
          m_wire{wire},
          m_ping{wire},
          m_incoming{logger},
          m_outgoing{wire, local} {}

    bool ProcessIncomingText(std::string_view data) final;
    bool ProcessIncomingBinary(std::span<const uint8_t> data) final;

    bool ProcessIncomingMessages(size_t max) final {
      if (!DoProcessIncomingMessages(m_incoming, max)) {
        m_wire.StartRead();
        return false;
      }
      return true;
    }

    void SendValue(TopicData* topic, const Value& value,
                   ValueSendMode mode) final;
    void SendAnnounce(TopicData* topic, std::optional<int> pubuid) final;
    void SendUnannounce(TopicData* topic) final;
    void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                              bool ack) final;
    void SendOutgoing(uint64_t curTimeMs, bool flush) final;

    void Flush() final {}

    void UpdatePeriod(TopicData::TopicClientData& tcd, TopicData* topic) final;

   public:
    WireConnection& m_wire;

   private:
    NetworkPing m_ping;
    NetworkIncomingClientQueue m_incoming;
    NetworkOutgoingQueue<ServerMessage> m_outgoing;
  };

  class ClientData3 final : public ClientData, private net3::MessageHandler3 {
   public:
    ClientData3(std::string_view connInfo, bool local,
                net3::WireConnection3& wire,
                ServerImpl::Connected3Func connected,
                ServerImpl::SetPeriodicFunc setPeriodic, ServerImpl& server,
                int id, wpi::Logger& logger)
        : ClientData{"", connInfo, local, setPeriodic, server, id, logger},
          m_connected{std::move(connected)},
          m_wire{wire},
          m_decoder{*this},
          m_incoming{logger} {}

    bool ProcessIncomingText(std::string_view data) final { return false; }
    bool ProcessIncomingBinary(std::span<const uint8_t> data) final;

    bool ProcessIncomingMessages(size_t max) final { return false; }

    void SendValue(TopicData* topic, const Value& value,
                   ValueSendMode mode) final;
    void SendAnnounce(TopicData* topic, std::optional<int> pubuid) final;
    void SendUnannounce(TopicData* topic) final;
    void SendPropertiesUpdate(TopicData* topic, const wpi::json& update,
                              bool ack) final;
    void SendOutgoing(uint64_t curTimeMs, bool flush) final;

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
    void EntryAssign(std::string_view name, unsigned int id,
                     unsigned int seq_num, const Value& value,
                     unsigned int flags) final;
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

    NetworkIncomingClientQueue m_incoming;
    std::vector<net3::Message3> m_outgoing;
    wpi::DenseMap<NT_Topic, size_t> m_outgoingValueMap;
    int64_t m_nextPubUid{1};
    uint64_t m_lastSendMs{0};

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

  struct PublisherData {
    PublisherData(ClientData* client, TopicData* topic, int64_t pubuid)
        : client{client}, topic{topic}, pubuid{pubuid} {
      UpdateMeta();
    }

    void UpdateMeta();

    ClientData* client;
    TopicData* topic;
    int64_t pubuid;
    std::vector<uint8_t> metaClient;
    std::vector<uint8_t> metaTopic;
  };

  struct SubscriberData {
    SubscriberData(ClientData* client, std::span<const std::string> topicNames,
                   int64_t subuid, const PubSubOptionsImpl& options)
        : client{client},
          topicNames{topicNames.begin(), topicNames.end()},
          subuid{subuid},
          options{options},
          periodMs(std::lround(options.periodicMs / 10.0) * 10) {
      UpdateMeta();
      if (periodMs < kMinPeriodMs) {
        periodMs = kMinPeriodMs;
      }
    }

    void Update(std::span<const std::string> topicNames_,
                const PubSubOptionsImpl& options_) {
      topicNames = {topicNames_.begin(), topicNames_.end()};
      options = options_;
      UpdateMeta();
      periodMs = std::lround(options_.periodicMs / 10.0) * 10;
      if (periodMs < kMinPeriodMs) {
        periodMs = kMinPeriodMs;
      }
    }

    bool Matches(std::string_view name, bool special);

    void UpdateMeta();

    ClientData* client;
    std::vector<std::string> topicNames;
    int64_t subuid;
    PubSubOptionsImpl options;
    std::vector<uint8_t> metaClient;
    std::vector<uint8_t> metaTopic;
    wpi::DenseMap<TopicData*, bool> topics;
    // in options as double, but copy here as integer; rounded to the nearest
    // 10 ms
    uint32_t periodMs;
  };

  wpi::Logger& m_logger;
  ServerMessageHandler* m_local{nullptr};
  bool m_controlReady{false};

  ClientDataLocal* m_localClient;
  std::vector<std::unique_ptr<ClientData>> m_clients;
  wpi::UidVector<std::unique_ptr<TopicData>, 16> m_topics;
  wpi::StringMap<TopicData*> m_nameTopics;
  bool m_persistentChanged{false};

  // global meta topics (other meta topics are linked to from the specific
  // client or topic)
  TopicData* m_metaClients;

  void DumpPersistent(wpi::raw_ostream& os);

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

  void PropertiesChanged(ClientData* client, TopicData* topic,
                         const wpi::json& update);
};

}  // namespace nt::net
