// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "NetworkInterface.h"
#include "net3/WireConnection3.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net3 {
class WireConnection3;
}  // namespace nt::net3

namespace nt::net {

struct ClientMessage;
class LocalInterface;
class ServerStartup;
class WireConnection;

class ServerImpl final {
  friend class ServerStartup;

 public:
  using SetPeriodicFunc = std::function<void(uint32_t repeatMs)>;
  using Connected3Func =
      std::function<void(std::string_view name, uint16_t proto)>;

  explicit ServerImpl(wpi::Logger& logger);
  ~ServerImpl();

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
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

class ServerStartup final : public NetworkStartupInterface {
 public:
  explicit ServerStartup(ServerImpl& server) : m_server{server} {}

  // NetworkStartupInterface interface
  void Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options) final;
  void Subscribe(NT_Subscriber subHandle,
                 std::span<const std::string> topicNames,
                 const PubSubOptions& options) final;
  void SetValue(NT_Publisher pubHandle, const Value& value) final;

 private:
  ServerImpl& m_server;
};

}  // namespace nt::net
