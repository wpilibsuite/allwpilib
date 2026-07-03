// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <deque>
#include <optional>
#include <string>
#include <variant>
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

    bool operator==(const PublishCall&) const = default;
  };

  struct UnpublishCall {
    int pubuid;

    bool operator==(const UnpublishCall&) const = default;
  };

  struct SetPropertiesCall {
    std::string name;
    wpi::util::json update;

    bool operator==(const SetPropertiesCall&) const = default;
  };

  struct SubscribeCall {
    int subuid;
    std::vector<std::string> prefixes;
    PubSubOptionsImpl options;

    bool operator==(const SubscribeCall&) const = default;
  };

  struct UnsubscribeCall {
    int subuid;

    bool operator==(const UnsubscribeCall&) const = default;
  };

  struct SetValueCall {
    int pubuid;
    Value value;

    bool operator==(const SetValueCall&) const = default;
  };

  using Call = std::variant<PublishCall, UnpublishCall, SetPropertiesCall,
                            SubscribeCall, UnsubscribeCall, SetValueCall>;

  void ClientPublish(int pubuid, std::string_view name,
                     std::string_view typeStr,
                     const wpi::util::json& properties,
                     const PubSubOptionsImpl& options) override {
    publishCalls.emplace_back(pubuid, std::string{name}, std::string{typeStr},
                              properties, options);
    calls.emplace_back(publishCalls.back());
  }

  void ClientUnpublish(int pubuid) override {
    unpublishCalls.emplace_back(pubuid);
    calls.emplace_back(UnpublishCall{pubuid});
  }

  void ClientSetProperties(std::string_view name,
                           const wpi::util::json& update) override {
    setPropertiesCalls.emplace_back(std::string{name}, update);
    calls.emplace_back(setPropertiesCalls.back());
  }

  void ClientSubscribe(int subuid, std::span<const std::string> prefixes,
                       const PubSubOptionsImpl& options) override {
    subscribeCalls.emplace_back(
        subuid, std::vector<std::string>{prefixes.begin(), prefixes.end()},
        options);
    calls.emplace_back(subscribeCalls.back());
  }

  void ClientUnsubscribe(int subuid) override {
    unsubscribeCalls.emplace_back(subuid);
    calls.emplace_back(UnsubscribeCall{subuid});
  }

  void ClientSetValue(int pubuid, const Value& value) override {
    setValueCalls.emplace_back(pubuid, value);
    calls.emplace_back(setValueCalls.back());
  }

  std::vector<Call> calls;
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

    bool operator==(const AnnounceCall&) const = default;
  };

  struct UnannounceCall {
    std::string name;
    int id;

    bool operator==(const UnannounceCall&) const = default;
  };

  struct PropertiesUpdateCall {
    std::string name;
    wpi::util::json update;
    bool ack;

    bool operator==(const PropertiesUpdateCall&) const = default;
  };

  struct SetValueCall {
    int topicuid;
    Value value;

    bool operator==(const SetValueCall&) const = default;
  };

  using Call = std::variant<AnnounceCall, UnannounceCall, PropertiesUpdateCall,
                            SetValueCall>;

  int ServerAnnounce(std::string_view name, int id, std::string_view typeStr,
                     const wpi::util::json& properties,
                     std::optional<int> pubuid) override {
    announceCalls.emplace_back(std::string{name}, id, std::string{typeStr},
                               properties, pubuid);
    calls.emplace_back(announceCalls.back());
    if (!announceReturns.empty()) {
      int rv = announceReturns.front();
      announceReturns.pop_front();
      return rv;
    }
    return defaultAnnounceReturn;
  }

  void ServerUnannounce(std::string_view name, int id) override {
    unannounceCalls.emplace_back(std::string{name}, id);
    calls.emplace_back(unannounceCalls.back());
  }

  void ServerPropertiesUpdate(std::string_view name,
                              const wpi::util::json& update,
                              bool ack) override {
    propertiesUpdateCalls.emplace_back(std::string{name}, update, ack);
    calls.emplace_back(propertiesUpdateCalls.back());
  }

  void ServerSetValue(int topicuid, const Value& value) override {
    setValueCalls.emplace_back(topicuid, value);
    calls.emplace_back(setValueCalls.back());
  }

  int defaultAnnounceReturn = 0;
  std::deque<int> announceReturns;
  std::vector<Call> calls;
  std::vector<AnnounceCall> announceCalls;
  std::vector<UnannounceCall> unannounceCalls;
  std::vector<PropertiesUpdateCall> propertiesUpdateCalls;
  std::vector<SetValueCall> setValueCalls;
};

}  // namespace wpi::nt::net
