// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerImpl.h"

#include <stdint.h>

#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <wpi/MessagePack.h>

#include "Log.h"
#include "server/MessagePackWriter.h"
#include "server/ServerClient3.h"
#include "server/ServerClient4.h"
#include "server/ServerClientLocal.h"

using namespace nt;
using namespace nt::server;
using namespace mpack;

ServerImpl::ServerImpl(wpi::Logger& logger)
    : m_logger{logger},
      m_storage{logger, [this](ServerTopic* topic, ServerClient* client) {
                  SendAnnounce(topic, client);
                }} {
  // local is client 0
  m_clients.emplace_back(
      std::make_unique<ServerClientLocal>(m_storage, 0, logger));
  m_localClient = static_cast<ServerClientLocal*>(m_clients.back().get());

  // create server meta topics
  m_metaClients = m_storage.CreateMetaTopic("$clients");
}

std::pair<std::string, int> ServerImpl::AddClient(std::string_view name,
                                                  std::string_view connInfo,
                                                  bool local,
                                                  net::WireConnection& wire,
                                                  SetPeriodicFunc setPeriodic) {
  if (name.empty()) {
    name = "NT4";
  }
  size_t index = GetEmptyClientSlot();

  // ensure name is unique by suffixing index
  std::string dedupName = fmt::format("{}@{}", name, index);

  auto& clientData = m_clients[index];
  clientData = std::make_unique<ServerClient4>(dedupName, connInfo, local, wire,
                                               std::move(setPeriodic),
                                               m_storage, index, m_logger);

  DEBUG3("AddClient('{}', '{}') -> {}", name, connInfo, index);
  return {std::move(dedupName), index};
}

int ServerImpl::AddClient3(std::string_view connInfo, bool local,
                           net3::WireConnection3& wire,
                           Connected3Func connected,
                           SetPeriodicFunc setPeriodic) {
  size_t index = GetEmptyClientSlot();

  m_clients[index] = std::make_unique<ServerClient3>(
      connInfo, local, wire, std::move(connected), std::move(setPeriodic),
      m_storage, index, m_logger);

  DEBUG3("AddClient3('{}') -> {}", connInfo, index);
  return index;
}

std::shared_ptr<void> ServerImpl::RemoveClient(int clientId) {
  DEBUG3("RemoveClient({})", clientId);
  auto& client = m_clients[clientId];
  if (client) {
    m_storage.RemoveClient(client.get());
  }
  return std::move(client);
}

size_t ServerImpl::GetEmptyClientSlot() {
  size_t size = m_clients.size();
  // find an empty slot
  // just do a linear search as number of clients is typically small (<10)
  for (size_t i = 0, end = size; i < end; ++i) {
    if (!m_clients[i]) {
      return i;
    }
  }
  m_clients.emplace_back();
  return size;
}

void ServerImpl::SendAnnounce(ServerTopic* topic, ServerClient* client) {
  for (auto&& aClient : m_clients) {
    if (!aClient) {
      continue;
    }

    // look for subscriber matching prefixes
    wpi::SmallVector<ServerSubscriber*, 16> subscribersBuf;
    auto subscribers =
        aClient->GetSubscribers(topic->name, topic->special, subscribersBuf);

    // don't announce to this client if no subscribers
    if (subscribers.empty()) {
      continue;
    }

    auto& tcd = topic->clients[aClient.get()];
    bool added = false;
    for (auto subscriber : subscribers) {
      if (tcd.AddSubscriber(subscriber)) {
        added = true;
      }
    }
    if (added) {
      aClient->UpdatePeriod(tcd, topic);
    }

    if (aClient.get() == client) {
      continue;  // don't announce to requesting client again
    }

    DEBUG4("client {}: announce {}", aClient->GetId(), topic->name);
    aClient->SendAnnounce(topic, std::nullopt);
  }
}

void ServerImpl::UpdateMetaClients(const std::vector<ConnectionInfo>& conns) {
  Writer w;
  mpack_start_array(&w, conns.size());
  for (auto&& conn : conns) {
    mpack_start_map(&w, 3);
    mpack_write_str(&w, "id");
    mpack_write_str(&w, conn.remote_id);
    mpack_write_str(&w, "conn");
    mpack_write_str(&w, fmt::format("{}:{}", conn.remote_ip, conn.remote_port));
    mpack_write_str(&w, "ver");
    mpack_write_u16(&w, conn.protocol_version);
    mpack_finish_map(&w);
  }
  mpack_finish_array(&w);
  if (mpack_writer_destroy(&w) == mpack_ok) {
    m_storage.SetValue(nullptr, m_metaClients,
                       Value::MakeRaw(std::move(w.bytes)));
  } else {
    DEBUG4("failed to encode $clients");
  }
}

void ServerImpl::SendAllOutgoing(uint64_t curTimeMs, bool flush) {
  for (auto&& client : m_clients) {
    if (client) {
      client->SendOutgoing(curTimeMs, flush);
    }
  }
}

void ServerImpl::SendOutgoing(int clientId, uint64_t curTimeMs) {
  if (auto client = m_clients[clientId].get()) {
    client->SendOutgoing(curTimeMs, false);
  }
}

void ServerImpl::SetLocal(net::ServerMessageHandler* local,
                          net::ClientMessageQueue* queue) {
  DEBUG4("SetLocal()");
  m_localClient->SetLocal(local, queue);
}

bool ServerImpl::ProcessIncomingText(int clientId, std::string_view data) {
  if (auto client = m_clients[clientId].get()) {
    return client->ProcessIncomingText(data);
  } else {
    return false;
  }
}

bool ServerImpl::ProcessIncomingBinary(int clientId,
                                       std::span<const uint8_t> data) {
  if (auto client = m_clients[clientId].get()) {
    return client->ProcessIncomingBinary(data);
  } else {
    return false;
  }
}

bool ServerImpl::ProcessIncomingMessages(size_t max) {
  DEBUG4("ProcessIncomingMessages({})", max);
  bool rv = false;
  for (auto&& client : m_clients) {
    if (client && client->ProcessIncomingMessages(max)) {
      rv = true;
    }
  }
  return rv;
}

bool ServerImpl::ProcessLocalMessages(size_t max) {
  DEBUG4("ProcessLocalMessages({})", max);
  return m_localClient->ProcessIncomingMessages(max);
}

std::string ServerImpl::DumpPersistent() {
  std::string rv;
  wpi::raw_string_ostream os{rv};
  m_storage.DumpPersistent(os);
  os.flush();
  return rv;
}
