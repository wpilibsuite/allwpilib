// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <deque>
#include <optional>
#include <string>
#include <vector>

#include "PubSubOptions.hpp"
#include "net/MessageHandler.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/json.hpp"

namespace wpi::nt::net {

class MockClientMessageHandler : public net::ClientMessageHandler {
 public:
  struct PublishCall {
    int pubuid;
    std::string name;
    std::string typeStr;
    wpi::util::json properties;
    PubSubOptionsImpl options;
  };

  struct SetPropertiesCall {
    std::string name;
    wpi::util::json update;
  };

  struct SubscribeCall {
    int subuid;
    std::vector<std::string> prefixes;
    PubSubOptionsImpl options;
  };

  struct SetValueCall {
    int pubuid;
    Value value;
  };

  void ClientPublish(int pubuid, std::string_view name,
                     std::string_view typeStr,
                     const wpi::util::json& properties,
                     const PubSubOptionsImpl& options) override {
    publishCalls.emplace_back(pubuid, std::string{name}, std::string{typeStr},
                              properties, options);
  }

  void ClientUnpublish(int pubuid) override {
    unpublishCalls.emplace_back(pubuid);
  }

  void ClientSetProperties(std::string_view name,
                           const wpi::util::json& update) override {
    setPropertiesCalls.emplace_back(std::string{name}, update);
  }

  void ClientSubscribe(int subuid, std::span<const std::string> prefixes,
                       const PubSubOptionsImpl& options) override {
    subscribeCalls.emplace_back(
        subuid, std::vector<std::string>{prefixes.begin(), prefixes.end()},
        options);
  }

  void ClientUnsubscribe(int subuid) override {
    unsubscribeCalls.emplace_back(subuid);
  }

  void ClientSetValue(int pubuid, const Value& value) override {
    setValueCalls.emplace_back(pubuid, value);
  }

  std::vector<PublishCall> publishCalls;
  std::vector<int> unpublishCalls;
  std::vector<SetPropertiesCall> setPropertiesCalls;
  std::vector<SubscribeCall> subscribeCalls;
  std::vector<int> unsubscribeCalls;
  std::vector<SetValueCall> setValueCalls;
};

class MockServerMessageHandler : public net::ServerMessageHandler {
 public:
  struct AnnounceCall {
    std::string name;
    int id;
    std::string typeStr;
    wpi::util::json properties;
    std::optional<int> pubuid;
  };

  struct UnannounceCall {
    std::string name;
    int id;
  };

  struct PropertiesUpdateCall {
    std::string name;
    wpi::util::json update;
    bool ack;
  };

  struct SetValueCall {
    int topicuid;
    Value value;
  };

  int ServerAnnounce(std::string_view name, int id, std::string_view typeStr,
                     const wpi::util::json& properties,
                     std::optional<int> pubuid) override {
    announceCalls.emplace_back(std::string{name}, id, std::string{typeStr},
                               properties, pubuid);
    if (!announceReturns.empty()) {
      int rv = announceReturns.front();
      announceReturns.pop_front();
      return rv;
    }
    return defaultAnnounceReturn;
  }

  void ServerUnannounce(std::string_view name, int id) override {
    unannounceCalls.emplace_back(std::string{name}, id);
  }

  void ServerPropertiesUpdate(std::string_view name,
                              const wpi::util::json& update,
                              bool ack) override {
    propertiesUpdateCalls.emplace_back(std::string{name}, update, ack);
  }

  void ServerSetValue(int topicuid, const Value& value) override {
    setValueCalls.emplace_back(topicuid, value);
  }

  int defaultAnnounceReturn = 0;
  std::deque<int> announceReturns;
  std::vector<AnnounceCall> announceCalls;
  std::vector<UnannounceCall> unannounceCalls;
  std::vector<PropertiesUpdateCall> propertiesUpdateCalls;
  std::vector<SetValueCall> setValueCalls;
};

}  // namespace wpi::nt::net
