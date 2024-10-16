// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <vector>

namespace nt::server {

class ServerClient;
struct ServerTopic;

struct ServerPublisher {
  ServerPublisher(ServerClient* client, ServerTopic* topic, int64_t pubuid)
      : client{client}, topic{topic}, pubuid{pubuid} {
    UpdateMeta();
  }

  void UpdateMeta();

  ServerClient* client;
  ServerTopic* topic;
  int64_t pubuid;
  std::vector<uint8_t> metaClient;
  std::vector<uint8_t> metaTopic;
};

}  // namespace nt::server
