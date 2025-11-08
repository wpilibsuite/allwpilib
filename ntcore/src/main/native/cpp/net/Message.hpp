// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <wpi/json.h>

#include "PubSubOptions.h"
#include "networktables/NetworkTableValue.h"

namespace nt::net {

#if __GNUC__ >= 13
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

struct PublishMsg {
  static constexpr std::string_view kMethodStr = "publish";
  int pubuid{0};
  std::string name;
  std::string typeStr;
  wpi::json properties;
  PubSubOptionsImpl options;  // will be empty when coming from network
};

struct UnpublishMsg {
  static constexpr std::string_view kMethodStr = "unpublish";
  int pubuid{0};
};

struct SetPropertiesMsg {
  static constexpr std::string_view kMethodStr = "setproperties";
  std::string name;
  wpi::json update;
};

struct SubscribeMsg {
  static constexpr std::string_view kMethodStr = "subscribe";
  int subuid{0};
  std::vector<std::string> topicNames;
  PubSubOptionsImpl options;
};

struct UnsubscribeMsg {
  static constexpr std::string_view kMethodStr = "unsubscribe";
  int subuid{0};
};

struct ClientValueMsg {
  int pubuid{0};
  Value value;
};

#if __GNUC__ >= 13
#pragma GCC diagnostic pop
#endif

struct ClientMessage {
  using Contents =
      std::variant<std::monostate, PublishMsg, UnpublishMsg, SetPropertiesMsg,
                   SubscribeMsg, UnsubscribeMsg, ClientValueMsg>;
  using ValueMsg = ClientValueMsg;
  Contents contents;
};

struct AnnounceMsg {
  static constexpr std::string_view kMethodStr = "announce";
  std::string name;
  int id{0};
  std::string typeStr;
  std::optional<int> pubuid;
  wpi::json properties;
};

struct UnannounceMsg {
  static constexpr std::string_view kMethodStr = "unannounce";
  std::string name;
  int id{0};
};

struct PropertiesUpdateMsg {
  static constexpr std::string_view kMethodStr = "properties";
  std::string name;
  wpi::json update;
  bool ack;
};

struct ServerValueMsg {
  int topic{0};
  Value value;
};

struct ServerMessage {
  using Contents = std::variant<std::monostate, AnnounceMsg, UnannounceMsg,
                                PropertiesUpdateMsg, ServerValueMsg>;
  using ValueMsg = ServerValueMsg;
  Contents contents;
};

}  // namespace nt::net
