// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

#include <wpi/DenseMap.h>

#include "MessageHandler.h"
#include "NetworkOutgoingQueue.h"
#include "NetworkPing.h"
#include "PubSubOptions.h"
#include "WireConnection.h"
#include "WireDecoder.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt {
class PubSubOptionsImpl;
class Value;
}  // namespace nt

namespace nt::net {

struct ClientMessage;
class WireConnection;

class ClientImpl final : private ServerMessageHandler {
 public:
  ClientImpl(
      uint64_t curTimeMs, WireConnection& wire, wpi::Logger& logger,
      std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
          timeSyncUpdated,
      std::function<void(uint32_t repeatMs)> setPeriodic);

  void ProcessIncomingText(std::string_view data);
  void ProcessIncomingBinary(uint64_t curTimeMs, std::span<const uint8_t> data);
  void HandleLocal(std::span<ClientMessage> msgs);

  void SendOutgoing(uint64_t curTimeMs, bool flush);

  void SetLocal(ServerMessageHandler* local) { m_local = local; }
  void SendInitial();

 private:
  struct PublisherData {
    PubSubOptionsImpl options;
    // in options as double, but copy here as integer; rounded to the nearest
    // 10 ms
    uint32_t periodMs;
  };

  void UpdatePeriodic();

  // ServerMessageHandler interface
  int ServerAnnounce(std::string_view name, int id, std::string_view typeStr,
                     const wpi::json& properties,
                     std::optional<int> pubuid) final;
  void ServerUnannounce(std::string_view name, int id) final;
  void ServerPropertiesUpdate(std::string_view name, const wpi::json& update,
                              bool ack) final;
  void ServerSetValue(int topicId, const Value& value) final;

  void Publish(int pubuid, std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptionsImpl& options);
  void Unpublish(int pubuid, ClientMessage&& msg);
  void SetValue(int pubuid, const Value& value);

  WireConnection& m_wire;
  wpi::Logger& m_logger;
  ServerMessageHandler* m_local{nullptr};
  std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
      m_timeSyncUpdated;
  std::function<void(uint32_t repeatMs)> m_setPeriodic;

  // indexed by publisher index
  std::vector<std::unique_ptr<PublisherData>> m_publishers;

  // indexed by server-provided topic id
  wpi::DenseMap<int, int> m_topicMap;

  // ping
  NetworkPing m_ping;

  // timestamp handling
  static constexpr uint32_t kRttIntervalMs = 3000;
  uint64_t m_nextPingTimeMs{0};
  uint64_t m_pongTimeMs{0};
  uint32_t m_rtt2Us{UINT32_MAX};
  bool m_haveTimeOffset{false};

  // periodic sweep handling
  static constexpr uint32_t kMinPeriodMs = 5;
  static constexpr uint32_t kMaxPeriodMs = NetworkPing::kPingIntervalMs;
  uint32_t m_periodMs{kMaxPeriodMs};

  // outgoing queue
  NetworkOutgoingQueue<ClientMessage> m_outgoing;
};

}  // namespace nt::net
