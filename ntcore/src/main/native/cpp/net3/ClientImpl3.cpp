// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ClientImpl3.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <wpi/DenseMap.h>
#include <wpi/StringMap.h>
#include <wpi/timestamp.h>

#include "Log.h"
#include "Types_internal.h"
#include "net/Message.h"
#include "net/NetworkInterface.h"
#include "net3/WireEncoder3.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;
using namespace nt::net3;

static constexpr uint32_t kMinPeriodMs = 5;

// maximum amount of time the wire can be not ready to send another
// transmission before we close the connection
static constexpr uint32_t kWireMaxNotReadyUs = 1000000;

wpi::json ClientImpl3::Entry::SetFlags(unsigned int flags_) {
  bool wasPersistent = IsPersistent();
  flags = flags_;
  bool isPersistent = IsPersistent();
  if (isPersistent && !wasPersistent) {
    properties["persistent"] = true;
    return {{"persistent", true}};
  } else if (!isPersistent && wasPersistent) {
    properties.erase("persistent");
    return {{"persistent", wpi::json()}};
  } else {
    return wpi::json::object();
  }
}

ClientImpl3::ClientImpl3(uint64_t curTimeMs, int inst, WireConnection3& wire,
                         wpi::Logger& logger,
                         std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_wire{wire},
      m_logger{logger},
      m_setPeriodic{std::move(setPeriodic)},
      m_initTimeMs{curTimeMs},
      m_nextKeepAliveTimeMs{curTimeMs + kKeepAliveIntervalMs},
      m_decoder{*this} {}

ClientImpl3::~ClientImpl3() {
  DEBUG4("NT3 ClientImpl destroyed");
}

void ClientImpl3::ProcessIncoming(std::span<const uint8_t> data) {
  DEBUG4("received {} bytes", data.size());
  if (!m_decoder.Execute(&data)) {
    m_wire.Disconnect(m_decoder.GetError());
  }
}

void ClientImpl3::HandleLocal(std::span<const net::ClientMessage> msgs) {
  for (const auto& elem : msgs) {  // NOLINT
    // common case is value
    if (auto msg = std::get_if<net::ClientValueMsg>(&elem.contents)) {
      SetValue(msg->pubuid, msg->value);
    } else if (auto msg = std::get_if<net::PublishMsg>(&elem.contents)) {
      Publish(msg->pubuid, msg->name, msg->typeStr, msg->properties,
              msg->options);
    } else if (auto msg = std::get_if<net::UnpublishMsg>(&elem.contents)) {
      Unpublish(msg->pubuid);
    } else if (auto msg = std::get_if<net::SetPropertiesMsg>(&elem.contents)) {
      SetProperties(msg->name, msg->update);
    }
  }
}

void ClientImpl3::DoSendPeriodic(uint64_t curTimeMs, bool initial, bool flush) {
  DEBUG4("SendPeriodic({})", curTimeMs);

  // rate limit sends
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return;
  }

  auto out = m_wire.Send();

  // send keep-alive
  if (curTimeMs >= m_nextKeepAliveTimeMs) {
    if (!CheckNetworkReady(curTimeMs)) {
      return;
    }
    DEBUG4("Sending keep alive");
    WireEncodeKeepAlive(out.stream());
    // drift isn't critical here, so just go from current time
    m_nextKeepAliveTimeMs = curTimeMs + kKeepAliveIntervalMs;
  }

  // send any stored-up flags updates
  if (!m_outgoingFlags.empty()) {
    if (!CheckNetworkReady(curTimeMs)) {
      return;
    }
    for (auto&& p : m_outgoingFlags) {
      WireEncodeFlagsUpdate(out.stream(), p.first, p.second);
    }
    m_outgoingFlags.resize(0);
  }

  // send any pending updates due to be sent
  bool checkedNetwork = false;
  for (auto&& pub : m_publishers) {
    if (pub && !pub->outValues.empty() &&
        (flush || curTimeMs >= pub->nextSendMs)) {
      if (!checkedNetwork) {
        if (!CheckNetworkReady(curTimeMs)) {
          return;
        }
        checkedNetwork = true;
      }
      for (auto&& val : pub->outValues) {
        SendValue(out, pub->entry, val);
      }
      pub->outValues.resize(0);
      pub->nextSendMs = curTimeMs + pub->periodMs;
    }
  }

  if (initial) {
    DEBUG4("Sending ClientHelloDone");
    WireEncodeClientHelloDone(out.stream());
  }

  m_wire.Flush();
  m_lastSendMs = curTimeMs;
}

