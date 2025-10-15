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

#include "PubSubOptions.h"
#include "server/Constants.h"

namespace nt::server {

class ServerClient;
struct ServerTopic;

class ServerSubscriber {
 public:
  ServerSubscriber(std::string_view clientName,
                   std::span<const std::string> topicNames, int64_t subuid,
                   const PubSubOptionsImpl& options)
      : m_clientName{clientName},
        m_topicNames{topicNames.begin(), topicNames.end()},
        m_subuid{subuid},
        m_options{options},
        m_periodMs(std::lround(options.periodicMs / 10.0) * 10) {
    UpdateMeta();
    if (m_periodMs < kMinPeriodMs) {
      m_periodMs = kMinPeriodMs;
    }
  }
  ServerSubscriber(const ServerSubscriber&) = delete;
  ServerSubscriber& operator=(const ServerSubscriber&) = delete;

  void Update(std::span<const std::string> topicNames_,
              const PubSubOptionsImpl& options_) {
    m_topicNames = {topicNames_.begin(), topicNames_.end()};
    m_options = options_;
    UpdateMeta();
    m_periodMs = std::lround(options_.periodicMs / 10.0) * 10;
    if (m_periodMs < kMinPeriodMs) {
      m_periodMs = kMinPeriodMs;
    }
  }

  bool Matches(std::string_view name, bool special);

  const PubSubOptions& GetOptions() const { return m_options; }
  uint32_t GetPeriodMs() const { return m_periodMs; }

  std::span<const uint8_t> GetMetaClientData() const { return m_metaClient; }
  std::span<const uint8_t> GetMetaTopicData() const { return m_metaTopic; }

 private:
  void UpdateMeta();

  std::string m_clientName;
  std::vector<std::string> m_topicNames;
  int64_t m_subuid;
  PubSubOptionsImpl m_options;
  std::vector<uint8_t> m_metaClient;
  std::vector<uint8_t> m_metaTopic;
  // in options as double, but copy here as integer; rounded to the nearest
  // 10 ms
  uint32_t m_periodMs;
};

}  // namespace nt::server
