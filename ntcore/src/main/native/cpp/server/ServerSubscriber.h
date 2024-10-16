// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <cmath>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <wpi/DenseMap.h>

#include "PubSubOptions.h"
#include "server/Constants.h"

namespace nt::server {

class ServerClient;
struct ServerTopic;

struct ServerSubscriber {
  ServerSubscriber(std::string_view clientName,
                   std::span<const std::string> topicNames, int64_t subuid,
                   const PubSubOptionsImpl& options)
      : clientName{clientName},
        topicNames{topicNames.begin(), topicNames.end()},
        subuid{subuid},
        options{options},
        periodMs(std::lround(options.periodicMs / 10.0) * 10) {
    UpdateMeta();
    if (periodMs < kMinPeriodMs) {
      periodMs = kMinPeriodMs;
    }
  }

  void Update(std::span<const std::string> topicNames_,
              const PubSubOptionsImpl& options_) {
    topicNames = {topicNames_.begin(), topicNames_.end()};
    options = options_;
    UpdateMeta();
    periodMs = std::lround(options_.periodicMs / 10.0) * 10;
    if (periodMs < kMinPeriodMs) {
      periodMs = kMinPeriodMs;
    }
  }

  bool Matches(std::string_view name, bool special);

  void UpdateMeta();

  std::string clientName;
  std::vector<std::string> topicNames;
  int64_t subuid;
  PubSubOptionsImpl options;
  std::vector<uint8_t> metaClient;
  std::vector<uint8_t> metaTopic;
  wpi::DenseMap<ServerTopic*, bool> topics;
  // in options as double, but copy here as integer; rounded to the nearest
  // 10 ms
  uint32_t periodMs;
};

}  // namespace nt::server
