// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "aos/configuration.h"
#include "aos/events/event_loop.h"
#include "wpi/aosnt/FlatbufferToProto.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/RawTopic.hpp"

namespace wpi::aosnt {

/** Tag a channel with this to publish it onto NetworkTables. */
inline constexpr std::string_view PUBLISH_TAG = "nt:publish";

/**
 * Publishes AOS channels onto NetworkTables.
 *
 * A channel whose type is a table in aosnt/types/, such as
 * wpi.aosnt.Boolean, is published as the NetworkTables type the table is named
 * for, holding the table's `value`.
 *
 * Any other channel is published as protobuf. Every AOS channel carries its own
 * reflection schema, so the bridge builds FileDescriptorProtos from it, one per
 * message and enum (see BuildFileDescriptorProtos()), registers them in
 * NetworkTables' schema registry, and translates each message on its way out.
 * The result is an ordinary `proto:` topic, so dashboards and DataLog decode it
 * without knowing AOS is involved.
 *
 * A message's descriptor has to be the only one on the network. Each process
 * registers a descriptor as a retained `/.schema/proto:<file>` topic, which it
 * skips only if that process registered the same name itself, and the server
 * keeps whichever value arrived last. Glass and DataLog load each file name
 * once, and refuse a file that defines a message they already loaded, along
 * with every file that depends on it. So a message the bridge publishes must
 * not also be published by anything else with a descriptor of its own, such as
 * robot code publishing mrc.proto.ProtobufControlData from MrcComm.proto. A
 * viewer would decode it with whichever descriptor it saw first, and lose the
 * other file. Only publishers register descriptors, so subscribing to a
 * bridged topic is fine.
 *
 * The bridge keeps its own channels to that rule. Each message is described in
 * a file of its own, named after it and built only from its definition, so
 * channels whose schemas share a message register the same bytes under the
 * same name, once. Channels whose schemas define a message differently are
 * refused when the bridge is constructed.
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
   * @param eventLoop The event loop to subscribe on. AOS watchers cannot be
   *                  unregistered, so this must not be destroyed while the
   *                  event loop can still run.
   * @param instance The NetworkTables instance to publish to. Must outlive
   *                 this.
   * @throws std::invalid_argument if a tagged channel has no schema, its schema
   *         has no protobuf equivalent, two tagged channels share a name, two
   *         tagged channels' schemas describe the same message differently, or
   *         their descriptors cannot be loaded together, such as enums in one
   *         package that share a value name.
   */
  NtBridge(aos::EventLoop* eventLoop, wpi::nt::NetworkTableInstance instance);

  NtBridge(const NtBridge&) = delete;
  NtBridge& operator=(const NtBridge&) = delete;

  /**
   * Gets the topic name a channel is published under, which is the channel's
   * name. A NetworkTables topic has one type, and GetTypeString() carries it.
   *
   * @param channel channel
   * @return topic name
   */
  static std::string GetTopicName(const aos::Channel* channel);

  /**
   * Gets the topic type string. For a table in aosnt/types/ it is the
   * NetworkTables type, such as "boolean". Otherwise it names the message in
   * the schema registry, such as "proto:mrc.proto.ProtobufControlData".
   *
   * @param channel channel
   * @return topic type string
   */
  static std::string GetTypeString(const aos::Channel* channel);

  /**
   * Gets the number of channels being published.
   *
   * @return number of channels
   */
  size_t GetPublishedChannelCount() const { return m_publishers.size(); }

  /**
   * Gets the number of messages published so far, across all channels.
   *
   * @return number of messages
   */
  uint64_t GetPublishedMessageCount() const { return m_publishedMessages; }

 private:
  struct Publisher {
    const aos::Channel* channel = nullptr;
    // Set for a channel whose type is in aosnt/types/.
    wpi::nt::GenericPublisher primitive;
    // Set for any other channel.
    wpi::nt::RawPublisher publisher;
    std::unique_ptr<FlatbufferToProto> translator;
    // Grown if a message does not fit.
    std::vector<uint8_t> buffer;
  };

  void AddSchema(const ProtoFile& file);

  aos::EventLoop* m_eventLoop;
  wpi::nt::NetworkTableInstance m_instance;
  std::vector<std::unique_ptr<Publisher>> m_publishers;

  uint64_t m_publishedMessages = 0;
};

}  // namespace wpi::aosnt
