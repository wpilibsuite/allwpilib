// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <wpi/json_fwd.h>

#include "net/NetworkOutgoingQueue.h"
#include "server/Functions.h"
#include "server/ServerPublisher.h"
#include "server/ServerSubscriber.h"

namespace wpi {
class Logger;
template <typename T>
class SmallVectorImpl;
}  // namespace wpi

namespace nt::server {

struct ServerTopic;
class ServerStorage;
struct TopicClientData;

class ServerClient {
 public:
  ServerClient(std::string_view name, std::string_view connInfo, bool local,
               SetPeriodicFunc setPeriodic, ServerStorage& storage, int id,
               wpi::Logger& logger)
      : m_name{name},
        m_connInfo{connInfo},
        m_local{local},
        m_setPeriodic{std::move(setPeriodic)},
        m_storage{storage},
        m_id{id},
        m_logger{logger} {}
  ServerClient(const ServerClient&) = delete;
  ServerClient& operator=(const ServerClient&) = delete;
  virtual ~ServerClient() = default;

  // these return true if any messages have been queued for later processing
  virtual bool ProcessIncomingText(std::string_view data) = 0;
  virtual bool ProcessIncomingBinary(std::span<const uint8_t> data) = 0;

  virtual void SendValue(ServerTopic* topic, const Value& value,
                         net::ValueSendMode mode) = 0;
  virtual void SendAnnounce(ServerTopic* topic, std::optional<int> pubuid) = 0;
  virtual void SendUnannounce(ServerTopic* topic) = 0;
  virtual void SendPropertiesUpdate(ServerTopic* topic, const wpi::json& update,
                                    bool ack) = 0;
  virtual void SendOutgoing(uint64_t curTimeMs, bool flush) = 0;
  virtual void Flush() = 0;

  // later processing -- returns true if more to process
  virtual bool ProcessIncomingMessages(size_t max) = 0;

  void UpdateMetaClientPub();
  void UpdateMetaClientSub();

  std::span<ServerSubscriber*> GetSubscribers(
      std::string_view name, bool special,
      wpi::SmallVectorImpl<ServerSubscriber*>& buf);

  std::string_view GetName() const { return m_name; }
  int GetId() const { return m_id; }

  virtual void UpdatePeriod(TopicClientData& tcd, ServerTopic* topic) {}

 protected:
  std::string m_name;
  std::string m_connInfo;
  bool m_local;  // local to machine
  SetPeriodicFunc m_setPeriodic;
  // TODO: make this per-topic?
  uint32_t m_periodMs{UINT32_MAX};
  ServerStorage& m_storage;
  int m_id;

  wpi::Logger& m_logger;

  wpi::DenseMap<int, std::unique_ptr<ServerPublisher>> m_publishers;
  wpi::DenseMap<int, std::unique_ptr<ServerSubscriber>> m_subscribers;

 public:
  // meta topics
  ServerTopic* m_metaPub = nullptr;
  ServerTopic* m_metaSub = nullptr;
};

}  // namespace nt::server
