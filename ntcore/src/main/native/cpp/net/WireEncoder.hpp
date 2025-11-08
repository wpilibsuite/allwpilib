// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "wpi/util/json_fwd.hpp"

namespace wpi::util {
class raw_ostream;
}  // namespace wpi

namespace wpi::nt {
class PubSubOptionsImpl;
class Value;
}  // namespace wpi::nt

namespace wpi::nt::net {

struct ClientMessage;
struct ServerMessage;

// encoders for client text messages (avoids need to construct a Message struct)
void WireEncodePublish(wpi::util::raw_ostream& os, int pubuid, std::string_view name,
                       std::string_view typeStr, const wpi::util::json& properties);
void WireEncodeUnpublish(wpi::util::raw_ostream& os, int pubuid);
void WireEncodeSetProperties(wpi::util::raw_ostream& os, std::string_view name,
                             const wpi::util::json& update);
void WireEncodeSubscribe(wpi::util::raw_ostream& os, int subuid,
                         std::span<const std::string_view> topicNames,
                         const PubSubOptionsImpl& options);
void WireEncodeSubscribe(wpi::util::raw_ostream& os, int subuid,
                         std::span<const std::string> topicNames,
                         const PubSubOptionsImpl& options);
void WireEncodeUnsubscribe(wpi::util::raw_ostream& os, int subuid);

// encoders for server text messages (avoids need to construct a Message struct)
void WireEncodeAnnounce(wpi::util::raw_ostream& os, std::string_view name, int id,
                        std::string_view typeStr, const wpi::util::json& properties,
                        std::optional<int> pubuid);
void WireEncodeUnannounce(wpi::util::raw_ostream& os, std::string_view name,
                          int64_t id);
void WireEncodePropertiesUpdate(wpi::util::raw_ostream& os, std::string_view name,
                                const wpi::util::json& update, bool ack);

// Encode a single message; note text messages must be put into a
// JSON array "[msg1, msg2]" for transmission.
// Returns true if message was written
bool WireEncodeText(wpi::util::raw_ostream& os, const ClientMessage& msg);
bool WireEncodeText(wpi::util::raw_ostream& os, const ServerMessage& msg);

// encoder for binary messages
bool WireEncodeBinary(wpi::util::raw_ostream& os, int id, int64_t time,
                      const Value& value);

}  // namespace wpi::nt::net