void ClientImpl3::SendValue(Writer& out, Entry* entry, const Value& value) {
  DEBUG4("sending value for '{}', seqnum {}", entry->name,
         entry->seqNum.value());

  // bump sequence number
  ++entry->seqNum;

  // only send assigns during initial handshake
  if (entry->id == 0xffff || m_state == kStateInitialAssignments) {
    // send assign
    WireEncodeEntryAssign(out.stream(), entry->name, entry->id,
                          entry->seqNum.value(), value, entry->flags);
  } else {
    // send update
    WireEncodeEntryUpdate(out.stream(), entry->id, entry->seqNum.value(),
                          value);
  }
}

bool ClientImpl3::CheckNetworkReady(uint64_t curTimeMs) {
  if (!m_wire.Ready()) {
    uint64_t lastFlushTime = m_wire.GetLastFlushTime();
    uint64_t now = wpi::Now();
    if (lastFlushTime != 0 && now > (lastFlushTime + kWireMaxNotReadyUs)) {
      m_wire.Disconnect("transmit stalled");
    }
    return false;
  }
  return true;
}

void ClientImpl3::Publish(int pubuid, std::string_view name,
                          std::string_view typeStr, const wpi::json& properties,
                          const PubSubOptionsImpl& options) {
  DEBUG4("Publish('{}', '{}')", name, typeStr);
  if (static_cast<unsigned int>(pubuid) >= m_publishers.size()) {
    m_publishers.resize(pubuid + 1);
  }
  auto& publisher = m_publishers[pubuid];
  if (!publisher) {
    publisher = std::make_unique<PublisherData>(GetOrNewEntry(name));
    publisher->entry->typeStr = typeStr;
    publisher->entry->type = StringToType3(typeStr);
    publisher->entry->publishers.emplace_back(publisher.get());
  }
  publisher->options = options;
  publisher->periodMs = std::lround(options.periodicMs / 10.0) * 10;
  if (publisher->periodMs < 10) {
    publisher->periodMs = 10;
  }

  // update period
  m_periodMs = std::gcd(m_periodMs, publisher->periodMs);
  m_setPeriodic(m_periodMs);
}

void ClientImpl3::Unpublish(int pubuid) {
  DEBUG4("Unpublish({})", pubuid);
  if (static_cast<unsigned int>(pubuid) >= m_publishers.size()) {
    return;
  }
  auto& publisher = m_publishers[pubuid];
  publisher->entry->publishers.erase(
      std::remove(publisher->entry->publishers.begin(),
                  publisher->entry->publishers.end(), publisher.get()),
      publisher->entry->publishers.end());
  publisher.reset();

  // loop over all publishers to update period
  m_periodMs = kKeepAliveIntervalMs + 10;
  for (auto&& pub : m_publishers) {
    if (pub) {
      m_periodMs = std::gcd(m_periodMs, pub->periodMs);
    }
  }
  m_setPeriodic(m_periodMs);
}

