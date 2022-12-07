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
#include "ntcore_c.h"

namespace nt::net {

struct PublishMsg {
  static constexpr std::string_view kMethodStr = "publish";
  NT_Publisher pubHandle{0};
  NT_Topic topicHandle{0};  // will be 0 when coming from network
  std::string name;
  std::string typeStr;
  wpi::json properties;
  PubSubOptions options;  // will be empty when coming from network
};

struct UnpublishMsg {
  static constexpr std::string_view kMethodStr = "unpublish";
  NT_Publisher pubHandle{0};
  NT_Topic topicHandle{0};  // will be 0 when coming from network
};

struct SetPropertiesMsg {
  static constexpr std::string_view kMethodStr = "setproperties";
  NT_Topic topicHandle{0};  // will be 0 when coming from network
  std::string name;
  wpi::json update;
};

struct SubscribeMsg {
  static constexpr std::string_view kMethodStr = "subscribe";
  NT_Subscriber subHandle{0};
  std::vector<std::string> topicNames;
  PubSubOptions options;
};

struct UnsubscribeMsg {
  static constexpr std::string_view kMethodStr = "unsubscribe";
  NT_Subscriber subHandle{0};
};

struct ClientValueMsg {
  NT_Publisher pubHandle{0};
  Value value;
};

struct ClientMessage {
  using Contents =
      std::variant<std::monostate, PublishMsg, UnpublishMsg, SetPropertiesMsg,
                   SubscribeMsg, UnsubscribeMsg, ClientValueMsg>;
  Contents contents;
};

struct AnnounceMsg {
  static constexpr std::string_view kMethodStr = "announce";
  std::string name;
  int64_t id{0};
  std::string typeStr;
  std::optional<int64_t> pubuid;
  wpi::json properties;
};

struct UnannounceMsg {
  static constexpr std::string_view kMethodStr = "unannounce";
  std::string name;
  int64_t id{0};
};

struct PropertiesUpdateMsg {
  static constexpr std::string_view kMethodStr = "properties";
  std::string name;
  wpi::json update;
  bool ack;
};

struct ServerValueMsg {
  NT_Topic topic{0};
  Value value;
};

struct ServerMessage {
  using Contents = std::variant<std::monostate, AnnounceMsg, UnannounceMsg,
                                PropertiesUpdateMsg, ServerValueMsg>;
  Contents contents;
};

}  // namespace nt::net
