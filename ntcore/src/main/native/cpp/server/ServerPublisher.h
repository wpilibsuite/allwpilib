// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>
#include <string_view>
#include <vector>

namespace nt::server {

class ServerClient;
struct ServerTopic;

struct ServerPublisher {
  ServerPublisher(std::string_view clientName, ServerTopic* topic,
                  int64_t pubuid)
      : clientName{clientName}, topic{topic}, pubuid{pubuid} {
    UpdateMeta();
  }

  void UpdateMeta();

  std::string clientName;
  ServerTopic* topic;
  int64_t pubuid;
  std::vector<uint8_t> metaClient;
  std::vector<uint8_t> metaTopic;
};

}  // namespace nt::server