void ClientImpl3::SetProperties(std::string_view name,
                                const wpi::json& update) {
  DEBUG4("SetProperties({}, {})", name, update.dump());
  auto entry = GetOrNewEntry(name);
  bool updated = false;
  for (auto&& elem : update.items()) {
    entry->properties[elem.key()] = elem.value();
    if (elem.key() == "persistent") {
      if (auto val = elem.value().get_ptr<const bool*>()) {
        if (*val) {
          entry->flags |= NT_PERSISTENT;
        } else {
          entry->flags &= ~NT_PERSISTENT;
        }
        updated = true;
      }
    }
  }
  if (updated && entry->id == 0xffff) {
    m_outgoingFlags.emplace_back(entry->id, entry->flags);
  }
}

void ClientImpl3::SetValue(int pubuid, const Value& value) {
  DEBUG4("SetValue({})", pubuid);
  assert(static_cast<unsigned int>(pubuid) < m_publishers.size() &&
         m_publishers[pubuid]);
  auto& publisher = *m_publishers[pubuid];
  if (value == publisher.entry->value) {
    return;
  }
  publisher.entry->value = value;
  if (publisher.outValues.empty() || publisher.options.sendAll) {
    publisher.outValues.emplace_back(value);
  } else {
    publisher.outValues.back() = value;
  }
}

void ClientImpl3::KeepAlive() {
  DEBUG4("KeepAlive()");
  if (m_state != kStateRunning && m_state != kStateInitialAssignments) {
    m_decoder.SetError("received unexpected KeepAlive message");
    return;
  }
  // ignore
}

void ClientImpl3::ServerHelloDone() {
  DEBUG4("ServerHelloDone()");
  if (m_state != kStateInitialAssignments) {
    m_decoder.SetError("received unexpected ServerHelloDone message");
    return;
  }

  // send initial assignments
  DoSendPeriodic(m_initTimeMs, true, true);

  m_state = kStateRunning;
  m_setPeriodic(m_periodMs);
}

void ClientImpl3::ClientHelloDone() {
  DEBUG4("ClientHelloDone()");
  m_decoder.SetError("received unexpected ClientHelloDone message");
}

void ClientImpl3::ProtoUnsup(unsigned int proto_rev) {
  DEBUG4("ProtoUnsup({})", proto_rev);
  m_decoder.SetError(fmt::format("received ProtoUnsup(version={})", proto_rev));
}

void ClientImpl3::ClientHello(std::string_view self_id,
                              unsigned int proto_rev) {
  DEBUG4("ClientHello({}, {})", self_id, proto_rev);
  m_decoder.SetError("received unexpected ClientHello message");
}

void ClientImpl3::ServerHello(unsigned int flags, std::string_view self_id) {
  DEBUG4("ServerHello({}, {})", flags, self_id);
  if (m_state != kStateHelloSent) {
    m_decoder.SetError("received unexpected ServerHello message");
    return;
  }
  m_state = kStateInitialAssignments;
  m_remoteId = self_id;
  m_handshakeSucceeded();
  m_handshakeSucceeded = nullptr;  // no longer required
}

void ClientImpl3::EntryAssign(std::string_view name, unsigned int id,
                              unsigned int seq_num, const Value& value,
                              unsigned int flags) {
  DEBUG4("EntryAssign({}, {}, {}, value, {})", name, id, seq_num, flags);
  if (m_state != kStateInitialAssignments && m_state != kStateRunning) {
    m_decoder.SetError("received unexpected EntryAssign message");
    return;
  }
  auto entry = GetOrNewEntry(name);
  bool flagsChanged = entry->flags != flags;
  bool typeChanged;
  bool valueChanged;

  // don't update value if we locally published a "strong" value
  if (m_state == kStateInitialAssignments && entry->value &&
      entry->value.server_time() != 0) {
    typeChanged = false;
    valueChanged = false;
  } else {
    typeChanged = entry->type != value.type();
    valueChanged = entry->value != value;
    if (m_state == kStateInitialAssignments) {
      // remove outgoing during initial assignments so we don't get out of sync
      for (auto publisher : entry->publishers) {
        publisher->outValues.clear();
      }
    }
  }

  entry->id = id;
  entry->seqNum = SequenceNumber{seq_num};
  entry->SetFlags(flags);
  if (typeChanged) {
    entry->type = value.type();
    entry->typeStr = TypeToString(value.type());
  }
  if (valueChanged) {
    entry->value = value;
  }

  // add to id map
  if (id >= m_idMap.size()) {
    m_idMap.resize(id + 1);
  }
  m_idMap[id] = entry;

  if (m_local) {
    // XXX: need to handle type change specially? (e.g. with unannounce)
    if (!entry->topic || flagsChanged || typeChanged) {
      DEBUG4("NetworkAnnounce({}, {})", name, entry->typeStr);
      entry->topic = m_local->ServerAnnounce(name, 0, entry->typeStr,
                                             entry->properties, std::nullopt);
    }
    if (valueChanged) {
      m_local->ServerSetValue(entry->topic.value(), entry->value);
    }
  }
}

