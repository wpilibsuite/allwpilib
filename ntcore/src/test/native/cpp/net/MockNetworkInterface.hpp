// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <deque>
#include <optional>
#include <string>
#include <vector>

#include "net/NetworkInterface.hpp"
#include "wpi/nt/NetworkTableValue.hpp"
#include "wpi/util/json.hpp"

namespace wpi::nt::net {

class MockLocalStorage : public ILocalStorage {
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
    int topicId;
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

  void ServerSetValue(int topicId, const Value& value) override {
    setValueCalls.emplace_back(topicId, value);
  }

  void StartNetwork(ClientMessageHandler* network) override {
    startNetworkCalls.emplace_back(network);
  }

  void ClearNetwork() override { ++clearNetworkCalls; }

  int defaultAnnounceReturn = 0;
  std::deque<int> announceReturns;
  std::vector<AnnounceCall> announceCalls;
  std::vector<UnannounceCall> unannounceCalls;
  std::vector<PropertiesUpdateCall> propertiesUpdateCalls;
  std::vector<SetValueCall> setValueCalls;
  std::vector<ClientMessageHandler*> startNetworkCalls;
  int clearNetworkCalls = 0;
};

}  // namespace wpi::nt::net
