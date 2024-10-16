// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <wpi/json_fwd.h>

namespace wpi {
class raw_ostream;
}  // namespace wpi

namespace nt {
class PubSubOptionsImpl;
class Value;
}  // namespace nt

namespace nt::net {

struct ClientMessage;
struct ServerMessage;

// encoders for client text messages (avoids need to construct a Message struct)
void WireEncodePublish(wpi::raw_ostream& os, int pubuid, std::string_view name,
                       std::string_view typeStr, const wpi::json& properties);
void WireEncodeUnpublish(wpi::raw_ostream& os, int pubuid);
void WireEncodeSetProperties(wpi::raw_ostream& os, std::string_view name,
                             const wpi::json& update);
void WireEncodeSubscribe(wpi::raw_ostream& os, int subuid,
                         std::span<const std::string_view> topicNames,
                         const PubSubOptionsImpl& options);
void WireEncodeSubscribe(wpi::raw_ostream& os, int subuid,
                         std::span<const std::string> topicNames,
                         const PubSubOptionsImpl& options);
void WireEncodeUnsubscribe(wpi::raw_ostream& os, int subuid);

// encoders for server text messages (avoids need to construct a Message struct)
void WireEncodeAnnounce(wpi::raw_ostream& os, std::string_view name, int id,
                        std::string_view typeStr, const wpi::json& properties,
                        std::optional<int> pubuid);
void WireEncodeUnannounce(wpi::raw_ostream& os, std::string_view name,
                          int64_t id);
void WireEncodePropertiesUpdate(wpi::raw_ostream& os, std::string_view name,
                                const wpi::json& update, bool ack);

// Encode a single message; note text messages must be put into a
// JSON array "[msg1, msg2]" for transmission.
// Returns true if message was written
bool WireEncodeText(wpi::raw_ostream& os, const ClientMessage& msg);
bool WireEncodeText(wpi::raw_ostream& os, const ServerMessage& msg);

// encoder for binary messages
bool WireEncodeBinary(wpi::raw_ostream& os, int id, int64_t time,
                      const Value& value);

}  // namespace nt::net
