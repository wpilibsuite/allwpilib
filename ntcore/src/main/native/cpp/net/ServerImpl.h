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
#include <wpi/StringMap.h>
#include <wpi/UidVector.h>
#include <wpi/json.h>

#include "Message.h"
#include "NetworkInterface.h"
#include "PubSubOptions.h"
#include "VectorSet.h"
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

  void SendControl(uint64_t curTimeMs);
  void SendValues(int clientId, uint64_t curTimeMs);

  void HandleLocal(std::span<const ClientMessage> msgs);
  void SetLocal(LocalInterface* local);

  void ProcessIncomingText(int clientId, std::string_view data);
  void ProcessIncomingBinary(int clientId, std::span<const uint8_t> data);

  // Returns -1 if cannot add client (e.g. due to duplicate name).
  // Caller must ensure WireConnection lifetime lasts until RemoveClient() call.
  std::pair<std::string, int> AddClient(std::string_view name,
                                        std::string_view connInfo, bool local,
                                        WireConnection& wire,
                                        SetPeriodicFunc setPeriodic);
  int AddClient3(std::string_view connInfo, bool local,
                 net3::WireConnection3& wire, Connected3Func connected,
                 SetPeriodicFunc setPeriodic);
  void RemoveClient(int clientId);

  void ConnectionsChanged(const std::vector<ConnectionInfo>& conns);

  // if any persistent values changed since the last call to this function
  bool PersistentChanged();
  std::string DumpPersistent();
  // returns newline-separated errors
  std::string LoadPersistent(std::string_view in);

 private:
  static constexpr uint32_t kMinPeriodMs = 5;

  struct PublisherData;
  struct SubscriberData;
  struct TopicData;

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

    std::string_view GetName() const { return m_name; }
    int GetId() const { return m_id; }

   protected:
    std::string m_name;
    std::string m_connInfo;
    bool m_local;  // local to machine
    ServerImpl::SetPeriodicFunc m_setPeriodic;
    // TODO: make this per-topic?
    uint32_t m_periodMs{UINT32_MAX};
    uint64_t m_lastSendMs{0};
    ServerImpl& m_server;
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
    ClientData4Base(std::string_view name, std::string_view connInfo,
                    bool local, ServerImpl::SetPeriodicFunc setPeriodic,
                    ServerImpl& server, int id, wpi::Logger& logger)
        : ClientData{name, connInfo, local, setPeriodic, server, id, logger} {}

   protected:
    // ClientMessageHandler interface
    void ClientPublish(int64_t pubuid, std::string_view name,
                       std::string_view typeStr,
                       const wpi::json& properties) final;
    void ClientUnpublish(int64_t pubuid) final;
    void ClientSetProperties(std::string_view name,
                             const wpi::json& update) final;
    void ClientSubscribe(int64_t subuid,
                         std::span<const std::string> topicNames,
                         const PubSubOptionsImpl& options) final;
    void ClientUnsubscribe(int64_t subuid) final;

    void ClientSetValue(int64_t pubuid, const Value& value);

    wpi::DenseMap<TopicData*, bool> m_announceSent;
  };

  class ClientDataLocal final : public ClientData4Base {
   public:
    ClientDataLocal(ServerImpl& server, int id, wpi::Logger& logger)
        : ClientData4Base{"", "", true, [](uint32_t) {}, server, id, logger} {}

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
    ClientData4(std::string_view name, std::string_view connInfo, bool local,
                WireConnection& wire, ServerImpl::SetPeriodicFunc setPeriodic,
                ServerImpl& server, int id, wpi::Logger& logger)
        : ClientData4Base{name,   connInfo, local, setPeriodic,
                          server, id,       logger},
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
    wpi::DenseMap<NT_Topic, size_t> m_outgoingValueMap;

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
                net3::WireConnection3& wire,
                ServerImpl::Connected3Func connected,
                ServerImpl::SetPeriodicFunc setPeriodic, ServerImpl& server,
                int id, wpi::Logger& logger)
        : ClientData{"", connInfo, local, setPeriodic, server, id, logger},
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

    std::vector<net3::Message3> m_outgoing;
    wpi::DenseMap<NT_Topic, size_t> m_outgoingValueMap;
    int64_t m_nextPubUid{1};

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
                   int64_t subuid, const PubSubOptionsImpl& options)
        : client{client},
          topicNames{topicNames.begin(), topicNames.end()},
          subuid{subuid},
          options{options},
          periodMs(std::lround(options.periodicMs / 10.0) * 10) {
      if (periodMs < kMinPeriodMs) {
        periodMs = kMinPeriodMs;
      }
    }

    void Update(std::span<const std::string> topicNames_,
                const PubSubOptionsImpl& options_) {
      topicNames = {topicNames_.begin(), topicNames_.end()};
      options = options_;
      periodMs = std::lround(options_.periodicMs / 10.0) * 10;
      if (periodMs < kMinPeriodMs) {
        periodMs = kMinPeriodMs;
      }
    }

    bool Matches(std::string_view name, bool special);

    ClientData* client;
    std::vector<std::string> topicNames;
    int64_t subuid;
    PubSubOptionsImpl options;
    // in options as double, but copy here as integer; rounded to the nearest
    // 10 ms
    uint32_t periodMs;
  };

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
