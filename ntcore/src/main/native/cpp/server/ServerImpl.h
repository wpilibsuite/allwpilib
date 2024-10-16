// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/UidVector.h>
#include <wpi/json_fwd.h>

#include "server/Functions.h"
#include "server/ServerClient.h"
#include "server/ServerTopic.h"

namespace wpi {
class Logger;
template <typename T>
class SmallVectorImpl;
class raw_ostream;
}  // namespace wpi

namespace nt::net {
class ClientMessageQueue;
class LocalInterface;
class ServerMessageHandler;
class WireConnection;
}  // namespace nt::net

namespace nt::net3 {
class WireConnection3;
}  // namespace nt::net3

namespace nt::server {

class ServerClient;
class ServerClient3;
class ServerClient4;
class ServerClient4Base;
class ServerClientLocal;

class ServerImpl final {
  friend class ServerClient;
  friend class ServerClient3;
  friend class ServerClient4;
  friend class ServerClient4Base;
  friend class ServerClientLocal;

 public:
  explicit ServerImpl(wpi::Logger& logger);

  void SendAllOutgoing(uint64_t curTimeMs, bool flush);
  void SendOutgoing(int clientId, uint64_t curTimeMs);

  void SetLocal(net::ServerMessageHandler* local,
                net::ClientMessageQueue* queue);

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
                                        net::WireConnection& wire,
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
  wpi::Logger& m_logger;
  net::ServerMessageHandler* m_local{nullptr};
  bool m_controlReady{false};

  ServerClientLocal* m_localClient;
  std::vector<std::unique_ptr<ServerClient>> m_clients;
  wpi::UidVector<std::unique_ptr<ServerTopic>, 16> m_topics;
  wpi::StringMap<ServerTopic*> m_nameTopics;
  bool m_persistentChanged{false};

  // global meta topics (other meta topics are linked to from the specific
  // client or topic)
  ServerTopic* m_metaClients;

  void DumpPersistent(wpi::raw_ostream& os);

  // helper functions
  ServerTopic* CreateTopic(ServerClient* client, std::string_view name,
                           std::string_view typeStr,
                           const wpi::json& properties, bool special = false);
  ServerTopic* CreateMetaTopic(std::string_view name);
  void DeleteTopic(ServerTopic* topic);
  void SetProperties(ServerClient* client, ServerTopic* topic,
                     const wpi::json& update);
  void SetFlags(ServerClient* client, ServerTopic* topic, unsigned int flags);
  void SetValue(ServerClient* client, ServerTopic* topic, const Value& value);

  // update meta topic values from data structures
  void UpdateMetaClients(const std::vector<ConnectionInfo>& conns);
  void UpdateMetaTopicPub(ServerTopic* topic);
  void UpdateMetaTopicSub(ServerTopic* topic);

  void PropertiesChanged(ServerClient* client, ServerTopic* topic,
                         const wpi::json& update);
};

}  // namespace nt::server
