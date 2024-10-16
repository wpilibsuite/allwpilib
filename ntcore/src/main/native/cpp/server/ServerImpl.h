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

#include "server/Functions.h"
#include "server/ServerClient.h"
#include "server/ServerStorage.h"

namespace wpi {
class Logger;
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

class ServerClientLocal;
struct ServerTopic;

class ServerImpl final {
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

  void ConnectionsChanged(const std::vector<ConnectionInfo>& conns) {
    UpdateMetaClients(conns);
  }

  // if any persistent values changed since the last call to this function
  bool PersistentChanged() { return m_storage.PersistentChanged(); }

  std::string DumpPersistent();
  // returns newline-separated errors
  std::string LoadPersistent(std::string_view in) {
    return m_storage.LoadPersistent(in);
  }

 private:
  wpi::Logger& m_logger;

  ServerClientLocal* m_localClient;
  std::vector<std::unique_ptr<ServerClient>> m_clients;

  ServerStorage m_storage;

  // global meta topics (other meta topics are linked to from the specific
  // client or topic)
  ServerTopic* m_metaClients;

  size_t GetEmptyClientSlot();
  void SendAnnounce(ServerTopic* topic, ServerClient* client);
  void UpdateMetaClients(const std::vector<ConnectionInfo>& conns);
};

}  // namespace nt::server
