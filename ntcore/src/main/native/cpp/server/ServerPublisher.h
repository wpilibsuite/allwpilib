// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace nt::server {

class ServerClient;
struct ServerTopic;

class ServerPublisher {
 public:
  ServerPublisher(std::string_view clientName, ServerTopic* topic,
                  int64_t pubuid)
      : m_clientName{clientName}, m_topic{topic}, m_pubuid{pubuid} {
    UpdateMeta();
  }
  ServerPublisher(const ServerPublisher&) = delete;
  ServerPublisher& operator=(const ServerPublisher&) = delete;

  ServerTopic* GetTopic() const { return m_topic; }
  std::span<const uint8_t> GetMetaClientData() const { return m_metaClient; }
  std::span<const uint8_t> GetMetaTopicData() const { return m_metaTopic; }

 private:
  void UpdateMeta();

  std::string m_clientName;
  ServerTopic* m_topic;
  int64_t m_pubuid;
  std::vector<uint8_t> m_metaClient;
  std::vector<uint8_t> m_metaTopic;
};

}  // namespace nt::server
