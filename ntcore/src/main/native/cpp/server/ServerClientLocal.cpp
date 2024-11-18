// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ServerClientLocal.h"

#include "server/ServerImpl.h"

using namespace nt::server;

// Suppress false positive -Wmaybe-uninitialized warning from GCC 14
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
ServerClientLocal::ServerClientLocal(ServerStorage& storage, int id,
                                     wpi::Logger& logger)
    : ServerClient4Base{"", "", true, [](uint32_t) {}, storage, id, logger} {
  // create local client meta topics
  m_metaPub = storage.CreateMetaTopic("$serverpub");
  m_metaSub = storage.CreateMetaTopic("$serversub");

  // update meta topics
  UpdateMetaClientPub();
  UpdateMetaClientSub();
}
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

void ServerClientLocal::SendValue(ServerTopic* topic, const Value& value,
                                  net::ValueSendMode mode) {
  if (m_local) {
    m_local->ServerSetValue(topic->localTopic, value);
  }
}

void ServerClientLocal::SendAnnounce(ServerTopic* topic,
                                     std::optional<int> pubuid) {
  if (m_local) {
    auto& sent = m_announceSent[topic];
    if (sent) {
      return;
    }
    sent = true;

    topic->localTopic = m_local->ServerAnnounce(topic->name, 0, topic->typeStr,
                                                topic->properties, pubuid);
  }
}

void ServerClientLocal::SendUnannounce(ServerTopic* topic) {
  if (m_local) {
    auto& sent = m_announceSent[topic];
    if (!sent) {
      return;
    }
    sent = false;
    m_local->ServerUnannounce(topic->name, topic->localTopic);
  }
}

void ServerClientLocal::SendPropertiesUpdate(ServerTopic* topic,
                                             const wpi::json& update,
                                             bool ack) {
  if (m_local) {
    if (!m_announceSent.lookup(topic)) {
      return;
    }
    m_local->ServerPropertiesUpdate(topic->name, update, ack);
  }
}
