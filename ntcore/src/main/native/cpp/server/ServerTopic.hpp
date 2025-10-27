// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <utility>

#include <wpi/util/DenseMap.hpp>
#include <wpi/util/SmallPtrSet.hpp>
#include <wpi/util/json.hpp>

#include "net/NetworkOutgoingQueue.hpp"
#include "server/ServerPublisher.hpp"
#include "server/ServerSubscriber.hpp"
#include "wpi/nt/NetworkTableValue.hpp"

namespace wpi::util {
class Logger;
}  // namespace wpi::util

namespace wpi::nt::server {

class ServerClient;

struct TopicClientData {
  wpi::util::SmallPtrSet<ServerPublisher*, 2> publishers;
  wpi::util::SmallPtrSet<ServerSubscriber*, 2> subscribers;
  net::ValueSendMode sendMode = net::ValueSendMode::kDisabled;

  bool AddSubscriber(ServerSubscriber* sub) {
    bool added = subscribers.insert(sub).second;
    auto& options = sub->GetOptions();
    if (!options.topicsOnly) {
      if (options.sendAll) {
        sendMode = net::ValueSendMode::kAll;
      } else if (sendMode == net::ValueSendMode::kDisabled) {
        sendMode = net::ValueSendMode::kNormal;
      }
    }
    return added;
  }
};

struct ServerTopic {
  ServerTopic(wpi::util::Logger& logger, std::string_view name,
              std::string_view typeStr)
      : m_logger{logger}, name{name}, typeStr{typeStr} {}
  ServerTopic(wpi::util::Logger& logger, std::string_view name,
              std::string_view typeStr, wpi::util::json properties)
      : m_logger{logger},
        name{name},
        typeStr{typeStr},
        properties(std::move(properties)) {
    RefreshProperties();
  }
  ServerTopic(const ServerTopic&) = delete;
  ServerTopic& operator=(const ServerTopic&) = delete;

  bool IsPublished() const {
    return persistent || retained || publisherCount != 0;
  }

  // returns true if properties changed
  bool SetProperties(const wpi::util::json& update);
  void RefreshProperties();
  bool SetFlags(unsigned int flags_);

  wpi::util::Logger& m_logger;  // Must be m_logger for WARN macro to work
  std::string name;
  unsigned int id;
  Value lastValue;
  ServerClient* lastValueClient = nullptr;
  std::string typeStr;
  wpi::util::json properties = wpi::util::json::object();
  unsigned int publisherCount{0};
  bool persistent{false};
  bool retained{false};
  bool cached{true};
  bool special{false};
  int localTopic{0};

  void AddPublisher(ServerClient* client, ServerPublisher* pub) {
    if (clients[client].publishers.insert(pub).second) {
      ++publisherCount;
    }
  }

  void RemovePublisher(ServerClient* client, ServerPublisher* pub) {
    if (clients[client].publishers.erase(pub)) {
      --publisherCount;
    }
  }

  wpi::util::SmallDenseMap<ServerClient*, TopicClientData, 4> clients;

  // meta topics
  ServerTopic* metaPub = nullptr;
  ServerTopic* metaSub = nullptr;
};

}  // namespace wpi::nt::server
