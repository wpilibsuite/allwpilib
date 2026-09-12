// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "aos/configuration.h"
#include "aos/events/event_loop.h"
#include "aos/protobuf/flatbuffer_to_proto.h"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/RawTopic.hpp"

namespace wpi::aosnt {

/// Tag a channel with this to publish it onto NetworkTables.
inline constexpr std::string_view kPublishTag = "nt:publish";

/**
 * Publishes AOS channels onto NetworkTables.
 *
 * A channel whose type is a table in aosnt/types/, such as
 * wpi.aosnt.Boolean, is published as the NetworkTables type the table is named
 * for, holding the table's `value`.
 *
 * Any other channel is published as protobuf. Every AOS channel carries its own
 * reflection schema, so the bridge builds a FileDescriptorProto from it,
 * registers that in NetworkTables' schema registry, and translates each message
 * on its way out. The result is an ordinary `proto:` topic, so dashboards and
 * DataLog decode it without knowing AOS is involved.
 *
 * Channels are selected by tag. An untagged channel is not bridged. Each
 * channel is published under its own name, so two tagged channels may not
 * share one.
 *
 *     { "name": "/Netcomm/Control/ControlData",
 *       "type": "mrc.proto.ProtobufControlData", "tags": ["nt:publish"] }
 *
 * publishes a `proto:mrc.proto.ProtobufControlData` topic, and
 *
 *     { "name": "/Netcomm/Control/WatchdogActive",
 *       "type": "wpi.aosnt.Boolean", "tags": ["nt:publish"] }
 *
 * publishes a `boolean` topic.
 *
 * NetworkTables to AOS is not implemented.
 *
 * This is not a realtime path. NetworkTables allocates on every publish, so an
 * event loop running this cannot go realtime with malloc denial on.
 */
class NtBridge {
 public:
  /**
   * Constructs a bridge.
   *
   * @param event_loop The event loop to subscribe on. AOS watchers cannot be
   *                   unregistered, so this must not be destroyed while the
   *                   event loop can still run.
   * @param instance The NetworkTables instance to publish to. Must outlive
   *                 this.
   * @throws std::invalid_argument if a tagged channel has no schema, or two
   *         tagged channels share a name.
   */
  NtBridge(aos::EventLoop* event_loop, wpi::nt::NetworkTableInstance instance);

  NtBridge(const NtBridge&) = delete;
  NtBridge& operator=(const NtBridge&) = delete;

  /**
   * Returns the topic name a channel is published under, which is the channel's
   * name. A NetworkTables topic has one type, and NtTypeString() carries it.
   */
  static std::string NtName(const aos::Channel* channel);

  /**
   * Returns the topic type string. For a table in aosnt/types/ it is the
   * NetworkTables type, such as "boolean". Otherwise it names the message in
   * the schema registry, such as "proto:mrc.proto.ProtobufControlData".
   */
  static std::string NtTypeString(const aos::Channel* channel);

  size_t published_channels() const { return publishers_.size(); }
  uint64_t published_messages() const { return published_messages_; }

 private:
  struct Publisher {
    const aos::Channel* channel = nullptr;
    // Set for a channel whose type is in aosnt/types/.
    wpi::nt::GenericPublisher primitive;
    // Set for any other channel.
    wpi::nt::RawPublisher publisher;
    std::unique_ptr<aos::FlatbufferToProto> translator;
    // Grown if a message does not fit.
    std::vector<uint8_t> buffer;
  };

  void AddSchema(const aos::Channel* channel);

  aos::EventLoop* const event_loop_;
  wpi::nt::NetworkTableInstance instance_;
  std::vector<std::unique_ptr<Publisher>> publishers_;

  uint64_t published_messages_ = 0;
};

}  // namespace wpi::aosnt
