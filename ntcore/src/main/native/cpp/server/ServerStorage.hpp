// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <utility>

#include <wpi/StringMap.h>
#include <wpi/UidVector.h>
#include <wpi/json_fwd.h>

#include "server/ServerTopic.h"

namespace wpi {
class Logger;
class raw_ostream;
}  // namespace wpi

namespace nt::server {

class ServerClient;

class ServerStorage final {
 public:
  ServerStorage(wpi::Logger& logger,
                std::function<void(ServerTopic* topic, ServerClient* client)>
                    sendAnnounce)
      : m_logger{logger}, m_sendAnnounce{std::move(sendAnnounce)} {}
  ServerStorage(const ServerStorage&) = delete;
  ServerStorage& operator=(const ServerStorage&) = delete;

  ServerTopic* CreateTopic(ServerClient* client, std::string_view name,
                           std::string_view typeStr,
                           const wpi::json& properties, bool special = false);
  ServerTopic* CreateMetaTopic(std::string_view name);
  void DeleteTopic(ServerTopic* topic);
  void SetProperties(ServerClient* client, ServerTopic* topic,
                     const wpi::json& update);
  void SetFlags(ServerClient* client, ServerTopic* topic, unsigned int flags);
  void SetValue(ServerClient* client, ServerTopic* topic, const Value& value);

  void RemoveClient(ServerClient* client);

  void PropertiesChanged(ServerClient* client, ServerTopic* topic,
                         const wpi::json& update);

  ServerTopic* GetTopic(unsigned int id) const {
    return id < m_topics.size() ? m_topics[id].get() : nullptr;
  }
  ServerTopic* GetTopic(std::string_view name) const {
    auto it = m_nameTopics.find(name);
    if (it == m_nameTopics.end()) {
      return nullptr;
    }
    return it->second;
  }

  // Approximate upper bound, not exact quantity
  size_t GetNumTopics() const { return m_topics.size(); }

  void ForEachTopic(std::invocable<ServerTopic*> auto&& func) const {
    for (auto&& topic : m_topics) {
      func(topic.get());
    }
  }

  // update meta topic values from data structures
  void UpdateMetaTopicPub(ServerTopic* topic);
  void UpdateMetaTopicSub(ServerTopic* topic);

  // if any persistent values changed since the last call to this function
  bool PersistentChanged() {
    bool rv = m_persistentChanged;
    m_persistentChanged = false;
    return rv;
  }

  void DumpPersistent(wpi::raw_ostream& os);
  // returns newline-separated errors
  std::string LoadPersistent(std::string_view in);

 private:
  wpi::Logger& m_logger;
  std::function<void(ServerTopic* topic, ServerClient* client)> m_sendAnnounce;

  wpi::UidVector<std::unique_ptr<ServerTopic>, 16> m_topics;
  wpi::StringMap<ServerTopic*> m_nameTopics;
  bool m_persistentChanged{false};
};

}  // namespace nt::server