void ClientImpl3::EntryUpdate(unsigned int id, unsigned int seq_num,
                              const Value& value) {
  DEBUG4("EntryUpdate({}, {}, value)", id, seq_num);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received EntryUpdate message before ServerHelloDone");
    return;
  }
  if (auto entry = LookupId(id)) {
    entry->value = value;
    if (m_local && entry->topic) {
      m_local->ServerSetValue(entry->topic.value(), entry->value);
    }
  }
}

void ClientImpl3::FlagsUpdate(unsigned int id, unsigned int flags) {
  DEBUG4("FlagsUpdate({}, {})", id, flags);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received FlagsUpdate message before ServerHelloDone");
    return;
  }
  if (auto entry = LookupId(id)) {
    wpi::json update = entry->SetFlags(flags);
    if (!update.empty() && m_local) {
      m_local->ServerPropertiesUpdate(entry->name, update, false);
    }
  }

  // erase any outgoing flags updates
  m_outgoingFlags.erase(
      std::remove_if(m_outgoingFlags.begin(), m_outgoingFlags.end(),
                     [&](const auto& p) { return p.first == id; }),
      m_outgoingFlags.end());
}

void ClientImpl3::EntryDelete(unsigned int id) {
  DEBUG4("EntryDelete({})", id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received EntryDelete message before ServerHelloDone");
    return;
  }
  if (auto entry = LookupId(id)) {
    m_idMap[id] = nullptr;
    // set id to 0xffff so any future local setvalue will result in assign
    entry->id = 0xffff;
    entry->value = Value{};

    // if we have no local publishers, unannounce
    if (entry->publishers.empty() && m_local && entry->topic) {
      m_local->ServerUnannounce(entry->name, entry->topic.value());
    }
  }

  // erase any outgoing flags updates
  m_outgoingFlags.erase(
      std::remove_if(m_outgoingFlags.begin(), m_outgoingFlags.end(),
                     [&](const auto& p) { return p.first == id; }),
      m_outgoingFlags.end());
}

void ClientImpl3::ClearEntries() {
  DEBUG4("ClearEntries()");
  if (m_state != kStateRunning) {
    m_decoder.SetError("received ClearEntries message before ServerHelloDone");
    return;
  }
  for (auto& entry : m_idMap) {
    if (entry && entry->id != 0xffff && !entry->IsPersistent()) {
      entry->id = 0xffff;
      entry->value = Value{};

      // if we have no local publishers, unannounce
      if (entry->publishers.empty() && m_local && entry->topic) {
        m_local->ServerUnannounce(entry->name, entry->topic.value());
      }

      entry = nullptr;  // clear id mapping
    }
  }

  // erase all outgoing flags updates
  m_outgoingFlags.resize(0);
}

void ClientImpl3::Start(std::string_view selfId,
                        std::function<void()> succeeded) {
  if (m_state != kStateInitial) {
    return;
  }
  m_handshakeSucceeded = std::move(succeeded);
  auto writer = m_wire.Send();
  WireEncodeClientHello(writer.stream(), selfId, 0x0300);
  m_wire.Flush();
  m_state = kStateHelloSent;
}
